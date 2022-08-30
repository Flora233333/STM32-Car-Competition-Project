#include "stm32f10x.h"                  // Device header

void Timer1_InternalClock_Init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //使能时钟
	
	TIM_InternalClockConfig(TIM1); //选择晶振源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; //时基单元配置
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;   //定时1s
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1; 
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
										  
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //NVIC中断分组
	
	NVIC_InitTypeDef NVIC_InitStruct; //NVIC配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	//TIM_Cmd(TIM1, ENABLE); //开启时钟计数器
}

//void TIM1_UP_IRQHandler() {
//	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
//											 
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}
