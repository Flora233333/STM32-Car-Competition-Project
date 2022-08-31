#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"
#include "Motor.h"

void PWM_Init(void);
void PWM_Restrict(int *Motor_1, int *Motor_2);
void PWM_Updata(int Motor_1, int Motor_2);

#endif
