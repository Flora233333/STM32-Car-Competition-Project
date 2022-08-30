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
//#include "bsp_ds18b20.h"
#include "sys.h"
//#include "bsp_beep.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

char tmp_buf[33];			//�ַ�������
float pitch,roll,yaw; 		//ŷ����:�����ǣ�ƫ���ǣ���ת��
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����  angular acceleration
short gyrox,gyroy,gyroz;	//������ԭʼ����  gyroscope
short temp;					//�¶�

/*
struct MPU6050				//MPU6050�ṹ��
{
	u8 flag;				//�ɼ��ɹ���־λ
	u8 speed;				//�ϱ��ٶ�
} mpu6050;					//Ψһ�ṹ�����
*/

union DATA {
	float Recive[4];
	uint8_t send[32];
};

int main(void);				//������
void SYS_Init(void);		//ϵͳ��ʼ���ܺ���
void MPU_Read(void);		//MPU6050���ݶ�ȡ����	
void DATA_Report(void);		//MPU6050�����ϱ�
void Check(void);
