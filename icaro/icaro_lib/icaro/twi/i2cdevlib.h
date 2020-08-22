#ifndef I2CDEVLIB_H_
#define I2CDEVLIB_H_

#include <stdlib.h>

#define BUFFER_LENGTH 32
#define I2CDEV_DEFAULT_READ_TIMEOUT 1000

void wire_init();
void wire_set_address(uint8_t address);
int8_t wire_get_status(void);
uint8_t wire_request_from(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop);
void wire_begin_transmission(uint8_t address);
uint8_t wire_end_transmission(uint8_t sendStop);
size_t wire_write(uint8_t data);
int wire_available(void);
int wire_read(void);

void wire_set_on_receive(void (*)(int));
void wire_set_on_request(void (*)(void));

void wire_on_receive_service(uint8_t *in_bytes, int num_bytes);
void wire_on_request_service(void);

int8_t i2c_read_bit(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint8_t *data, uint16_t timeout);
int8_t i2c_read_bit_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint16_t *data, uint16_t timeout);
int8_t i2c_read_bits(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint8_t *data, uint16_t timeout);
int8_t i2c_read_bits_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint16_t *data, uint16_t timeout);
int8_t i2c_read_byte(uint8_t dev_address, uint8_t reg_address, uint8_t *data, uint16_t timeout);
int8_t i2c_read_word(uint8_t dev_address, uint8_t reg_address, uint16_t *data, uint16_t timeout);
int8_t i2c_read_bytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t *data, uint16_t timeout);
int8_t i2c_read_words(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint16_t *data, uint16_t timeout);
uint8_t i2c_write_bit(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint8_t data);
uint8_t i2c_write_bit_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint16_t data);
uint8_t i2c_write_bits(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint8_t data);
uint8_t i2c_write_bits_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint16_t data);
uint8_t i2c_write_byte(uint8_t dev_address, uint8_t reg_address, uint8_t data);
uint8_t i2c_write_word(uint8_t dev_address, uint8_t reg_address, uint16_t data);
uint8_t i2c_write_bytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t* data);
uint8_t i2c_write_words(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint16_t* data);

#endif