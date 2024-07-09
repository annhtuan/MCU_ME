/*
 * Message.h
 *
 *  Created on: Jul 5, 2024
 *      Author: HUNG
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include "uart.h"
#include "stdbool.h"
#include<string.h>
#include <stdio.h>
#include <math.h>

#define Hex_Error		16
typedef struct {
    char type;
    uint16_t data;
    uint8_t checksum;
} Message;

uint8_t calculateChecksum( uint16_t data);
Message createMessage(char type, uint16_t data);
void messageToString(Message *msg, char *str);
void processMessage(Message *msg);
uint32_t StrtoHex(char* ptr, uint8_t len);


#endif /* MESSAGE_H_ */
