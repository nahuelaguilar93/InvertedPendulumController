/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_ftm.h"
#include "fsl_dspi.h"
#include "fsl_uart.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definition of peripheral ID */
#define MY_PWM_PERIPHERAL FTM0
/* Definition of the clock source frequency */
#define MY_PWM_CLOCK_SOURCE CLOCK_GetFreq(kCLOCK_BusClk)
/* MY_PWM interrupt vector ID (number). */
#define MY_PWM_IRQN FTM0_IRQn
/* MY_PWM interrupt handler identifier. */
#define MY_PWM_IRQHANDLER FTM0_IRQHandler
/* BOARD_InitPeripherals defines for SPI0 */
/* Definition of peripheral ID */
#define DSPI_1_PERIPHERAL SPI0
/* Definition of the clock source */
#define DSPI_1_CLKSRC DSPI0_CLK_SRC
/* Definition of peripheral ID */
#define UART_1_PERIPHERAL UART0
/* Definition of the clock source frequency */
#define UART_1_CLOCK_SOURCE CLOCK_GetFreq(UART0_CLK_SRC)
/* UART_1 interrupt vector ID (number). */
#define UART_1_SERIAL_RX_TX_IRQN UART0_RX_TX_IRQn
/* UART_1 interrupt handler identifier. */
#define UART_1_SERIAL_RX_TX_IRQHANDLER UART0_RX_TX_IRQHandler
/* UART_1 interrupt vector ID (number). */
#define UART_1_SERIAL_ERROR_IRQN UART0_ERR_IRQn
/* UART_1 interrupt handler identifier. */
#define UART_1_SERIAL_ERROR_IRQHANDLER UART0_ERR_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const ftm_config_t MY_PWM_config;
extern const dspi_master_config_t DSPI_1_config;
extern const uart_config_t UART_1_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#endif /* _PERIPHERALS_H_ */
