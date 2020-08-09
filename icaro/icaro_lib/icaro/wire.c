#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "twi.h"
#include "wire.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static void (*user_on_request)(void);
static void (*user_on_receive)(int);

uint8_t rx_buffer[BUFFER_LENGTH];
uint8_t rx_buffer_index = 0;
uint8_t rx_buffer_length = 0;

uint8_t tx_address = 0;
uint8_t tx_buffer[BUFFER_LENGTH];
uint8_t tx_buffer_index = 0;
uint8_t tx_buffer_length = 0;

uint8_t transmitting = 0;

void wire_init()
{
    rx_buffer_index = 0;
    rx_buffer_length = 0;
    tx_buffer_index = 0;
    tx_buffer_length = 0;
    twi_init();
    twi_attach_slave_tx_event(wire_on_request_service);
    twi_attach_slave_rx_event(wire_on_receive_service);
}

void wire_set_address(uint8_t address) { twi_set_address(address); }

uint8_t wire_request_from(uint8_t address, uint8_t quantity, uint8_t send_stop)
{
    if (quantity > BUFFER_LENGTH) { quantity = BUFFER_LENGTH; }
    uint8_t read = twi_read(address, rx_buffer, quantity, send_stop);
    rx_buffer_index = 0;
    rx_buffer_length = read;
    return read;
}

void wire_begin_transmission(uint8_t address)
{
    transmitting = 1;
    tx_address = address;
    tx_buffer_index = 0;
    tx_buffer_length = 0;
}

uint8_t wire_end_transmission(uint8_t send_stop)
{
    uint8_t ret = twi_write(tx_address, tx_buffer, tx_buffer_length, 1, send_stop);
    tx_buffer_index = 0;
    tx_buffer_length = 0;
    transmitting = 0;
    return ret;
}

size_t wire_write(uint8_t data)
{
    if(transmitting)
    {
        if (tx_buffer_length >= BUFFER_LENGTH) { return 0; }
        tx_buffer[tx_buffer_index] = data;
        ++tx_buffer_index;
        tx_buffer_length = tx_buffer_index;
    }
    else { twi_transmit(&data, 1); }
    return 1;
}

int wire_available(void) { return rx_buffer_length - rx_buffer_index; }

int wire_read(void)
{
    int value = -1;
    if (rx_buffer_index < rx_buffer_length)
    {
        value = rx_buffer[rx_buffer_index];
        ++rx_buffer_index;
    }
    return value;
}

void wire_set_on_receive(void (*function)(int)) { user_on_receive = function; }

void wire_set_on_request(void (*function)(void)) { user_on_request = function; }

void wire_on_receive_service(uint8_t *in_bytes, int num_bytes)
{
    if (!user_on_receive) { return; }
    if (rx_buffer_index < rx_buffer_length) { return; }
    for(uint8_t i = 0; i < num_bytes; i++) { rx_buffer[i] = in_bytes[i]; }
    rx_buffer_index = 0;
    rx_buffer_length = num_bytes;
    user_on_receive(num_bytes);
}

void wire_on_request_service(void)
{
    if (!user_on_request) { return; }
    tx_buffer_index = 0;
    tx_buffer_length = 0;
    user_on_request();
}