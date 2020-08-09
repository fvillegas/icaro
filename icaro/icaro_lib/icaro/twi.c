#include <avr/interrupt.h>
#include <util/twi.h>

#include <avr/io.h>
#include <avr/sfr_defs.h>

#include "twi.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

static volatile uint8_t twi_state;
static volatile uint8_t twi_send_stop;
static volatile uint8_t twi_in_rep_start;
static volatile uint8_t twi_slarw;
static volatile uint8_t twi_error;

static volatile uint8_t twi_state;
static volatile uint8_t twi_slarw;

static uint8_t twi_master_buffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_master_buffer_index;
static volatile uint8_t twi_master_buffer_length;

static uint8_t twi_tx_buffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_tx_buffer_index;
static volatile uint8_t twi_tx_buffer_length;

static uint8_t twi_rx_buffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_rx_buffer_index;

static void (*twi_on_slave_transmit)(void);
static void (*twi_on_slave_receive)(uint8_t*, int);

void twi_init(void)
{
    twi_state = TWI_READY;
    
    cbi(TWSR, TWPS0);
    cbi(TWSR, TWPS1);
    
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

void twi_disable(void) { TWCR &= ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA)); }

void twi_set_address(uint8_t address) { TWAR = address << 1; }

void twi_attach_slave_rx_event(void (*function)(uint8_t*, int)) { twi_on_slave_receive = function; }

void twi_attach_slave_tx_event(void (*function)(void)) { twi_on_slave_transmit = function; }
 
uint8_t twi_read(uint8_t address, uint8_t* data, uint8_t length, uint8_t send_stop)
{
    uint8_t i;
    if (TWI_BUFFER_LENGTH < length) { return 0; }
    while (TWI_READY != twi_state) { continue; }
    
    twi_state = TWI_MRX;
    twi_send_stop = send_stop;
    twi_error = 0xFF;
    twi_master_buffer_index = 0;
    twi_master_buffer_length = length - 1;
    
    twi_slarw = TW_READ;
    twi_slarw |= address << 1;

    if (twi_in_rep_start == 1)
    {
        twi_in_rep_start = 0;
        do { TWDR = twi_slarw; } while(TWCR & _BV(TWWC));
        TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
    }
    else { TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); }

    while (TWI_MRX == twi_state) { continue; }

    if (twi_master_buffer_index < length) { length = twi_master_buffer_index; }

    for (i = 0; i < length; ++i) { data[i] = twi_master_buffer[i]; }
    
    return length;
}

uint8_t twi_write(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t send_stop)
{
    uint8_t i;
    
    if (TWI_BUFFER_LENGTH < length) { return 1; }
    while (TWI_READY != twi_state) { continue; }
    
    twi_state = TWI_MTX;
    twi_send_stop = send_stop;
    twi_error = 0xFF;

    twi_master_buffer_index = 0;
    twi_master_buffer_length = length;
    
    for (i = 0; i < length; ++i) { twi_master_buffer[i] = data[i]; }
    
    twi_slarw = TW_WRITE;
    twi_slarw |= address << 1;

    if (twi_in_rep_start)
    {
        twi_in_rep_start = 0;
        do { TWDR = twi_slarw; } while (TWCR & _BV(TWWC));
    }
    else { TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA); }
    
    while (wait && (TWI_MTX == twi_state)) { continue; }

    if (twi_error == 0xFF) { return 0; }
    else if (twi_error == TW_MT_SLA_NACK) { return 2; }
    else if (twi_error == TW_MT_DATA_NACK) { return 3; }
    else { return 4; }
}

uint8_t twi_transmit(const uint8_t* data, uint8_t length)
{
    uint8_t i;
    if (TWI_BUFFER_LENGTH < (twi_tx_buffer_length + length)) { return 1; }
    if (TWI_STX != twi_state) { return 2; }
    for (i = 0; i < length; ++i) { twi_tx_buffer[twi_tx_buffer_length+i] = data[i]; }
    twi_tx_buffer_length += length;
    return 0;
}
    
void twi_stop(void)
{
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
    while (TWCR & _BV(TWSTO)) { continue; }
    twi_state = TWI_READY;
}

void twi_reply(uint8_t ack)
{
    if (ack) { TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA); }
    else { TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT); }
}

void twi_release_bus(void)
{
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
    twi_state = TWI_READY;
}

ISR(TWI_vect)
{
    switch(TW_STATUS)
    {
        // MASTER
        case TW_START: // start condition transmitted
        case TW_REP_START: // repeated start condition transmitted
        {
            TWDR = twi_slarw;
            twi_reply(1);
        }
        break;
        
        // MASTER TRANSMITTER
        case TW_MT_SLA_ACK: // SLA+W transmitted, ACK received
        case TW_MT_DATA_ACK: // data transmitted, ACK received
        {
            if (twi_master_buffer_index < twi_master_buffer_length)
            {
                TWDR = twi_master_buffer[twi_master_buffer_index++];
                twi_reply(1);
            }
            else
            {
                if (twi_send_stop) { twi_stop(); }
                else
                {
                    twi_in_rep_start = 1;
                    TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
                    twi_state = TWI_READY;
                }
            }
        }
        break;

        case TW_MT_SLA_NACK:  // SLA+W transmitted, NACK received
        {
            twi_error = TW_MT_SLA_NACK;
            twi_stop();
        }
        break;
        case TW_MT_DATA_NACK: // data transmitted, NACK received
        {
            twi_error = TW_MT_DATA_NACK;
            twi_stop();
        }
        break;
        case TW_MT_ARB_LOST: // arbitration lost in SLA+W or data
        {
            twi_error = TW_MT_ARB_LOST;
            twi_release_bus();
        }
        break;
        
        // MASTER RECEIVER
        case TW_MR_DATA_ACK: // data received, ACK returned
        twi_master_buffer[twi_master_buffer_index++] = TWDR;
        /* fall through */
        case TW_MR_SLA_ACK:  // SLA+R transmitted, ACK received
        {
            if (twi_master_buffer_index < twi_master_buffer_length) { twi_reply(1); }
            else { twi_reply(0); }
        }
        break;
        case TW_MR_DATA_NACK: // data received, NACK returned
        {
            twi_master_buffer[twi_master_buffer_index++] = TWDR;
            if (twi_send_stop) { twi_stop(); }
            else {
                twi_in_rep_start = 1;
                TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
                twi_state = TWI_READY;
            }
        }
        break;
        case TW_MR_SLA_NACK: // SLA+R transmitted, NACK received
        {
            twi_stop();
        }
        break;
        
        // SLAVE RECEIVER
        case TW_SR_SLA_ACK: // SLA+W received, ACK returned
        case TW_SR_GCALL_ACK: // general call received, ACK returned
        case TW_SR_ARB_LOST_SLA_ACK:   // arbitration lost in SLA+RW, SLA+W received, ACK returned
        case TW_SR_ARB_LOST_GCALL_ACK: // arbitration lost in SLA+RW, general call received, ACK returned
        {
            twi_state = TWI_SRX;
            twi_rx_buffer_index = 0;
            twi_reply(1);
        }
        break;
        case TW_SR_DATA_ACK: // data received, ACK returned
        case TW_SR_GCALL_DATA_ACK: // general call data received, ACK returned
        {
            if (twi_rx_buffer_index < TWI_BUFFER_LENGTH) {
                
                twi_rx_buffer[twi_rx_buffer_index++] = TWDR;
                twi_reply(1);
            }
            else { twi_reply(0); }            
        }
        break;
        case TW_SR_STOP: // stop or repeated start condition received while selected
        {
            twi_release_bus();
            if(twi_rx_buffer_index < TWI_BUFFER_LENGTH) { twi_rx_buffer[twi_rx_buffer_index] = '\0'; }
            twi_on_slave_receive(twi_rx_buffer, twi_rx_buffer_index);
            twi_rx_buffer_index = 0;
        }
        break;

        case TW_SR_DATA_NACK:       // data received, NACK returned
        case TW_SR_GCALL_DATA_NACK: // general call data received, NACK returned
        {
            twi_reply(0);
        }
        break;
        
        // SLAVE TRANSMITTER
        case TW_ST_SLA_ACK: // SLA+R received, ACK returned
        case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost in SLA+RW, SLA+R received, ACK returned
        {            
            twi_state = TWI_STX;
            twi_tx_buffer_index = 0;
            twi_tx_buffer_length = 0;
            twi_on_slave_transmit();
            if (0 == twi_tx_buffer_length) {
                twi_tx_buffer_length = 1;
                twi_tx_buffer[0] = 0x00;
            }
        }
        /* fall through */
        case TW_ST_DATA_ACK: // data transmitted, ACK received
        {
            TWDR = twi_tx_buffer[twi_tx_buffer_index++];
            if (twi_tx_buffer_index < twi_tx_buffer_length) { twi_reply(1); }
            else { twi_reply(0); }
        }
        break;
        case TW_ST_DATA_NACK: // data transmitted, NACK received
        case TW_ST_LAST_DATA: // last data byte transmitted, ACK received
        {
            twi_reply(1);
            twi_state = TWI_READY;
        }
        break;
        
        // MISC
        case TW_NO_INFO:   // no state information available
        break;
        case TW_BUS_ERROR: // illegal start or stop condition
        {
            twi_error = TW_BUS_ERROR;
            twi_stop();
        }
        break;
    }
}