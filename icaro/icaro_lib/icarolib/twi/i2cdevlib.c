#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "timer/timer.h"
#include "twi/twi.h"
#include "i2cdevlib.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

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

int8_t wire_get_status(void)
{
    return twi_get_state();
}
    
uint8_t wire_request_from(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
    if (isize > 0)
    {
        wire_begin_transmission(address);
        if (isize > 3) { isize = 3; }
        while (isize-- > 0) { wire_write((uint8_t)(iaddress >> (isize*8))); }
        wire_end_transmission(0);
    }
    if (quantity > BUFFER_LENGTH) { quantity = BUFFER_LENGTH; }
    uint8_t read = twi_read(address, rx_buffer, quantity, sendStop);
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

uint8_t wire_end_transmission(uint8_t sendStop)
{
    uint8_t ret = twi_write(tx_address, tx_buffer, tx_buffer_length, 1, sendStop);
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
    for(uint8_t i = 0; i < num_bytes; i++)
    {
        rx_buffer[i] = in_bytes[i];
    }
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


/** Read a single bit from an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param bit_num Bit position to read (0-7)
* @param data Container for single bit value
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (true = success)
*/
int8_t i2c_read_bit(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint8_t *data, uint16_t timeout)
{
    uint8_t b;
    uint8_t count = i2c_read_byte(dev_address, reg_address, &b, timeout);
    *data = b & (1 << bit_num);
    return count;
}

/** Read a single bit from a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param bit_num Bit position to read (0-15)
* @param data Container for single bit value
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (true = success)
*/
int8_t i2c_read_bit_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint16_t *data, uint16_t timeout)
{
    uint16_t b;
    uint8_t count = i2c_read_word(dev_address, reg_address, &b, timeout);
    *data = b & (1 << bit_num);
    return count;
}

/** Read multiple bits from an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param bit_start First bit position to read (0-7)
* @param length Number of bits to read (not more than 8)
* @param data Container for right-aligned value (i.e. '101' read from any bit_start position will equal 0x05)
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (true = success)
*/
int8_t i2c_read_bits(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint8_t *data, uint16_t timeout)
{
    uint8_t count, b;
    if ((count = i2c_read_byte(dev_address, reg_address, &b, timeout)) != 0)
    {
        uint8_t mask = ((1 << length) - 1) << (bit_start - length + 1);
        b &= mask;
        b >>= (bit_start - length + 1);
        *data = b;
    }
    return count;
}

/** Read multiple bits from a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param bit_start First bit position to read (0-15)
* @param length Number of bits to read (not more than 16)
* @param data Container for right-aligned value (i.e. '101' read from any bit_start position will equal 0x05)
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
*/
int8_t i2c_read_bits_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint16_t *data, uint16_t timeout)
{
    uint8_t count;
    uint16_t w;
    if ((count = i2c_read_word(dev_address, reg_address, &w, timeout)) != 0)
    {
        uint16_t mask = ((1 << length) - 1) << (bit_start - length + 1);
        w &= mask;
        w >>= (bit_start - length + 1);
        *data = w;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param data Container for byte value read from device
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (true = success)
*/
int8_t i2c_read_byte(uint8_t dev_address, uint8_t reg_address, uint8_t *data, uint16_t timeout)
{
    return i2c_read_bytes(dev_address, reg_address, 1, data, timeout);
}

/** Read single word from a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to read from
* @param data Container for word value read from device
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Status of read operation (true = success)
*/
int8_t i2c_read_word(uint8_t dev_address, uint8_t reg_address, uint16_t *data, uint16_t timeout)
{
    return i2c_read_words(dev_address, reg_address, 1, data, timeout);
}

/** Read multiple bytes from an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address First register reg_address to read from
* @param length Number of bytes to read
* @param data Buffer to store read data in
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Number of bytes read (-1 indicates failure)
*/
int8_t i2c_read_bytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t *data, uint16_t timeout)
{
    int8_t count = 0;
    uint32_t t1 = millis();
    for (uint8_t k = 0; k < length; k += min((int)length, BUFFER_LENGTH))
    {
        wire_begin_transmission(dev_address);
        wire_write(reg_address);
        wire_end_transmission(1);
        wire_begin_transmission(dev_address);
        wire_request_from(dev_address, (uint8_t)min(length - k, BUFFER_LENGTH), 0, 0, 1);
        for (; wire_available() && (timeout == 0 || millis() - t1 < timeout); count++) { data[count] = wire_read(); }
    }
    if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout
    return count;
}

/** Read multiple words from a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address First register reg_address to read from
* @param length Number of words to read
* @param data Buffer to store read data in
* @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
* @return Number of words read (-1 indicates failure)
*/
int8_t i2c_read_words(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint16_t *data, uint16_t timeout)
{
    int8_t count = 0;
    uint32_t t1 = millis();
    for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH))
    {
        wire_begin_transmission(dev_address);
        wire_write(reg_address);
        wire_end_transmission(1);
        wire_begin_transmission(dev_address);
        wire_request_from(dev_address, (uint8_t)(length * 2), 0, 0, 1);    
        uint8_t msb = 1;
        for (; wire_available() && count < length && (timeout == 0 || millis() - t1 < timeout);)
        {
            if (msb) { data[count] = wire_read() << 8; }
            else { data[count] |= wire_read(); count++; }
            msb = !msb;
        }
        wire_end_transmission(1);
    }
    if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1;
    return count;
}

/** write a single bit in an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to write to
* @param bit_num Bit position to write (0-7)
* @param value New bit value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_bit(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint8_t data)
{
    uint8_t b;
    i2c_read_byte(dev_address, reg_address, &b, I2CDEV_DEFAULT_READ_TIMEOUT);
    b = (data != 0) ? (b | (1 << bit_num)) : (b & ~(1 << bit_num));
    return i2c_write_byte(dev_address, reg_address, b);
}

/** write a single bit in a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to write to
* @param bit_num Bit position to write (0-15)
* @param value New bit value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_bit_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_num, uint16_t data)
{
    uint16_t w;
    i2c_read_word(dev_address, reg_address, &w, I2CDEV_DEFAULT_READ_TIMEOUT);
    w = (data != 0) ? (w | (1 << bit_num)) : (w & ~(1 << bit_num));
    return i2c_write_word(dev_address, reg_address, w);
}

/** Write multiple bits in an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to write to
* @param bit_start First bit position to write (0-7)
* @param length Number of bits to write (not more than 8)
* @param data Right-aligned value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_bits(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint8_t data)
{
    uint8_t b;
    if (i2c_read_byte(dev_address, reg_address, &b, I2CDEV_DEFAULT_READ_TIMEOUT) != 0)
    {
        uint8_t mask = ((1 << length) - 1) << (bit_start - length + 1);
        data <<= (bit_start - length + 1);
        data &= mask;
        b &= ~(mask);
        b |= data;
        return i2c_write_byte(dev_address, reg_address, b);
    }
    else { return 0; }
}

/** Write multiple bits in a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register reg_address to write to
* @param bit_start First bit position to write (0-15)
* @param length Number of bits to write (not more than 16)
* @param data Right-aligned value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_bits_word(uint8_t dev_address, uint8_t reg_address, uint8_t bit_start, uint8_t length, uint16_t data)
{
    uint16_t w;
    if (i2c_read_word(dev_address, reg_address, &w, I2CDEV_DEFAULT_READ_TIMEOUT) != 0)
    {
        uint16_t mask = ((1 << length) - 1) << (bit_start - length + 1);
        data <<= (bit_start - length + 1);
        data &= mask;
        w &= ~(mask);
        w |= data;
        return i2c_write_word(dev_address, reg_address, w);
    }
    else { return 0; }
}

/** Write single byte to an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register address to write to
* @param data New byte value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_byte(uint8_t dev_address, uint8_t reg_address, uint8_t data)
{
    return i2c_write_bytes(dev_address, reg_address, 1, &data);
}

/** Write single word to a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address Register address to write to
* @param data New word value to write
* @return Status of operation (true = success)
*/
uint8_t i2c_write_word(uint8_t dev_address, uint8_t reg_address, uint16_t data)
{
    return i2c_write_words(dev_address, reg_address, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
* @param dev_address I2C slave device address
* @param reg_address First register address to write to
* @param length Number of bytes to write
* @param data Buffer to copy new data from
* @return Status of operation (true = success)
*/
uint8_t i2c_write_bytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t* data)
{
    
    uint8_t status = 0;
    wire_begin_transmission(dev_address);
    wire_write((uint8_t) reg_address);
    for (uint8_t i = 0; i < length; i++)
    {
        wire_write((uint8_t) data[i]);
    }
    status = wire_end_transmission(1);
    return status == 0;
}

/** Write multiple words to a 16-bit device register.
* @param dev_address I2C slave device address
* @param reg_address First register address to write to
* @param length Number of words to write
* @param data Buffer to copy new data from
* @return Status of operation (true = success)
*/
uint8_t i2c_write_words(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint16_t* data)
{
    uint8_t status = 0;
    wire_begin_transmission(dev_address);
    wire_write(reg_address);
    for (uint8_t i = 0; i < length; i++)
    {
        wire_write((uint8_t)(data[i] >> 8));
        wire_write((uint8_t)data[i]);
    }
    status = wire_end_transmission(1);
    return status == 0;
}