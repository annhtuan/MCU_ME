#include "systick.h"
#include "Core_NVIC.h"
uint32_t milliseconds=0;

// Handle SysTick interupt function
void SysTick_Handler(void) {
    milliseconds++;
}

//Init SysTick Timer
void SysTick_Init(void) {
    uint32_t ticks = SystemCoreClock / 1000; //  ticks per millisecond
    S32_SysTick->RVR = ticks - 1;         // Set reload register
	S32_SysTick->CVR  = 0;                 // Clear the current value register
	S32_SysTick->CSR |= S32_SysTick_CSR_ENABLE_MASK|S32_SysTick_CSR_CLKSOURCE_MASK|S32_SysTick_CSR_TICKINT_MASK; // Enable SysTick Timer and interupt
	NVIC_EnableIRQ(SysTick_IRQn);
}

//  millis() function return millisecond
unsigned long millis(void) {
    return milliseconds;
}

// delay millisecond
void delay_ms(uint32_t delay) {
    uint32_t start = millis();
    while (millis() - start < delay);
}

