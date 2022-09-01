#include "Control.h"

Control mode;

float KP = 18;
float KD = 0.18;

void Control_Init(void) {
    mode.flag = 0;                                   //初始化mode参数   
    mode.status = 0;
}

float PID_Turn(float x, float yaw) {
	float Turn;     
    float Bias;	

	Bias = 90 - yaw;
	Turn = -Bias * KP - yaw * KD;

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

void Turn_Left(void) {

}

void Turn_Right(void) {

}

void Mode_Select(void) {

    if(mode.flag == 1) 
    {
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

        case 3: //左转
            Turn_Left();
            break;

        case 4: //右转
            Turn_Right();
            break;

        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    }
}
