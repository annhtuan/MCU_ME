/*
 * Driver_ADC.h
 *
 *  Created on: Jun 26, 2024
 *      Author: HUNG
 */

#ifndef DRIVER_ADC_H_
#define DRIVER_ADC_H_

#include "S32K144.h"

typedef enum{
	External_Chanel0  =0,
	External_Chanel1  =1,
	External_Chanel2  =2,
	External_Chanel3  =3,
	External_Chanel4  =4,
	External_Chanel5  =5,
	External_Chanel6  =6,
	External_Chanel7  =7,
	External_Chanel8  =8,
	External_Chanel9  =9,
	External_Chanel10 =10,
	External_Chanel11 =11,
	External_Chanel12 =12,
	External_Chanel13 =13,
	External_Chanel14 =14,
	External_Chanel15 =15,
	Reserved_0 		  =-1,
	Reserved_1 		  =-1,
	Reserved_2 		  =-1,
	Reserved_3 		  =-1,
	Internal_Chanel0  =20,
	Internal_Chanel1  =21,
	Internal_Chanel2  =22,
}Input_chanel_t;

/*
 * ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC. Refer to
REFSEL description in ADC chapter for more details.
 * */
void ADC_Init(ADC_Type*ADCn,Input_chanel_t channel);
uint16_t ADC_Read(void);


#endif /* DRIVER_ADC_H_ */
