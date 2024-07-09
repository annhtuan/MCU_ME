/*
 * Driver_ADC.c
 *
 *  Created on: Jun 26, 2024
 *      Author: HUNG
 */
#include "Driver_ADC.h"
#include "Core_NVIC.h"


void ADC_Init(ADC_Type*ADCn, Input_chanel_t channel)
{
	/*Config clock*/
	if(ADCn == ADC0)
	{
		/*Config and enable clock source to module ADC*/
		PCC->PCCn[PCC_ADC0_INDEX] &= ~PCC_PCCn_CGC_MASK;  	// Disable clock
		SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;    		// Disable Fast IRC
		SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);	 		// Divider = 1
		SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1);       		// Enable Fast IRC
		/*Select clock source and enable clock for module ADC*/
		PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(3);     	// Select clock source
		PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC(1);   	// Enable clock
	}else{
		/*Config and enable clock source to module ADC*/
		PCC->PCCn[PCC_ADC1_INDEX] &= ~PCC_PCCn_CGC_MASK;  	// Disable clock
		SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;    		// Disable Fast IRC
		SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);	 		// Divider = 1
		SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1);       		// Enable Fast IRC
		/*Select clock source and enable clock for module ADC*/
		PCC->PCCn[PCC_ADC1_INDEX] |= PCC_PCCn_PCS(3);    	// Select clock source
		PCC->PCCn[PCC_ADC1_INDEX] |= PCC_PCCn_CGC(1);   	// Enable clock
	}

	ADCn->SC1[0] &= ~ADC_SC1_ADCH_MASK; // Disable to config
	ADCn->CFG1 	 |= ADC_CFG1_ADICLK(0)|ADC_CFG1_MODE(1);
	ADCn->CFG2   |= ADC_CFG2_SMPLTS(12);
	ADCn->SC2    |= ADC_SC2_ADTRG(0);
	ADCn->SC3    |= (ADC_SC3_AVGE_MASK | ADC_SC3_CAL_MASK | ADC_SC3_ADCO_MASK | ADC_SC3_AVGS(0));

	/*Enable NVIC*/
	if(ADCn == ADC0)
	{
		NVIC_EnableIRQ(ADC0_IRQn);
	}else{
		NVIC_EnableIRQ(ADC1_IRQn);
	}

	ADCn->SC1[0] |= ADC_SC1_AIEN(1);
	ADCn->SC1[0] &= ~ADC_SC1_ADCH_MASK; // Disable to config
	ADCn->SC1[0] |= ADC_SC1_ADCH(channel);  // Start conversion on specified channel

}
uint16_t ADC_Read(void){
	return ADC0->R[0];
}

