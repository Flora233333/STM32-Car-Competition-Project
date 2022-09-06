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
#include "Motor.h"
#include "PWM.h"  
#include "Control.h"

float pitch,roll,yaw; 		//ŷ����:�����ǣ�ƫ���ǣ���ת��
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����  
short gyrox,gyroy,gyroz;	//������ԭʼ����  
short temp;					//�¶�
uint8_t status;             //����ģ��״̬

union DATA {
	float Recive[4];        //float��ȡ
	uint8_t raw[32];        //ԭʼ����
};

int main(void);				//������
void SYS_Init(void);		//ϵͳ��ʼ���ܺ���
void Receive_Data(void);
