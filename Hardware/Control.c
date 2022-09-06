#include "Control.h"

Control mode;
PID_TypeDef speed_control;
PID_TypeDef angle_control;

int i = 0;
float Motor_out = 0;

int Motor_1 = 0, Motor_2 = 0;

void Control_Init(void) {
    mode.flag = 0;                                   //初始化mode参数   
    mode.status = 1;
    mode.angle = 0;

    angle_control.KP = 0.7;
    angle_control.KD = 0.07;

    //speed_control.SetPoint = 600;
}

float PID_Speed(float NextPoint) {
    float Ek, out;

    Ek = (float)speed_control.SetPoint - NextPoint;                 //偏差

    if((Ek < 0.2f ) && (Ek > -0.2f))
        Ek = 0.0f;

    out = (speed_control.KP * Ek)                                   //E[k]项
                - (speed_control.KI * speed_control.LastError)      //E[k-1]项
                + (speed_control.KD * speed_control.PrevError);     //E[k-2]项

    speed_control.PrevError = speed_control.LastError;              //存储误差，用于下次计算
    speed_control.LastError = Ek;
    
    return out;  
}

float PID_Turn(float yaw) {
	float out;
    float Ek;     

	// Bias = mode.angle - 90;
	// Turn = -Bias * KP - yaw * KD;
    Ek = mode.angle - yaw;

    //Ek = (Ek > 0 ? Ek : -Ek);
    //printf("Ek = %f\r\n", Ek);

    out = Ek * angle_control.KP + (Ek - angle_control.LastError) * angle_control.KD;

    angle_control.LastError = Ek;

	return out;
}

void Stop(void) {
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);
}

void Go_Ahead(void) {
    Motor1_SetDirct(1, 0);
    Motor2_SetDirct(1, 0);

    TIM_SetCompare1(TIM1, 100);
    TIM_SetCompare4(TIM1, 100);
}

void Go_Back(void) {
    Motor1_SetDirct(0, 1);
    Motor2_SetDirct(0, 1);

    TIM_SetCompare1(TIM1, 100);
    TIM_SetCompare4(TIM1, 100);
}

void Turn(void) {
    printf("yaw = %f  ", MPU_Data.yaw);

    Motor_out = PID_Turn(MPU_Data.yaw);

    Motor_1 = (int)Motor_out;
    Motor_2 = (int)Motor_out;

    PWM_Restrict(&Motor_1, &Motor_2);

    PWM_Updata(Motor_1, Motor_2);
    
    if((mode.angle - MPU_Data.yaw <= 2) && (mode.angle - MPU_Data.yaw >= -2)) { //判断是否到位
        printf("del = %f  ", mode.angle - MPU_Data.yaw);
        i++;
        if(i > 10) {
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
        case 1: //停车
            Stop();
            printf("Stop\r\n");
            break;

        case 2: //前进
            Go_Ahead();
            printf("Go_Ahead\r\n");
            break;

        case 3: //后退
            Go_Back();
            printf("Go_Back\r\n");
            break;

        case 4: //左转(由发送的数据包决定左右转)
            Turn();
            mode.angle = mode.angle;
            printf("Turn Left %d\r\n", mode.angle);
            break;

        case 5: //右转
            Turn();
            mode.angle = -mode.angle;
            printf("Turn Right %d\r\n", mode.angle);
            break;

        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    //}
}
