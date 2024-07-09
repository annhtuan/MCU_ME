/*
 * Core_NVIC.c
 *
 *  Created on: Jun 26, 2024
 *      Author: HUNG
 */
#include "Core_NVIC.h"

void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
    	S32_NVIC->ISER[IRQn >> 5U] = (1 << (IRQn & 0x1F));
    }
}

void NVIC_DisableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
    	S32_NVIC->ICER[IRQn >> 5U] = (1 << (IRQn & 0x1F));
    }
}

void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)(IRQn) >= 0)
    {
        S32_NVIC->IP[((uint32_t)IRQn)] = (uint32_t)((priority << 4U) & 0xFFUL);
    }
}
