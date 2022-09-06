#include "Control.h"

Control mode;
PID_TypeDef speed_control;
PID_TypeDef angle_control;
PID_TypeDef straight_control;

int i = 0;
float Motor_out = 0;

int Motor_1 = 0, Motor_2 = 0;

void Control_Init(void) {
    mode.flag = 0;                                   //初始化mode参数   
    mode.status = 1;
    mode.angle = 0;

    angle_control.KP = 3;
    angle_control.KI = 0;
    angle_control.KD = 2.5;
    angle_control.SetPoint = 0;
    angle_control.LastError = 0;
    angle_control.PrevError = 0;

    speed_control.SetPoint = 10;
    speed_control.LastError = 0;
    speed_control.PrevError = 0;
    
    straight_control.KP = 30;
    straight_control.KD = 0;
    straight_control.SetPoint = 0;
    straight_control.LastError = 0;
    straight_control.PrevError = 0;
}

float PID_Speed(int nowspeed) { //增量式PID
    int Ek = 0;
    static float out = 0;

    //当前编码器值/转一圈的编码器值/速度环计算周期(10ms) = 转速

    Ek = speed_control.SetPoint - nowspeed;                     //偏差(nowspeed是编码器的读数)

    if((Ek < 5 ) && (Ek > -5))
        Ek = 0;

    out = out 
            + (speed_control.KP * Ek)                           //E[k]项
            - (speed_control.KI * speed_control.LastError)      //E[k-1]项
            + (speed_control.KD * speed_control.PrevError);     //E[k-2]项

    speed_control.PrevError = speed_control.LastError;          //存储误差，用于下次计算
    speed_control.LastError = Ek;
    
    return out;  
}

/**可以试试看这个下次调编码器的时候：
 * 
 *   位置式pid控制，也就是pd控制，累计编码器脉冲作为输入，
 *   并以此为目标进行闭环控制，具体方法到我主页视频观看，
 *   就是累加编码器的脉冲，90度看看轮子转了多少，编码器有多少个脉冲，
 *   以这个目标为闭环控制，就可以控制小车旋转多少角度了
 *   左右轮可以分开试试   
 */

int LastEk_left = 0;
int LastEk_right = 0;

float PID_Turn_Encoder(int encoder, int SetPoint, int *lastError) { 
    int Ek = 0;  
    float out = 0;   

    Ek = encoder - SetPoint;

    out = Ek * angle_control.KP - (Ek - *lastError) * angle_control.KD;

    *lastError = Ek;

	return out;
}

float PID_Turn(float yaw) { //位置式PID,但基于姿态角
    float Ek = 0;  
    static float out = 0;   

    Ek = yaw - angle_control.SetPoint;

    out = Ek * angle_control.KP - (Ek - angle_control.LastError) * angle_control.KD;

    // out = out
    //         + (angle_control.KP * Ek)                           //增量式PID                          
    //         - (angle_control.KI * angle_control.LastError)      
    //         + (angle_control.KD * angle_control.PrevError);

    // angle_control.PrevError = angle_control.LastError;          //存储误差，用于下次计算
    // angle_control.LastError = Ek;

    angle_control.LastError = Ek;

	return out;
}

float PID_Straight(float yaw) {
    float out = 0;
    float Ek = 0;
    //static float out = 0;

    Ek = yaw - straight_control.SetPoint;

    if((Ek < 1) && (Ek > -1))
        Ek = 0;

    // out =  (straight_control.KP * Ek)                                 //增量式PID                          
    //        - (straight_control.KI * straight_control.LastError)      
    //        + (straight_control.KD * straight_control.PrevError);
    
    // straight_control.PrevError = straight_control.LastError;          //存储误差，用于下次计算
    // straight_control.LastError = Ek;
          
	out = Ek * straight_control.KP - (Ek - straight_control.LastError) * straight_control.KD; //位置式PID

	straight_control.LastError = Ek;

    return out;
}

void Stop(void) {
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);
}

void Go_Ahead(void) {
    int straight;
    // int Left_speed, right_speed;
    // int Encoder_Left, Encoder_Right;

    // Encoder_Left = Get_M1Encoder();
    // Encoder_Right = -Get_M2Encoder();

    Motor1_SetDirct(1, 0);
    Motor2_SetDirct(1, 0);

    straight = (int)PID_Straight(MPU_Data.yaw);
    // Left_speed = PID_Speed(Encoder_Left);
    // right_speed = PID_Speed(Encoder_Right);	

    Motor_1 = 500 + straight;                  
    Motor_2 = 500 - straight;

    // PWM_Restrict(&Motor_1, &Motor_2);

    // PWM_Updata(Motor_1, Motor_2);         					
	
    TIM_SetCompare1(TIM1, Motor_1);
    TIM_SetCompare4(TIM1, Motor_2);
}

void Go_Back(void) {
    Motor1_SetDirct(0, 1);
    Motor2_SetDirct(0, 1);

    TIM_SetCompare1(TIM1, 500);
    TIM_SetCompare4(TIM1, 500);
}

void Turn(void) {
    printf("yaw = %f  ", MPU_Data.yaw);

    Motor_out = PID_Turn(MPU_Data.yaw);

    Motor_1 = (int)Motor_out;
    Motor_2 = -(int)Motor_out;

    PWM_Restrict(&Motor_1, &Motor_2);

    PWM_Updata(Motor_1, Motor_2);
    
    if((angle_control.SetPoint - MPU_Data.yaw <= 2) && (angle_control.SetPoint - MPU_Data.yaw >= -2)) { //判断是否到位
        printf("del = %f  ", angle_control.SetPoint - MPU_Data.yaw);
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
            angle_control.SetPoint = mode.angle - 2;
            Turn();
            printf("Turn Left %d\r\n", mode.angle);
            break;

        case 5: //右转
            angle_control.SetPoint = -mode.angle + 2;
            Turn();
            printf("Turn Right %d\r\n", mode.angle);
            break;

        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    //}
}
