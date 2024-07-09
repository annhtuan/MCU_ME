#include "uart.h"
#include "Driver_GPIO.h"
#include "Core_NVIC.h"
#include "Driver_ADC.h"
#include "Message.h"
#include "systick.h"
#include <string.h>

extern ARM_DRIVER_GPIO Driver_GPIO;
static ARM_DRIVER_GPIO * GPIO_drv = &Driver_GPIO;
#define RED_LED					111
#define GREEN_LED               112
#define SW2  					76
#define SW3 					77
#define PERCENT_THRESHOLD      9

// Global variables
volatile uint8_t buttonPressed = 0;
volatile uint32_t timerCounter = 0;
volatile uint16_t num = 0;
#define ADC_MAX_VALUE 4096


//variable store current data of adc,percentage of volume
volatile uint16_t ADC_Value;
uint16_t previous_volume_value = 0;

//store string
char p[10];
//store the last send string when "WRONG"
char temp_str[10];

//index for p[10]
uint8_t i = 0;

//flag check if message is receive a full a message ( 8byte string)
uint8_t messageReceived = 0;

//status of led
//on led = true =>>>green
//on led = false =>>>red
uint8_t on_led = 0;



//strsub in C
void substring(const char* source, char* dest, int start, int length) {
	strncpy(dest, source + start, length);
    dest[length] = '\0';
}

//Init green ,red led
//middleware function
void GPIO_LEDInit(void)
{
    /* Initialize pins */
	//Red led
	//driver function
	GPIO_drv->Setup(RED_LED,NULL);
	GPIO_drv->SetDirection(RED_LED,ARM_GPIO_OUTPUT);
	GPIO_drv->SetOutput(RED_LED,1);
	GPIO_drv->Setup(GREEN_LED,NULL);
	GPIO_drv->SetDirection(GREEN_LED,ARM_GPIO_OUTPUT);
    GPIO_drv->SetOutput(GREEN_LED,1);
}

//function to toggle red ( not use)
void GPIO_Toggle(void){
	GPIO_drv->Toggle(RED_LED);
}

//when PC send message to MCU
//check that message
//with 3 case "W", "R", "O"
uint8_t ParseMessage(Message *msg) {
    if (messageReceived) {
		//create type field to store 2 first character
    	char first[3];

    	//create data field to store 4 next character
    	char second[5];

    	//function to store character to first and second
    	substring(p, first, 0, 2);
    	substring(p, second, 2, 4);

    	//function to convert string to hex then typecast to char
		char type  =  (char)StrtoHex(first,2);

		//function to convert string to hex then typecast to uint16_t
		uint16_t data = (uint16_t)StrtoHex(second,4);

		//pass type,data to msg to create msg
		*msg = createMessage(type,data);

		//local var , don't care
		uint8_t test = VerifyMessage(msg);

		//check if checksum is right or not
		if(VerifyMessage(msg)){
			//reset num
			if(msg->type == 'R'){
              num =0;
			}
			//wrong, need to resent form mcu to pc
			else if(msg->type ==  'W'){
				//resent the last string send
				UART1_SendString(temp_str);
			}
			//on off led , odd : off , even :on
			else if(msg->type == 'O'){
				on_led = ~on_led;
			}
			return 1;
		}
		else
		{
			//if wrong check sum return 0
			return 0;
		}
    } else {
    	//if not receive enough data return 0
        return 0;
    }
}
void Getdata(void){
	//pc to mcu , enable interrput
	//store that string to p[]
	//store full ->toggle
	//receive data then push into p
	p[i] = LPUART1->DATA;
	if(i >=7){
		i = 0;
		messageReceived = true;
	}
	else{
		messageReceived = false;
		i++;
	}
	num = 0;
}
//
//void sendMessage(Message msg) {
//	char messageString[4];
//	messageToString(&msg, messageString);
//    UART1_SendString(messageString);
//}
//void requestResend() {
//    Message msg = createMessage('R', 0);
//    sendMessage(msg);
//}
//

void receiveAndProcessMessage() {
    Message msg;
    if (ParseMessage(&msg)) {
        //UART1_SendString("SUCCESS");
    } else {
        //requestResend();
    	//UART1_SendString("ERROR");
    }
}


void ADC0_IRQHandler(void)
{	//message : type : V ->>>0x56
	//			data : 1 ->>>0x0001 (+)
	//			checksum : have

		if((ADC0->SC1[0] & ADC_SC1_COCO_MASK) != 0u){
			    ADC_Value = ADC_Read();
			    uint16_t volume_value = (ADC_Read() * 100) / ADC_MAX_VALUE;
				if (volume_value >= (previous_volume_value + PERCENT_THRESHOLD)) {
					//volumeup send
					char buffer[20];
					Message a = createMessage('V',volume_value);
					//save a to buffer then transform a to hexstring : 560001___
					messageToString(&a,buffer);
					UART1_SendString(buffer);
					previous_volume_value = volume_value;
									}
				else if (volume_value <= (previous_volume_value - PERCENT_THRESHOLD)) {
					//volume down send
					char buffer[20];
					Message a = createMessage('V',volume_value);
					//save a to buffer then transform a to hexstring : 560000___
					messageToString(&a,buffer);
					UART1_SendString(buffer);
					previous_volume_value = volume_value;
					}
		}
			    //previous_volume_value = volume_value;
			    //ADC0->SC1[0] |= ADC_SC1_ADCH(External_Chanel12);
}

void Button_Init(void) {
//	GPIO_drv->GPIO_Setup(SW2,NULL);
//	GPIO_drv->GPIO_Setup(SW3,NULL);
//	GPIO_drv->GPIO_SetPullResistor (SW2,ARM_GPIO_PULL_UP);
//	GPIO_drv->GPIO_SetPullResistor (SW3,ARM_GPIO_PULL_UP);
//
//	GPIO_drv->SetEventTrigger(SW2,ARM_GPIO_TRIGGER_EITHER_EDGE);
//	GPIO_drv->SetEventTrigger(SW3,ARM_GPIO_TRIGGER_EITHER_EDGE);
//	GPIO_drv->SetDirection(SW2,ARM_GPIO_INPUT);
//	GPIO_drv->SetDirection(SW3,ARM_GPIO_INPUT);
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xB);
    PORTC->PCR[13] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xB);
    PTC->PDDR &= ~(1 << 12);
    PTC->PDDR &= ~(1 << 13);
    // Enable Interrupts for PORTC
    NVIC_EnableIRQ(PORTC_IRQn);
}


void Timer_Init(void) {
    //
 	PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC(1u);
	SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(1);
	SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(1);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(0b011);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC(1u);
	LPIT0->MCR |= (1<<LPIT_MCR_M_CEN_SHIFT) | (1<<LPIT_MCR_DBG_EN_SHIFT);
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_MODE(0u);
    LPIT0->TMR[0].TVAL = 48000000; // Set timer value for 2 seconds (assuming 2MHz clock)
   // LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_TIE_MASK; // Enable interrupt
    LPIT0->MIER |= (1 << LPIT_MIER_TIE0_SHIFT);
    // Enable Interrupts for LPIT0s
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
}
void PORTC_IRQHandler(void)
{
	//SW2
    if (PORTC->ISFR & (1 << 12))
    {
        if (buttonPressed == 0)
        {
            buttonPressed = 1;
            timerCounter = 0;
            LPIT0->TMR[0].TVAL = 24000000; // Reset timer value
            LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK; // Enable timer
        } else
        {
            buttonPressed = 0; 
            if (timerCounter < 2)
            {
                num++;
            	if(num > 16) num = 1;
                char buffer[20];
                //sprintf(buffer, "%d", num);
			   //message : 41|0001|FF
			   Message a = createMessage('A',num);
			   messageToString(&a,buffer);
			   UART1_SendString(buffer);
			   //temp string
				for(int id =0;id<8;id++){
					temp_str[id] = buffer[id];
				}
            }
			LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK; // Disable timer
        }
        PORTC->ISFR |= (1 << 12); // Clear interrupt flag
    }
    //SW3
    if (PORTC->ISFR & (1 << 13))
    {
        if (buttonPressed == 0)
        {
            buttonPressed = 1;
            timerCounter = 0;
            LPIT0->TMR[0].TVAL = 24000000; // Reset timer value
            LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK; // Enable timer
        } else
        {
            buttonPressed = 0;
            if (timerCounter < 2)
            {

                num--;
                if(num < 1) num = 16;
                char buffer[20];
                Message a = createMessage('A',num);
                messageToString(&a,buffer);
                UART1_SendString(buffer);
                //temp string
                for(int id =0;id<8;id++){
					temp_str[id] = buffer[id];
				}
            }
			LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK; // Disable timer
        }
        PORTC->ISFR |= (1 << 13); // Clear interrupt flag
    }
}
//DONE TEMP STR
void LPIT0_Ch0_IRQHandler(void) {
    timerCounter++;
    if (timerCounter >= 2)
    {
        if (PTC->PDIR & (1 << 12))
        {
        	char buffer[20];

			//message : 41|0001|FF
			Message a = createMessage('S',0);
			messageToString(&a,buffer);
			UART1_SendString(buffer);
			//temp string
			for(int id =0;id<8;id++){
				temp_str[id] = buffer[id];
			}
        }
    	PORTC->ISFR |= (1 << 12);
        if (PTC->PDIR & (1 << 13))
        {	char buffer[20];
        	//UART1_SendChar('b');
			//message : 41|0001|FF
			Message a = createMessage('B',0);
			messageToString(&a,buffer);
			UART1_SendString(buffer);
			//temp string
			for(int id =0;id<8;id++){
				temp_str[id] = buffer[id];
			}
        }
    	PORTC->ISFR |= (1 << 13);
        buttonPressed = 1;
        LPIT0->TMR[0].TCTRL &= ~LPIT_TMR_TCTRL_T_EN_MASK; // Disable timer
    }
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; // Clear interrupt flag
}
int main(void) {
	MyUART1_Config_t cf;
	cf.baudrate = 9600;
	cf.data_length = 8;
	cf.stopbit = 1;
	cf.parity = None;
	cf.direct = LSB;
	cf.ptr = &Getdata;

	UART1_Init(&cf);
	UART1_ReceiveNonBlocking();
	GPIO_LEDInit();
	SysTick_Init();
	Button_Init();
	Timer_Init();
	ADC_Init(ADC0,External_Chanel12);



	while(1){
		if(messageReceived){
			receiveAndProcessMessage();
			messageReceived=0;
		}

		if(on_led){
			GPIO_drv->SetOutput(RED_LED,1);
			delay_ms(500);
			GPIO_drv->Toggle(GREEN_LED);

			//GPIO_drv->SetOutput(GREEN_LED,0);
		}
		else{
			GPIO_drv->SetOutput(GREEN_LED,1);
			delay_ms(500);
			GPIO_drv->Toggle(RED_LED);
			//GPIO_drv->SetOutput(RED_LED,0);

		}
	}
    return 0;
}
