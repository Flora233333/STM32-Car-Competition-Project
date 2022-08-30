#include "main.h"

uint8_t Ds18b20 [8];
uint8_t txbuf[32];
union DATA re;
uint8_t status;

int main() {
	SYS_Init();
	while(1) {
		MPU_Read();    //MPU6050数据读取
		DATA_Report(); //MPU6050数据上报
				
	}
}

void SYS_Init()
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组函数
	OLED_Init();									 //OLED初始化
	MPU_Init();										 //初始化MPU6050
    
    BEEP_GPIO_Config();
	USART_Config();
	
	SPI_NRF_Init();
	Check();
	NRF_RX_Mode(); 									 //默认接收模式
	
	DS18B20_Init();
	DS18B20_ReadId(Ds18b20);
	
	Timer1_InternalClock_Init();

	while(mpu_dmp_init())                            //初始化mpu_dmp库
 	{
		OLED_ShowString(1,1,"Failed");			 	 //显示字符串
		OLED_Clear();                              
		delay_ms(200);          
		OLED_Clear();							
	}
	OLED_ShowString(1, 1, "successed");
	delay_ms(200);          
	OLED_Clear();
	OLED_ShowString(1,5,"OK!");						 //显示字符串
	delay_ms(999);									 //延时初界面显示
	mpu6050.flag = 0;                                //采集成功标志位初始化
	mpu6050.speed = 0;								 //上报速度初始化
	
	TIM_Cmd(TIM1, ENABLE);
}

void MPU_Read()
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)			//dmp处理得到数据，对返回值进行判断
	{ 
		temp = MPU_Get_Temperature();	                //得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	    //得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);		//得到陀螺仪数据
		mpu6050.speed++;                            	//上报速度自加
		if(mpu6050.speed == 4)							//上报速度阈值设置
		{
			mpu6050.flag = 1;							//采集成功标志位设置为有效
			mpu6050.speed = 0;							//上报速度归零
		}
		re.Recive[1] = pitch;
		re.Recive[2] = roll;
		re.Recive[3] = yaw;
        
        if (pitch > 60 || pitch < -60 || roll > 60 || roll < -60)
            BEEP(ON);
        else 
            BEEP(OFF);
       
	}
	else 												//采集不成功										
	{
		mpu6050.flag = 0;								//采集成功标志位设置为无效
	}	
}

void DATA_Report(void)
{
	if(mpu6050.flag == 1)							 //采集成功时
	{ 
		temp = pitch * 10;							 //赋temp为pitch
		if(temp < 0) {								 //对数据正负判断，判断为负时
			temp = -temp;						     //对负数据取反		
		}
		else{}                                    	 
		OLED_ShowString(2, 1, "Pitch:00.0");	
		OLED_ShowNum(2, 7, temp / 10, 2);
		OLED_ShowNum(2, 10, temp % 10, 1);
			
		temp = roll * 10;                            
		if(temp < 0)									 //对数据正负判断，判断为负时
		{
			temp = -temp;						   	  	 //对负数据取反	
		}
		else{}                                   	 //判断为正时
		OLED_ShowString(3, 1, "Roll:00.0");	
		OLED_ShowNum(3, 6, temp / 10, 2);
		OLED_ShowNum(3, 9, temp % 10, 1);
		
		temp = yaw * 10;                           
		if(temp < 0)								//对数据正负判断，判断为负时
		{
			temp = -temp;						    //对负数据取反
		}
		else{}                                    //判断为正时
		OLED_ShowString(4, 1, "Yaw:00.0");	
		OLED_ShowNum(4, 5, temp / 10, 2);
		OLED_ShowNum(4, 8, temp % 10, 1);
			
		mpu6050.flag = 0;						//复位	
	}
	else;										//防卡死
}

void Check(void) {
	status = NRF_Check(); 

  /*判断连接状态*/  
    if(status == SUCCESS)	   
     printf("\r\nNRF1与MCU连接成功\r\n");  
    else	  
     printf("\r\nNRF1与MCU连接失败\r\n");
}

void Send_Data(void) {
	
	re.Recive[0] = DS18B20_GetTemp_MatchRom(Ds18b20); //温度值
	
	NRF_TX_Mode();
			   
	status = NRF_Tx_Dat(re.send);
		  
	/* 发送数据的状态 */
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
