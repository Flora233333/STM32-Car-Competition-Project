#include "Control.h"

Control mode;
PID_TypeDef speed_control;
PID_TypeDef angle_control;
PID_TypeDef straight_control;
PID_TypeDef distance_control;

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
    
    straight_control.KP = 10;
    straight_control.KD = 2.5;
    straight_control.SetPoint = 0;
    straight_control.LastError = 0;
    straight_control.PrevError = 0;

    distance_control.SetPoint = 0;
    distance_control.LastError = 0;
    distance_control.PrevError = 0;
}

float PID_Speed(int nowspeed) { //增量式PID
    int Ek = 0;
    static float out = 0;

    //当前编码器值/转一圈的编码器值/速度环计算周期(10ms) * 1000 = 转速(rad/s)

    Ek = speed_control.SetPoint - nowspeed;                     //偏差(nowspeed是编码器的读数)

    if((Ek < 5 ) && (Ek > -5))
        Ek = 0;

    out = out 
            + (speed_control.KP * (Ek - speed_control.LastError));                        
            + (speed_control.KI * Ek) ;     
            + (speed_control.KD * (Ek - 2 * speed_control.LastError + speed_control.PrevError));     

    speed_control.PrevError = speed_control.LastError;          //存储误差，用于下次计算
    speed_control.LastError = Ek;
    
    return out;  
}

float PID_Distance(int encoder) { //增量式PID
    int Ek = 0;
    static float out = 0;

    Ek = distance_control.SetPoint - encoder;                     //偏差(nowspeed是编码器的读数)

    if((Ek < 5 ) && (Ek > -5))
        Ek = 0;

    out = out 
            + (distance_control.KP * (Ek - distance_control.LastError));                        
            + (distance_control.KI * Ek) ;     
            + (distance_control.KD * (Ek - 2 * distance_control.LastError + distance_control.PrevError));     

    distance_control.PrevError = distance_control.LastError;          //存储误差，用于下次计算
    distance_control.LastError = Ek;
    
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
    //         + (angle_control.KP * (Ek - angle_control.LastError))                           //增量式PID                          
    //         + (angle_control.KI * Ek)      
    //         + (angle_control.KD * (Ek - 2 * angle_control.LastError + angle_control.PrevError));

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
          
	out = Ek * straight_control.KP - (Ek - straight_control.LastError) * straight_control.KD; //位置式PID

	straight_control.LastError = Ek;

    return out;
}

void Stop(void) {
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);
}

void Go_Ahead(void) {
    int straight = 0;
    // int Left_speed = 0, right_speed = 0;
    // int Encoder_Left = 0, Encoder_Right = 0;
    // int rad_Left = 0, rad_Right = 0;

    // Encoder_Left = Get_M1Encoder();
    // Encoder_Right = -Get_M2Encoder();
    // rad_Left = Encoder_Left / 780 / 10 * 1000;
    // rad_Right = Encoder_Right / 780 / 10 * 1000;

    Motor1_SetDirct(1, 0);
    Motor2_SetDirct(1, 0);

    straight = (int)PID_Straight(MPU_Data.yaw);
    // Left_speed = PID_Speed(rad_Left);
    // right_speed = PID_Speed(rad_Right);	

    Motor_1 = 500 - straight;                  
    Motor_2 = 500 + straight;

    PWM_Restrict_Go(&Motor_1, &Motor_2);

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

    PWM_Restrict_Turn(&Motor_1, &Motor_2);

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

void Turn_Encoder(void) {
    int Encoder_Left = 0, Encoder_Right = 0;

    Encoder_Left = TIM_GetCounter(TIM2);
    Encoder_Right = TIM_GetCounter(TIM4);

    Motor_1 = (int)PID_Turn_Encoder(Encoder_Left, 9000, &LastEk_left);
    Motor_2 = -(int)PID_Turn_Encoder(Encoder_Right, 10800, &LastEk_right);

    PWM_Restrict_Go(&Motor_1, &Motor_2);

    PWM_Updata(Motor_1, Motor_2);
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
        case 6:

            break;
        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    //}
}

void Print_Wave() {
    int SetPoint = speed_control.SetPoint;
    int now = (int)((short)TIM_GetCounter(TIM2) / 780 / 10 * 1000); //rad/s

    printf("ch: %d,%d\n", SetPoint, now);
}
