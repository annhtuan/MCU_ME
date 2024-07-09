/*
 * uart.c
 *
 *  Created on: Jun 25, 2024
 *      Author: HUNG
 */

#include "UART.h"
#include "S32K144.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

static LPUART1_RxTx_IRQPtr FunctionCall = NULL;

void UART1_Enable(void) 
{
	//Enable clock for Port C
	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
	//Pin 6 RX
	PORTC->PCR[6] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[6] |= 	PORT_PCR_MUX(2);			//Mux uart
	//Pin 7 TX
	PORTC->PCR[7] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[7] |= 	PORT_PCR_MUX(2);			//Mux uart
	/*Clock for UART module*/
	/*Clock source*/
	SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;    /* Disable Fast IRC */
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1u);	 /* Divider = 1 */
	SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1u);       /* Enable Fast IRC */
	/*Select clock source and enable clock for LPUART1 module*/
	PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(3u); //Select FIRCDIV2_CLK
	PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_CGC_MASK;//Enable clock for LPUART1 module
	/*clock = 48MHz*/
}

void UART1_DefaultBaudrate(uint32_t baudrate)
{
	LPUART1->CTRL &= ~(LPUART_CTRL_TE_MASK|LPUART_CTRL_RE_MASK);
	switch (baudrate){
	case 9600:
		//OSR = 15 -> SBR = 312 (9600)
		LPUART1->BAUD &= ~LPUART_BAUD_SBR_MASK;
		LPUART1->BAUD |= LPUART_BAUD_SBR(312u);
		break;
	}
}
void UART1_SelectSourceClock(MyUART1_SourceClock_t source)
{
	// Clear PCS field ~ Disable
	PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_PCS_MASK;
	if (source != Disable)
	{
		//Set PCS field
		switch ((uint8_t)source)
		{
			case SOSCDIV2_CLK:
				PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(1u);
				break;
			case SIRCDIV2_CLK:
				PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(2u);
				break;
			case FIRCDIV2_CLK :
				PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(3u);
				break;
			case SPLLDIV2_CLK:
				PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(6u);
				break;
		}
	}
}

void UART1_ConfigFrame(MyUART1_Config_t* config)
{
	LPUART1->CTRL &= ~(LPUART_CTRL_TE_MASK|LPUART_CTRL_RE_MASK);
	/* Configure data */
	switch (config->data_length)
	{
	case 7:
		//Set bit M10 to 0 to choose 7 to 9 bit data
		LPUART1->BAUD &= ~LPUART_BAUD_M10_MASK;
		//Select 7-bit data characters
		LPUART1->CTRL |= LPUART_CTRL_M7(1u);
	case 8:
		//Set bit M10 to 0 to choose 7 to 9 bit data
		LPUART1->BAUD &= ~LPUART_BAUD_M10_MASK;
		//Clear bit M7 to 0 to select 8to 10-bit data
		LPUART1->CTRL &= ~LPUART_CTRL_M7_MASK;
		//Select 8-bit data characters
		LPUART1->CTRL &= ~LPUART_CTRL_M_MASK;
		break;
	case 9:
		//Set bit M10 to 0 to choose 7 to 9 bit data
		LPUART1->BAUD &= ~LPUART_BAUD_M10_MASK;
		//Clear bit M7 to 0 to select 8to 10-bit data
		LPUART1->CTRL &= ~LPUART_CTRL_M7_MASK;
		//Select 9-bit data characters
		LPUART1->CTRL |= LPUART_CTRL_M(1u);
		break;
	case 10:
		//Clear bit M7 to 0 to select 8to 10-bit data
		LPUART1->CTRL &= ~LPUART_CTRL_M7_MASK;
		//Select 10-bit data characters
		LPUART1->BAUD |= LPUART_BAUD_M10(1u);
		break;

	}
	/* Configure parity bit. Default parity none*/
	//Disable parity bit
	LPUART1->CTRL &= ~LPUART_CTRL_PE_MASK;
	if (config->parity != None)
	{
		//Enable parity bit
		LPUART1->CTRL |= LPUART_CTRL_PE(1u);
		//Even mode
		LPUART1->CTRL &= ~LPUART_CTRL_PT_MASK;
		if (config->parity == Odd)
		{
			LPUART1->CTRL |= LPUART_CTRL_PT(1u);
		}
	}
	/* Configure direction frame. Default LSB */
	LPUART1->STAT &= ~ LPUART_STAT_MSBF_MASK;
	if (config->direct == MSB)
	{
		LPUART1->STAT |= LPUART_STAT_MSBF(1u);
	}
	/*Data polarity*/
	//LPUART1->CTRL |= LPUART_CTRL_TXINV(1u);
	LPUART1->CTRL &= ~LPUART_CTRL_TXINV_MASK;
	//LPUART1->STAT |= LPUART_STAT_RXINV(1u);
	LPUART1->STAT &= ~LPUART_STAT_RXINV_MASK;
	/* Configure stop bit*/
	LPUART1->BAUD &= ~LPUART_BAUD_SBNS_MASK;
	if (config->stopbit == Two)
	{
		LPUART1->BAUD |= LPUART_BAUD_SBNS_MASK;
	}
}

void UART1_SetHandler(LPUART1_RxTx_IRQPtr address)
{
	FunctionCall = address;
}
void UART1_Init(MyUART1_Config_t* config)
{
	UART1_Enable();
	UART1_DefaultBaudrate(config->baudrate);
	UART1_ConfigFrame(config);
	UART1_SetHandler(config->ptr);
}

void UART1_SendChar(uint8_t ch)
{
	//Enable Transmitor
	LPUART1->CTRL |= LPUART_CTRL_TE(1u);
	//wait TDRE = 1
	while(!(LPUART1->STAT & LPUART_STAT_TDRE_MASK));
	//Write Data to Tx - Buffer
	LPUART1->DATA = ch;
	//wait TC = 1
	while(!(LPUART1->STAT & LPUART_STAT_TC_MASK));
	//Disable Transmitor
	LPUART1->CTRL &= ~LPUART_CTRL_TE_MASK;
}
uint8_t UART1_ReceiveChar(void){
	uint8_t Data;
	//Enable RX
	LPUART1->CTRL |= LPUART_CTRL_RE(1u);
	//Wait RDRF
	while((LPUART1->STAT & LPUART_STAT_RDRF_MASK) == 0U);
	//Read Data Buffer
	Data = LPUART1->DATA;
	//Disable RX
	LPUART1->CTRL &= ~LPUART_CTRL_RE_MASK;
	return Data;
}

void UART1_SendString(uint8_t * pTxBuffer)
{
	/* Enable Transmit */
	LPUART1->CTRL |= LPUART_CTRL_TE(1u);
	while (*pTxBuffer != '\0')
	{
		while(!(LPUART1->STAT & LPUART_STAT_TDRE_MASK));
		LPUART1->DATA = *(pTxBuffer++);
	}
	/* Disable Transmit */
	while(!(LPUART1->STAT & LPUART_STAT_TC_MASK));
	LPUART1->CTRL &= ~LPUART_CTRL_TE_MASK;
}

void UART1_ReceiveBlocking(uint8_t *pRxBuffer, uint8_t len)
{
	uint8_t data_extra;
	//Enable Receive
	LPUART1->CTRL |= LPUART_CTRL_RE(1u);
	//wait RDRF = 1
	while (len--)
	{
		while(!(LPUART1->STAT & LPUART_STAT_RDRF_MASK));
		//Read Rx Data Buffer
		*pRxBuffer = LPUART1->DATA;
		pRxBuffer++;
	}
	*pRxBuffer = '\0';
	data_extra = LPUART1->DATA;
	//Disable Receive
	LPUART1->CTRL &= ~LPUART_CTRL_RE_MASK;
}

void UART1_ReceiveNonBlocking(void)
{
	//Enable Interrupt
	S32_NVIC->ISER[LPUART1_RxTx_IRQn  >> 5] = (1 << (LPUART1_RxTx_IRQn  & 0x1F));
	LPUART1->CTRL |= LPUART_CTRL_RIE_MASK;
	LPUART1->CTRL |= LPUART_CTRL_ORIE_MASK;
	//Enable Receive
	LPUART1->CTRL |= LPUART_CTRL_RE_MASK;
}


void LPUART1_RxTx_IRQHandler(void)
{
	if (LPUART1->STAT & LPUART_STAT_RDRF_MASK)
	{
		FunctionCall();
	}
	else if (LPUART1->STAT & LPUART_STAT_OR_MASK)
	{
		/* Clear Flag */
		LPUART1->STAT |= LPUART_STAT_OR(1u);
	}
}
void GetMessage(){

}
