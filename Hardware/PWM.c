#include "PWM.h"              

/**
  * @brief  ��ʼ��TIM1_CH1,TIM1_CH4��ΪPWM���
  * @param  ��
  * @retval ��
  */
void PWM_Init(void) {
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1; //PWMƵ��һ��Ҫ�죬������ת����һ��һ��
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode  = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
    
    TIM_OC1Init(TIM1,&TIM_OCInitStructure); //��ʼ������ �øո����õĲ��� ���뵽��Ӧ�Ĵ�������
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);      //�ò���/�Ƚ�1�Ĵ��� Ԥװ�ع���ʹ�� ͬʱ����CC1ͨ��Ϊ���
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);      //�ò���/�Ƚ�1�Ĵ��� Ԥװ�ع���ʹ�� ͬʱ����CC4ͨ��Ϊ���

    TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);        //ȷ����TIM1���PWM

	TIM_ARRPreloadConfig(TIM1,ENABLE);      //�Զ���װ��Ԥװ������
}

void PWM_Restrict(int *Motor_1, int *Motor_2) {

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
	
	TIM_SetCompare1(TIM1, Motor_1);
	TIM_SetCompare4(TIM1, Motor_2);
}
