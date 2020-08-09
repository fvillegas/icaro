#ifndef __WIRE_H_
#define __WIRE_H_

#define BUFFER_LENGTH 32

void wire_init(void);
void wire_set_address(uint8_t address);
uint8_t wire_request_from(uint8_t address, uint8_t quantity, uint8_t send_stop);
void wire_begin_transmission(uint8_t address);
uint8_t wire_end_transmission(uint8_t send_stop);
size_t wire_write(uint8_t data);
int wire_available(void);
int wire_read(void);

void wire_set_on_receive(void (*)(int));
void wire_set_on_request(void (*)(void));

void wire_on_receive_service(uint8_t *in_bytes, int num_bytes);
void wire_on_request_service(void);


#endif /* WIRE_H_ */