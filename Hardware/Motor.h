#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"        

void Motor_Init(void);
void Encoder_Init(void);
void Motor1_SetDirct(uint16_t A1, uint16_t B1);
void Motor2_SetDirct(uint16_t A2, uint16_t B2);
uint16_t Motor_Security(float Roll);

#endif
