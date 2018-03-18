/*
 * uart_com.h
 *
 *  Created on: Dec 12, 2017
 *      Author: Nahuel
 */

#ifndef UART_COM_H_
#define UART_COM_H_

int32_t rx_used();
int32_t tx_used();
void send_bin(uint8_t * data, int8_t data_size);
void send_int(uint32_t * data);
void send_float(float * data);
void send_string(char * data, uint32_t data_length);

void get_bin(uint8_t * data, int8_t data_size);
void get_int(uint32_t * data);
void get_float(float * data);
void get_string(char * data, uint32_t data_length);

#endif /* UART_COM_H_ */
