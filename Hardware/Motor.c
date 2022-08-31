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
  * @brief  初始化TIM2,TIM4为编码器器模式,检测转速
  * @param  无
  * @retval 无
  */
void Encoder_Init() {
    //使能GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //使能定时器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //因为编码器模式使用的就是通道1和通道2，这个是固定的，通道3和通道4不可配置为编码器模式，硬件设计时要注意。   
    GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB6 PB7
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PA0 PA1
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

	//时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;    
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;  //配置成0xFFFF
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0x00; //不要分频,否则电机会一卡一卡的转（频率太粗）
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_SetCounter(TIM2, 0);
	TIM_SetCounter(TIM4, 0);
	//开启时钟
	TIM_Cmd(TIM2, ENABLE); 
    TIM_Cmd(TIM4, ENABLE); 
}

//设置电机1的旋转方向
void Motor1_SetDirct(uint16_t A1, uint16_t A2) { 
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)A1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)A2);
}

//设置电机2的旋转方向
void Motor2_SetDirct(uint16_t B1, uint16_t B2) { 
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)B1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)B2);
}

void Stop(void) {

}

void Go_Ahead(void) {

}

void Go_Back(void) {

}

void Turn_Left(void) {

}

void Turn_Right(void) {

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
