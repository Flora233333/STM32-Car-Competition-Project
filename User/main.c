#include "main.h"

union DATA rxbuf; //�������ݻ�����
uint8_t status;   //����ģ��״̬

int main() {

	SYS_Init();

	while(1) {
		MPU_Read();     //MPU6050���ݶ�ȡ

		DATA_Report();  //MPU6050�����ϱ�

		Receive_Data(); //��������
	}
}

void SYS_Init()
{
	delay_init();                                    //DELAY��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��

	OLED_Init();									 //OLED��ʼ��
	MPU_Init();										 //MPU6050��ʼ��
	USART_Config();                                  //���ڳ�ʼ��
	
	SPI_NRF_Init();                                  //NRF24L01��ʼ��
	Check();                                         //���NRF24L01
	NRF_RX_Mode(); 									 //Ĭ�Ͻ���ģʽ

    Motor_Init();                                    //�����ʼ��

    //PWM_Init();                                      //PWM��ʼ����ԭ������һ��ʱPWMû��Ӧ��������53��ʱPWM��������

    //Timer3IT_Init();

	while(mpu_dmp_init())                            //��ʼ��mpu_dmp��
 	{
		OLED_ShowString(1,1,"Failed");			 	 //��ʾ�ַ���                          
		delay_ms(200);          
		OLED_Clear();							
	}
    
	OLED_ShowString(1, 1, "successed");
	delay_ms(200);          
	OLED_Clear();
	OLED_ShowString(1,5,"OK!");						 //��ʾ�ַ���
	delay_ms(999);									 //��ʱ��������ʾ
    
    //Motor_Init();                                    //�����ʼ��
   
    PWM_Init();                                      //PWM��ʼ��������ʱǰ�治Ҫ����ʱ����ʱЧ�ԵĶ���
   
}

void Check(void) {
	status = NRF_Check(); 

  /*�ж�����״̬*/  
    if(status == SUCCESS)	   
     printf("\r\nNRF1��MCU���ӳɹ�\r\n");  
    else	  
     printf("\r\nNRF1��MCU����ʧ��\r\n");
}

void Receive_Data(void) {
    /* �ȴ��������� */
	status = NRF_Rx_Dat(rxbuf.raw);

	/* �жϽ���״̬ */
	if(status == RX_DR)
	{
        if(rxbuf.raw[0] == 0xFF && rxbuf.raw[2] == 0xFF)
        {
            mode.flag = 1;
            mode.status = rxbuf.raw[1]; 
            printf("\r\nRecive Data\r\n"); 
        }
    }	
    
    Mode_Select();

    NRF_RX_Mode();  
}
