/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "fsl_pit.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
//#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "fsl_ftm.h"

#include "uart_com.h"
/* TODO: insert other definitions and declarations here. */

void delay(void) {
    volatile uint32_t i = 0U;
    for (i = 0U; i < 80000U; ++i) {
        __asm("NOP"); /* delay */
}}

void short_delay(void) {
    volatile uint32_t i = 0U;
    for (i = 0U; i < 160U; ++i) {
        __asm("NOP"); /* delay */
}}

#define MOTOR true
#define BRAZO false

void update_duty_cycle(int32_t duty) {
//    PRINTF("%d\r\n", duty);
	// Disable channel output before updating the duty cycle
	FTM_UpdateChnlEdgeLevelSelect(MY_PWM_PERIPHERAL, BOARD_My_PWM_CHANNEL, 0U);
	// Update PWM duty cycle
	// Software trigger to update registers
	// Start channel output with updated duty cycle
	if(duty>0) {
		GPIO_ClearPinsOutput(BOARD_My_DIRECTION_GPIO, 1u << BOARD_My_DIRECTION_GPIO_PIN);
		FTM_UpdatePwmDutycycle(MY_PWM_PERIPHERAL, BOARD_My_PWM_CHANNEL, kFTM_EdgeAlignedPwm, duty);
		FTM_SetSoftwareTrigger(MY_PWM_PERIPHERAL, true);
		FTM_UpdateChnlEdgeLevelSelect(MY_PWM_PERIPHERAL, BOARD_My_PWM_CHANNEL, kFTM_HighTrue);
	} else {
		GPIO_SetPinsOutput(BOARD_My_DIRECTION_GPIO, 1u << BOARD_My_DIRECTION_GPIO_PIN);
		FTM_UpdatePwmDutycycle(MY_PWM_PERIPHERAL, BOARD_My_PWM_CHANNEL, kFTM_EdgeAlignedPwm, -duty);
		FTM_SetSoftwareTrigger(MY_PWM_PERIPHERAL, true);
		FTM_UpdateChnlEdgeLevelSelect(MY_PWM_PERIPHERAL, BOARD_My_PWM_CHANNEL, kFTM_LowTrue);
	}
	// TODO: Consider implementing a similar function with more quantization values.
}

int32_t ts;
#define SPI_TIMEOUT 25
#define REQUEST		0x10
#define ZEROING		0x70
#define POLLING		0x00

#define TS_20HZ 50000
#define K1_20HZ 0.65360
#define K2_20HZ 1.76169
#define K3_20HZ 0.1507

#define TS_100HZ 10000
#define K1_100HZ 0.84256
#define K2_100HZ 11.3952
#define K3_100HZ 0.95630

//Muy robusto! 0.5 de amplitud de oscilación en estado estacionario, y se banca altos golpes.
#define TS_AGR 10000
#define K1_AGR 1.6215
#define K2_AGR 12.8891
#define K3_AGR 0.88325

#define TS_AGR2 10000
#define K1_AGR2 0.7973
#define K2_AGR2 22.18026
#define K3_AGR2 0.61211


#define RIGHT_ZEROED_CORRECTION 534

int32_t zero_encoder(bool encoder){
	//Encoder must be powered off and on again for the new zero to be used.
    uint32_t read;
    uint32_t read2;
    uint32_t timeout_count = 0;

    dspi_master_config_t master;
    DSPI_MasterGetDefaultConfig(&master);
    dspi_command_data_config_t command;
    DSPI_GetDefaultDataCommandConfig(&command);
    if(encoder == BRAZO) {
    	command.whichPcs = kDSPI_Pcs1;
    	master.whichPcs = kDSPI_Pcs1;
    }
    DSPI_MasterInit(DSPI_1_PERIPHERAL, &master, CLOCK_GetFreq(DSPI_1_CLKSRC));
	DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, ZEROING);
	read = DSPI_ReadData(DSPI_1_PERIPHERAL);
	short_delay();

	while(read != 0x80 && timeout_count < SPI_TIMEOUT) {
		timeout_count++;
		DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, POLLING);
		read = DSPI_ReadData(DSPI_1_PERIPHERAL);
		short_delay();
	}
	if(timeout_count == SPI_TIMEOUT) {
		return false;
	}
	return true;
}

int32_t read_encoder(bool encoder){
    uint32_t read;
    uint32_t read2;
    uint32_t timeout_count = 0;

    dspi_master_config_t master;
    DSPI_MasterGetDefaultConfig(&master);
    dspi_command_data_config_t command;
    DSPI_GetDefaultDataCommandConfig(&command);
    if(encoder == BRAZO) {
    	command.whichPcs = kDSPI_Pcs1;
    	master.whichPcs = kDSPI_Pcs1;
    }
    DSPI_MasterInit(DSPI_1_PERIPHERAL, &master, CLOCK_GetFreq(DSPI_1_CLKSRC));
	DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, REQUEST);
	read = DSPI_ReadData(DSPI_1_PERIPHERAL);
//	PRINTF("sent: %x  recieved: %x\r\n", REQUEST, read);
	short_delay();

	while(read != 0x10 && timeout_count < SPI_TIMEOUT) {
		timeout_count++;
		DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, POLLING);
		read = DSPI_ReadData(DSPI_1_PERIPHERAL);
//		PRINTF("sent: %x  recieved: %x\r\n", POLLING, read);
		short_delay();
	}
	if(timeout_count == SPI_TIMEOUT) {
//		PRINTF("Encoder Timeout.\n\r");
		return 5000;
	}
	DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, POLLING);
	read = DSPI_ReadData(DSPI_1_PERIPHERAL);
//	PRINTF("sent: %x  recieved: %x\r\n", POLLING, read);
	short_delay();
	DSPI_MasterWriteDataBlocking(DSPI_1_PERIPHERAL, &command, POLLING);
	read2 = DSPI_ReadData(DSPI_1_PERIPHERAL);
//	PRINTF("sent: %x  recieved: %x\r\n", POLLING, read2);
	read <<= 8;
	read |= read2;

	return read;
}

volatile bool ts_flag = false;

void PIT0_IRQHandler(void) {
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	ts_flag = true;
}

void pit_init(void) {
    /* Structure of initialize PIT */
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);

    /* Init pit module */
    PIT_Init(PIT, &pitConfig);

    /* Set timer period for channel 0 */
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(ts, CLOCK_GetFreq(kCLOCK_BusClk)));

    /* Enable timer interrupts for channel 0 */
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(PIT0_IRQn);

    /* Start channel 0 */
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}


int16_t prev_speed_pos;
int16_t prev_tdot_pos;
#define SPEED_BUFFER_SIZE 1
int32_t speed_buffer[SPEED_BUFFER_SIZE];
uint8_t speed_head = 0;
#define TDOT_BUFFER_SIZE 1
int32_t tdot_buffer[TDOT_BUFFER_SIZE];
uint8_t tdot_head = 0;

float avg(int32_t *buffer, int32_t buffer_size) {
	float s = 0;
	uint8_t i = 0;

	for(i=0; i<buffer_size; i++) {
		s += buffer[i];
	}
	s /= buffer_size;
	return s;
}

void update_speed_buffer(int32_t new_pos) {
	int16_t speed = new_pos-prev_speed_pos;
	int8_t sign = speed>0 ? 1 : -1;
	if(abs(speed)>=2048) {
		speed = (4096-abs(speed))*(-sign);
	}
	speed_buffer[speed_head++] = speed;
	speed_head %= SPEED_BUFFER_SIZE;
	prev_speed_pos = new_pos;
}

void update_tdot_buffer(int32_t new_pos) {
	int16_t tdot = new_pos-prev_tdot_pos;
	int8_t sign = tdot>0 ? 1 : -1;
	if(abs(tdot)>=2048) {
		tdot = (4096-abs(tdot))*(-sign);
	}
	tdot_buffer[tdot_head++] = tdot;
	tdot_head %= TDOT_BUFFER_SIZE;
	prev_tdot_pos = new_pos;
}

int32_t ts = TS_20HZ;
float k1 = K1_20HZ;
float k2 = K2_20HZ;
float k3 = K3_20HZ;
int32_t zero_correction = RIGHT_ZEROED_CORRECTION;
int32_t pw = 20;

void nop(){}
void update_ts(){
	PIT_StopTimer(PIT, kPIT_Chnl_0);
	DisableIRQ(PIT0_IRQn);
    PIT_DisableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(ts, CLOCK_GetFreq(kCLOCK_BusClk)));
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT0_IRQn);
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}
void update_pw(){
	update_duty_cycle(pw);
}

uint8_t advanced_labels[][2]  = {"k1", "k2", "k3", "ts", "pw"};
uint8_t* advanced_pointers[]  = {&k1 , &k2 , &k3 , &ts , &pw };
void (*advanced_func_ptr[])() = {nop , nop , nop , update_ts, update_pw};


uint8_t labels[][2]  = {"k1", "k2", "k3", "ts", "pw"};
uint8_t* pointers[]  = {&k1 , &k2 , &k3 , &ts , &pw };
void (*func_ptr[])() = {nop , nop , nop , update_ts, update_pw};

void parse_instruction(void) {
    uint8_t cmd_capture[2];
    uint8_t i = 0;
    uint8_t a = sizeof(pointers);	// For debugging
	get_bin(&(cmd_capture[0]), 2);
	for(i=0; i<sizeof(pointers); i++){
		if(labels[i][0] == cmd_capture[0] && labels[i][1] == cmd_capture[1]) {
			get_bin(pointers[i],4);
			(*(func_ptr[i]))();
			break;
		}
	}
}

void duty_correction(int32_t *duty) {
	if(*duty < -100) {
		*duty = -100;
	}
	else if (*duty > 100) {
		*duty = 100;
	}
}

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
//    BOARD_InitDebugConsole();

    pit_init();
    DSPI_Enable(DSPI_1_PERIPHERAL, true);
    zero_encoder(BRAZO);	// ZERAR CONTRA LA DERECHA
    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0,};
	GPIO_PinInit(BOARD_My_DIRECTION_GPIO, BOARD_My_DIRECTION_GPIO_PIN, &led_config);

    int32_t encoder_pos, new_duty;
    uint8_t i = 0;
    float err=0;
    float acum_err=0;
    for(i=0; i<SPEED_BUFFER_SIZE; i++)
    	speed_buffer[i]=0;
    for(i=0; i<TDOT_BUFFER_SIZE; i++)
    	tdot_buffer[i]=0;

	update_duty_cycle(-20);
	int32_t pos_brazo;
	int32_t pos_motor;
	int32_t vel_brazo;
	int32_t vel_motor;
	int32_t state = 0;
	int32_t time_counter;
	pos_motor = (4096-read_encoder(MOTOR))%4096;
	update_speed_buffer(pos_motor);
	vel_motor = avg(speed_buffer, SPEED_BUFFER_SIZE);
	pos_motor = (4096-read_encoder(MOTOR))%4096;
	update_speed_buffer(pos_motor);
	vel_motor = avg(speed_buffer, SPEED_BUFFER_SIZE);


	while(1) {					//Rutina infinita de control
    	if(ts_flag) {			//Polling de flag de tiempo de muestreo
    		ts_flag = false;
			pos_brazo = read_encoder(BRAZO);	//Lectura del encoder del brazo
			if(pos_brazo>2048) {
				pos_brazo -= 4096;				//Corrección de overflow
			}
			pos_brazo -= zero_correction;		//Corrección de posición de cero.
    		pos_motor = (4096-read_encoder(MOTOR))%4096;	//Lectura del encoder del motor
			update_tdot_buffer(pos_brazo);		//Calculo de velocidades por diferencia
			update_speed_buffer(pos_motor);
			vel_brazo = avg(tdot_buffer, TDOT_BUFFER_SIZE);
			vel_motor = avg(speed_buffer, SPEED_BUFFER_SIZE);

			new_duty = (k1*pos_brazo + k2*vel_brazo + k3*vel_motor);	//Ecuación de control
			duty_correction(&new_duty);		//Corrección de saturación de duty cycle
			update_duty_cycle(new_duty);	//Nueva señal de control
/*
        	switch(state) {
        	case 0:
        		if(abs(pos_brazo) > 470) {
        			time_counter = 0;
        			state++;
        		} break;
        	case 1:
        		if(time_counter < 50) {
        			if(abs(vel_motor)<200) {
						update_duty_cycle(0);
        			} else {
						if(pos_brazo > 0) {
							update_duty_cycle(100-2*time_counter);
						} else {
							update_duty_cycle(-100+2*time_counter);
						}
        			}
        			time_counter++;
        		} else {
        			if(pos_brazo > 0) {
        				update_duty_cycle(-100);
        			} else {
        				update_duty_cycle(100);
        			}
        			time_counter = 0;
        			state++;
        		} break;
        	case 2:
        		if(time_counter < 20) {
        			time_counter++;
        		} else {
        			time_counter = 0;
        			state++;
        		} break;
        	case 3:
        		if(time_counter <= 4) {
        			time_counter++;
        		}
    			if(pos_brazo > 0) {
        			update_duty_cycle(-100+50*time_counter);
    			} else {
        			update_duty_cycle(100-50*time_counter);
    			}
    			if(abs(pos_brazo) < 300) {
            		time_counter = 0;
    				state = 0;
    			}
        	}

    		if(state == 0) {
    		}
*/
    		send_int(&pos_brazo);			//Envío de datos vía UART
			send_int(&vel_motor);
			send_int(&new_duty);
//			send_int(&ts);
//			send_int(&k1);
//			send_int(&k2);
    	}
//    	if(rx_used()>=6) {
//    		parse_instruction();			//Lectura de comandos UART
//        }
    }
    return 0;
}
