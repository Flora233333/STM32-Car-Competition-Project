#include "main.h"

union DATA rxbuf; //�������ݻ�����

int main() {

	SYS_Init();
    //Go_Ahead();
	while(1) {

        // MPU_Read();     //MPU6050���ݶ�ȡ

 		// DATA_Report();  //MPU6050�����ϱ�
        
		// Receive_Data(); //��������

	}
}

void SYS_Init()
{
	delay_init();                                    //DELAY��ʼ��
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��

	OLED_Init();									 //OLED��ʼ��
	MPU_Init();										 //MPU6050��ʼ��
    Control_Init();                                  //���Ƴ�ʼ��
	USART_Config();                                  //���ڳ�ʼ��
	
	SPI_NRF_Init();                                  //NRF24L01��ʼ��
	Check();                                         //���NRF24L01
	NRF_RX_Mode(); 									 //Ĭ�Ͻ���ģʽ

    Motor_Init();                                    //�����ʼ��

    //PWM_Init();                                    //PWM��ʼ����ԭ������һ��ʱPWMû��Ӧ��������53��ʱPWM��������

    Timer3IT_Init();                                 //�򿪶�ʱ���жϼ���

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

void Receive_Data(void) {
    /* �ȴ��������� */
	status = NRF_Rx_Dat(rxbuf.raw);

	/* �жϽ���״̬ */
	if(status == RX_DR)
	{
        if(rxbuf.raw[0] == 0xFF && rxbuf.raw[3] == 0xFF)
        {
            mode.flag = 1;
            mode.status = rxbuf.raw[1]; 
            mode.angle = rxbuf.raw[2];
            printf("\r\nRecive Data\r\n"); 
        }
    }	
    
    //Mode_Select();

    NRF_RX_Mode();  
}

void TIM3_IRQHandler() {
 	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {

 		MPU_Read();     //MPU6050���ݶ�ȡ

 		DATA_Report();  //MPU6050�����ϱ�
		
		Receive_Data();

        Mode_Select();

 		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
 	}
 }
