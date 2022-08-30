#include "main.h"

uint8_t Ds18b20 [8];
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
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��
	OLED_Init();									 //OLED��ʼ��
	MPU_Init();										 //��ʼ��MPU6050
    
    BEEP_GPIO_Config();
	USART_Config();
	
	SPI_NRF_Init();
	Check();
	NRF_RX_Mode(); 									 //Ĭ�Ͻ���ģʽ
	
	DS18B20_Init();
	DS18B20_ReadId(Ds18b20);
	
	Timer1_InternalClock_Init();

	while(mpu_dmp_init())                            //��ʼ��mpu_dmp��
 	{
		OLED_ShowString(1,1,"Failed");			 	 //��ʾ�ַ���
		OLED_Clear();                              
		delay_ms(200);          
		OLED_Clear();							
	}
	OLED_ShowString(1, 1, "successed");
	delay_ms(200);          
	OLED_Clear();
	OLED_ShowString(1,5,"OK!");						 //��ʾ�ַ���
	delay_ms(999);									 //��ʱ��������ʾ
	mpu6050.flag = 0;                                //�ɼ��ɹ���־λ��ʼ��
	mpu6050.speed = 0;								 //�ϱ��ٶȳ�ʼ��
	
	TIM_Cmd(TIM1, ENABLE);
}

void MPU_Read()
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)			//dmp����õ����ݣ��Է���ֵ�����ж�
	{ 
		temp = MPU_Get_Temperature();	                //�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	    //�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);		//�õ�����������
		mpu6050.speed++;                            	//�ϱ��ٶ��Լ�
		if(mpu6050.speed == 4)							//�ϱ��ٶ���ֵ����
		{
			mpu6050.flag = 1;							//�ɼ��ɹ���־λ����Ϊ��Ч
			mpu6050.speed = 0;							//�ϱ��ٶȹ���
		}
		re.Recive[1] = pitch;
		re.Recive[2] = roll;
		re.Recive[3] = yaw;
        
        if (pitch > 60 || pitch < -60 || roll > 60 || roll < -60)
            BEEP(ON);
        else 
            BEEP(OFF);
       
	}
	else 												//�ɼ����ɹ�										
	{
		mpu6050.flag = 0;								//�ɼ��ɹ���־λ����Ϊ��Ч
	}	
}

void DATA_Report(void)
{
	if(mpu6050.flag == 1)							 //�ɼ��ɹ�ʱ
	{ 
		temp = pitch * 10;							 //��tempΪpitch
		if(temp < 0) {								 //�����������жϣ��ж�Ϊ��ʱ
			temp = -temp;						     //�Ը�����ȡ��		
		}
		else{}                                    	 
		OLED_ShowString(2, 1, "Pitch:00.0");	
		OLED_ShowNum(2, 7, temp / 10, 2);
		OLED_ShowNum(2, 10, temp % 10, 1);
			
		temp = roll * 10;                            
		if(temp < 0)									 //�����������жϣ��ж�Ϊ��ʱ
		{
			temp = -temp;						   	  	 //�Ը�����ȡ��	
		}
		else{}                                   	 //�ж�Ϊ��ʱ
		OLED_ShowString(3, 1, "Roll:00.0");	
		OLED_ShowNum(3, 6, temp / 10, 2);
		OLED_ShowNum(3, 9, temp % 10, 1);
		
		temp = yaw * 10;                           
		if(temp < 0)								//�����������жϣ��ж�Ϊ��ʱ
		{
			temp = -temp;						    //�Ը�����ȡ��
		}
		else{}                                    //�ж�Ϊ��ʱ
		OLED_ShowString(4, 1, "Yaw:00.0");	
		OLED_ShowNum(4, 5, temp / 10, 2);
		OLED_ShowNum(4, 8, temp % 10, 1);
			
		mpu6050.flag = 0;						//��λ	
	}
	else;										//������
}

void Check(void) {
	status = NRF_Check(); 

  /*�ж�����״̬*/  
    if(status == SUCCESS)	   
     printf("\r\nNRF1��MCU���ӳɹ�\r\n");  
    else	  
     printf("\r\nNRF1��MCU����ʧ��\r\n");
}

void Send_Data(void) {
	
	re.Recive[0] = DS18B20_GetTemp_MatchRom(Ds18b20); //�¶�ֵ
	
	NRF_TX_Mode();
			   
	status = NRF_Tx_Dat(re.send);
		  
	/* �������ݵ�״̬ */
	if(status == TX_DS)
	{
		printf("\r\nSEND SUCCESS\r\n");
	}
	else
	{
		printf("\r\nERROR = 0x%x\r\n", status);
	}
		  
	printf("\r\nRecive Mode\r\n"); 

	NRF_RX_Mode();
}

void TIM1_UP_IRQHandler() {
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
		Send_Data();						 
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
