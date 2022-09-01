#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "Motor.h"

typedef struct {
    uint8_t status;           //模式
    uint8_t flag;             //接收标志位
} Control;

extern Control mode;

void Control_Init(void);
void Stop(void);
void Go_Ahead(void);
void Go_Back(void);
void Turn_Left(void);
void Turn_Right(void);


#endif
