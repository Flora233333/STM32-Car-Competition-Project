#ifndef __CONTROL_H
#define __CONTROL_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "Motor.h"
#include "mpu6050.h"
#include "PWM.h"

typedef struct {
    uint8_t status;               //模式
    uint8_t flag;                 //接收标志位
    int     angle;                //转角
} Control;

typedef struct {
    int      SetPoint;                                 //设定目标 Desired Value
    float    SumError;                                 //误差累计
    float    KP;                                       //比例常数 Proportional Const
    float    KI;                                       //积分常数 Integral Const
    float    KD;                                       //微分常数 Derivative Const
    int      LastError;                                //Error[-1]
    int      PrevError;                                //Error[-2]
} PID_TypeDef;

extern Control mode;

void Control_Init(void);
void Stop(void);
void Go_Ahead(void);
void Go_Back(void);
void Turn(void);


#endif
