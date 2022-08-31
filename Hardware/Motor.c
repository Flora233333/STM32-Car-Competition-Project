#include "Motor.h"                

/**
* @brief  初始化GPIOB的12,13,14,15为控制电机方向的IO口
  * @param  无
  * @retval 无
  */
void Motor_Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  初始化TIM3,TIM4为编码器器模式,检测转速
  * @param  无
  * @retval 无
  */
void Encoder_Init() {
    //使能GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //使能定时器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //因为编码器模式使用的就是通道1和通道2，这个是固定的，通道3和通道4不可配置为编码器模式，硬件设计时要注意。   
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);		

	//时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;    
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;  //配置0xFFFF是技巧
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0x00; //不要分频,否则电机会一卡一卡的转（频率太粗）
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_SetCounter(TIM3, 0);
	TIM_SetCounter(TIM4, 0);
	//开启时钟
	TIM_Cmd(TIM3, ENABLE); 
    TIM_Cmd(TIM4, ENABLE); 
}

void Motor1_SetDirct(uint16_t A1, uint16_t A2) { //设置电机1的方向
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)A1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)A2);
}

void Motor2_SetDirct(uint16_t B1, uint16_t B2) { //设置电机2的方向
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)B1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)B2);
}

uint16_t Motor_Security(float Roll) {			 //侧倾停车
	if(Roll > 50 || Roll < -50) {
		Motor1_SetDirct(0, 0);
		Motor2_SetDirct(0, 0);
		TIM_Cmd(TIM1, DISABLE);
		return 0;
	}
	return 1;
}

void TIM3_IRQHandler(void) {
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != 0)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void) {
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != 0)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
