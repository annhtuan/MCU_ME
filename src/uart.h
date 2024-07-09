/*
 * uart.h
 *
 *  Created on: Jun 25, 2024
 *      Author: HUNG
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#ifndef NULL
#define NULL ((void *)0)
#endif
typedef enum
{
	Disable			= 0U,
	SOSCDIV2_CLK 	= 1U, 
	SIRCDIV2_CLK 	= 2U,
	FIRCDIV2_CLK 	= 3U,
	SPLLDIV2_CLK 	= 4U
} MyUART1_SourceClock_t;

typedef enum
{
	None 	= 0u,
	Odd		= 1u,
	Even	= 2u,
} MyUART1_Parity_t;

typedef enum
{
	One 	= 1u,
	Two		= 2u,
} MyUART1_StopBit_t;

typedef enum
{
	LSB 	= 0u,
	MSB		= 1u,
} MyUART1_FrameDirection_t;

typedef void (*LPUART1_RxTx_IRQPtr)(void);

typedef struct
{
	uint32_t 					baudrate;
	uint8_t 					data_length;
	MyUART1_Parity_t 			parity;
	MyUART1_StopBit_t 			stopbit;
	MyUART1_FrameDirection_t 	direct;
	LPUART1_RxTx_IRQPtr 		ptr;
} MyUART1_Config_t;
void UART1_Enable(void);
void UART1_Init(MyUART1_Config_t* config);
void UART1_SendChar(uint8_t ch);
uint8_t UART1_ReceiveChar(void);
void UART1_SendMessage(uint8_t * pTxBuffer);
void UART1_SendString(uint8_t * pTxBuffer);
void UART1_ReceiveBlocking(uint8_t *pRxBuffer, uint8_t len);
void UART1_ReceiveNonBlocking(void);



#endif /* UART_H_ */
