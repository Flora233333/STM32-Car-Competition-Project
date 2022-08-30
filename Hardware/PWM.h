#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"
#include "bsp_usart.h"

void PWM_Init(void);
void PWM_Updata(float angle);

#endif
