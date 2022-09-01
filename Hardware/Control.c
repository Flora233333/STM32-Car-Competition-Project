#include "Control.h"

Control mode;

float out = 0;
float KP = 7;
float KD = 0.07;
float Ek = 0, Ek_0 = 0;

int Motor_1 = 0, Motor_2 = 0;

void Control_Init(void) {
    mode.flag = 0;                                   //初始化mode参数   
    mode.status = 0;
    mode.angle = 0;
}

float PID_Turn(float yaw) {
	float Turn;     

	// Bias = mode.angle - 90;
	// Turn = -Bias * KP - yaw * KD;
    Ek = mode.angle - yaw;

    Turn = Ek * KP + (Ek - Ek_0) * KD;

    Ek_0 = Ek;

	return Turn;
}

void Stop(void) {
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);
}

void Go_Ahead(void) {
    Motor1_SetDirct(1, 0);
    Motor2_SetDirct(1, 0);

    TIM_SetCompare1(TIM1, 200);
    TIM_SetCompare4(TIM1, 200);
}

void Go_Back(void) {
    Motor1_SetDirct(0, 1);
    Motor2_SetDirct(0, 1);

    TIM_SetCompare1(TIM1, 200);
    TIM_SetCompare4(TIM1, 200);
}

void Turn(void) {
    static int i = 0;

    out = PID_Turn(MPU_Data.yaw);

    Motor_1 = (int)out;
    Motor_2 = (int)out;

    PWM_Restrict(&Motor_1, &Motor_2);

    PWM_Updata(Motor_1, Motor_2);

    if(mode.angle == MPU_Data.yaw) { //判断是否到位
        i++;
        if(i > 20) {
            i = 0;
            mode.status = 0;         //停车
        }
    }
}

void Mode_Select(void) {

    //if(mode.flag == 1) 
    //{
        mode.flag = 0;

        switch (mode.status)
        {
        case 0: //停车
            Stop();
            break;

        case 1: //前进
            Go_Ahead();
            break;

        case 2: //后退
            Go_Back();
            break;

        case 3: //左转(由发送的数据包决定左右转)
            Turn();
            break;

        case 4: //右转
            Turn();
            break;

        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    //}
}
