#include "main.h"

uint8_t txbuf[32];
union DATA re;
uint8_t status;

int main() {
	SYS_Init();
	while(1) {
		MPU_Read();    //MPU6050���ݶ�ȡ
		DATA_Report(); //MPU6050�����ϱ�
				
	}
}

void SYS_Init()
{
	delay_init();                                    //DELAY��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��

	OLED_Init();									 //OLED��ʼ��
	MPU_Init();										 //MPU6050��ʼ��
	USART_Config();                                  //���ڳ�ʼ��
	
	SPI_NRF_Init();
	Check();
	NRF_RX_Mode(); 									 //Ĭ�Ͻ���ģʽ

	//Timer1_InternalClock_Init();

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
	
	//TIM_Cmd(TIM1, ENABLE);
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

}

// void Send_Data(void) {
	
// 	NRF_TX_Mode();
			   
// 	status = NRF_Tx_Dat(re.send);
		  
// 	/* �������ݵ�״̬ */
// 	if(status == TX_DS)
// 	{
// 		printf("\r\nSEND SUCCESS\r\n");
// 	}
// 	else
// 	{
// 		printf("\r\nERROR = 0x%x\r\n", status);
// 	}
		  
// 	printf("\r\nRecive Mode\r\n"); 

// 	NRF_RX_Mode();
// }

// void TIM1_UP_IRQHandler() {
// 	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
// 		Send_Data();						 
// 		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
// 	}
// }
