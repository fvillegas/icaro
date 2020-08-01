#ifndef __TWI_H__
#define __TWI_H__

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif

#define TWI_READY 0
#define TWI_MRX   1
#define TWI_MTX   2
#define TWI_SRX   3
#define TWI_STX   4

void twi_init(void);
void twi_disable(void);
void twi_set_address(uint8_t address);
void twi_attach_slave_rx_event(void (*function)(uint8_t*, int));
void twi_attach_slave_tx_event(void (*function)(void));
uint8_t twi_read(uint8_t address, uint8_t* data, uint8_t length, uint8_t send_stop);
uint8_t twi_write(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t send_stop);
uint8_t twi_transmit(const uint8_t* data, uint8_t length);

#endif

