/*
 * Core_NVIC.h
 *
 *  Created on: Jun 26, 2024
 *      Author: HUNG
 */

#ifndef CORE_NVIC_H_
#define CORE_NVIC_H_
#include "S32K144.h"
#include "stdint.h"
// Enable interrupt
void NVIC_EnableIRQ(IRQn_Type IRQn);

// Disable interrupt
void NVIC_DisableIRQ(IRQn_Type IRQn);

// Set Priority
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);


#endif /* CORE_NVIC_H_ */
