#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#define SystemCoreClock 48000000

//khai bao systick
//truyen vao so milis
//void SysTick_Init(uint32_t time);

// Handle SysTick interupt function


//Init SysTick Timer
void SysTick_Init(void) ;

//  millis() function return millisecond
unsigned long millis(void);

// delay millisecond
void delay_ms(uint32_t delay) ;

#endif // SYSTICK_H
