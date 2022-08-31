#include "PWM.h"              

/**
  * @brief  初始化TIM2_CH3,TIM2_CH4作为PWM输出
  * @param  无
  * @retval 无
  */
void PWM_Init() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1; //PWM频率一定要快，否则电机转起来一顿一顿
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode  = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
    
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	TIM_Cmd(TIM2, ENABLE);
}

void PWM_Restrict(int *Motor_1, int *Motor_2) {
	if(*Motor_1 > 800) *Motor_1 = 800;
	if(*Motor_2 > 800) *Motor_2 = 800;
	
	if(*Motor_1 < -800) *Motor_1 = -800;
	if(*Motor_2 < -800) *Motor_2 = -800;
}

void PWM_Updata(int Motor_1, int Motor_2) {
	if(Motor_1 > 0)
	{
		Motor1_SetDirct(1, 0);
	}
	else 
	{
		Motor1_SetDirct(0, 1);
		Motor_1 = -1 * Motor_1;
	}
	
	if(Motor_2 > 0)
	{
		Motor2_SetDirct(1, 0);
	}
	else
	{
		Motor2_SetDirct(0, 1);
		Motor_2 = -1 * Motor_2;
	}
	
	TIM_SetCompare3(TIM2, Motor_1);
	TIM_SetCompare4(TIM2, Motor_2);
}
