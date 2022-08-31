#include "main.h"

Control mode;
union DATA rxbuf; //接收数据缓冲区
uint8_t status;   //无线模块状态

int main() {

	SYS_Init();

	while(1) {
		MPU_Read();     //MPU6050数据读取

		DATA_Report();  //MPU6050数据上报

		Receive_Data(); //接收数据
	}
}

void SYS_Init()
{
	delay_init();                                    //DELAY初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组函数

	OLED_Init();									 //OLED初始化
	MPU_Init();										 //MPU6050初始化
	USART_Config();                                  //串口初始化
	
	SPI_NRF_Init();                                  //NRF24L01初始化
	Check();                                         //检测NRF24L01
	NRF_RX_Mode(); 									 //默认接收模式

    Motor_Init();                                    //电机初始化

    //PWM_Init();                                      //PWM初始化，原先在这一行时PWM没反应，调换至53行时PWM运行正常

	while(mpu_dmp_init())                            //初始化mpu_dmp库
 	{
		OLED_ShowString(1,1,"Failed");			 	 //显示字符串                          
		delay_ms(200);          
		OLED_Clear();							
	}
    
	OLED_ShowString(1, 1, "successed");
	delay_ms(200);          
	OLED_Clear();
	OLED_ShowString(1,5,"OK!");						 //显示字符串
	delay_ms(999);									 //延时初界面显示

    mode.flag = 0;                                   //初始化mode参数   
    mode.status = 0;
    
    PWM_Init();                                      //PWM初始化
}

void Mode_Select(void) {

    if(mode.flag == 1) 
    {
        mode.flag = 0;

        switch (mode.status)
        {
        case 0: //停车
            Stop();
            break;

        case 1: //前进
            Go_Ahead();
            break;

        case 2: //后退
            Go_Back();
            break;

        case 3: //左转
            Turn_Left();
            break;

        case 4: //右转
            Turn_Right();
            break;

        default:
            printf("Error:%s, %d\r\n", __FILE__, __LINE__);
            break;
        }
    }
}

void Check(void) {
	status = NRF_Check(); 

  /*判断连接状态*/  
    if(status == SUCCESS)	   
     printf("\r\nNRF1与MCU连接成功\r\n");  
    else	  
     printf("\r\nNRF1与MCU连接失败\r\n");
}

void Receive_Data(void) {
    /* 等待接收数据 */
	status = NRF_Rx_Dat(rxbuf.raw);

	/* 判断接收状态 */
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
