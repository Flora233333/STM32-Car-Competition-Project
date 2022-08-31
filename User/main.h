#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "NRF24L01.h"
#include "Timer.h"
#include "bsp_usart.h"
#include "sys.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

float pitch,roll,yaw; 		//欧拉角:俯仰角，偏航角，滚转角
short aacx,aacy,aacz;		//加速度传感器原始数据  angular acceleration
short gyrox,gyroy,gyroz;	//陀螺仪原始数据  gyroscope
short temp;					//温度

typedef struct {
    uint8_t status;           //模式

} Control;

union DATA {
	float Recive[4];        //float读取
	uint8_t raw[32];        //原始数据
};

int main(void);				//主函数
void SYS_Init(void);		//系统初始化总函数
void Check(void);
void Mode_Select(void);
void Receive_Data(void);
