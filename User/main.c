#include "main.h"

union DATA rxbuf; //接收数据缓冲区

int main() {

	SYS_Init();
    //Go_Ahead();
	while(1) {

        // MPU_Read();     //MPU6050数据读取

 		// DATA_Report();  //MPU6050数据上报
        
		// Receive_Data(); //接收数据

	}
}

void SYS_Init()
{
	delay_init();                                    //DELAY初始化
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组函数

	OLED_Init();									 //OLED初始化
	MPU_Init();										 //MPU6050初始化
    Control_Init();                                  //控制初始化
	USART_Config();                                  //串口初始化
	
	SPI_NRF_Init();                                  //NRF24L01初始化
	Check();                                         //检测NRF24L01
	NRF_RX_Mode(); 									 //默认接收模式

    Motor_Init();                                    //电机初始化

    //PWM_Init();                                    //PWM初始化，原先在这一行时PWM没反应，调换至53行时PWM运行正常

    Timer3IT_Init();                                 //打开定时器中断计算

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
    
    //Motor_Init();                                    //电机初始化
   
    PWM_Init();                                      //PWM初始化，长延时前面不要开定时器等时效性的东西
   
}

void Receive_Data(void) {
    /* 等待接收数据 */
	status = NRF_Rx_Dat(rxbuf.raw);

	/* 判断接收状态 */
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

 		MPU_Read();     //MPU6050数据读取

 		DATA_Report();  //MPU6050数据上报
		
		Receive_Data();

        Mode_Select();

 		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
 	}
 }
