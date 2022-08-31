#include "main.h"

Control mode;
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

void Mode_Select(void) {

    switch (mode.status)
    {
    case 0: //ͣ��

        break;

    case 1: //ǰ��

        break;

    case 2: //����

        break;

    case 3: //��ת

        break;

    case 4: //��ת

        break;

    default:
        printf("Error:%s, %d\r\n", __FILE__, __LINE__);
        break;
    }

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
    uint8_t i = 0;
    /* �ȴ��������� */
	status = NRF_Rx_Dat(rxbuf.raw);

	/* �жϽ���״̬ */
	if(status == RX_DR)
	{
        for(i=0;i<32;i++)
        {	
            printf("\r\nRecive Data = %d \r\n",rxbuf.raw[i]); 
        }
            
        printf("\r\nRecive Mode\r\n"); 
    }	
    
    Mode_Select();

    NRF_RX_Mode();  
}
