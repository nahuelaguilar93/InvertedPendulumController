/*
 * uart_com.c
 *
 *  Created on: Dec 12, 2017
 *      Author: Nahuel
 */

#include "peripherals.h"
#include "fsl_uart.h"
#include "uart_com.h"

#define TX_BUFFER_SIZE 512
#define RX_BUFFER_SIZE 512

uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t rx_buffer[RX_BUFFER_SIZE];
int16_t tx_base = 0;
int16_t rx_base = 0;
int16_t tx_head = 0;
int16_t rx_head = 0;

int32_t tx_used(){
	int32_t a = tx_head-tx_base;
	if(a<0)
		a += TX_BUFFER_SIZE;
	return a;
}

int32_t rx_used(){
	int32_t a = rx_head-rx_base;
	if(a<0)
		a += RX_BUFFER_SIZE;
	return a;
}

void UART_1_SERIAL_RX_TX_IRQHANDLER(void) {
    uint8_t data;

	// If new data arrived.
    uint32_t flags = UART_GetStatusFlags(UART_1_PERIPHERAL);
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & flags) {
        data = UART_ReadByte(UART_1_PERIPHERAL);

		// If ring buffer is not full, add data to ring buffer.
        if (((rx_head + 1) % RX_BUFFER_SIZE) != rx_base) {
            rx_buffer[rx_head++] = data;
            rx_head %= RX_BUFFER_SIZE;
        }
    }
    else if ((kUART_TxDataRegEmptyFlag | kUART_TransmissionCompleteFlag) & flags) {
    	// If there is data to send, do it.
    	if (tx_base != tx_head) {
    		UART_WriteByte(UART_1_PERIPHERAL, tx_buffer[tx_base++]);
    		tx_base %= TX_BUFFER_SIZE;
    	}
    	// Else disable the interruptions.
    	else {
    		UART_DisableInterrupts(UART_1_PERIPHERAL, kUART_TxDataRegEmptyInterruptEnable | kUART_TransmissionCompleteInterruptEnable);
    	}
    }
}

void send_bin(uint8_t * data, int8_t data_size) {
	uint8_t i;
	if(TX_BUFFER_SIZE-1 - tx_used() >= data_size) {
		for(i=0; i<data_size; i++) {
			tx_buffer[tx_head] = data[i];
			tx_head++;
			tx_head %= TX_BUFFER_SIZE;
		}
		UART_EnableInterrupts(UART_1_PERIPHERAL, kUART_TxDataRegEmptyInterruptEnable | kUART_TransmissionCompleteInterruptEnable);
	}
}

void send_int(uint32_t * data) {
	send_bin(data, sizeof(int32_t));
}

void send_float(float * data) {
	send_bin(data, sizeof(float));
}

void send_string(char * data, uint32_t data_length) {
	send_bin(data, data_length);
}

void get_bin(uint8_t * data, int8_t data_size) {
	uint8_t i;
	if(rx_used() >= data_size) {
		for(i=0; i<data_size; i++) {
			data[i] = rx_buffer[rx_base++];
			rx_base %= RX_BUFFER_SIZE;
		}
	}
}

void get_int(uint32_t * data) {
	get_bin(data, sizeof(int32_t));
}

void get_float(float * data) {
	get_bin(data, sizeof(float));
}

void get_string(char * data, uint32_t data_length) {
	get_bin(data, data_length);
}

