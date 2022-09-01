#include "stm32f10x.h"                  // Device header

void Timer3_InternalClock_Init() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能时钟
	
	TIM_InternalClockConfig(TIM3); //选择晶振源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; //时基单元配置
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;   //定时1s
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1; 
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
		
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);    
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
	
	NVIC_InitTypeDef NVIC_InitStruct; //NVIC配置
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3, ENABLE); //开启时钟计数器
}

//void TIM3_IRQHandler() {
// 	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
// 		
// 		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
// 	}
// }
