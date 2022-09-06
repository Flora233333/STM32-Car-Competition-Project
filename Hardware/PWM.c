#include "PWM.h"              

/**
  * @brief  ��ʼ��TIM1_CH1,TIM1_CH4��ΪPWM���
  * @param  ��
  * @retval ��
  */
void PWM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;              //ʱ��� �ṹ�����
	GPIO_InitTypeDef GPIO_InitStruct;							 //GPIO��ʼ�� �ṹ�����
	TIM_OCInitTypeDef TIM_OCInitStruct;							 //ͨ����ʼ�� �ṹ�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);          //ʹ��TIM1��ʱ��ʱ����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	     //ʹ��GPIOAʱ����
    
	//TIM_InternalClockConfig(TIM1);
    
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;     //�����TIM1 ����PWM�������޹�
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ�ʱģʽ
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;                 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;                 
																							 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);              //��ʼ������ �øո����õĲ��� ���뵽��Ӧ�Ĵ�������
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                 //GPIO���ø����������ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;         //TIM1ͬʱ������·PWM�� �ڹܽ�a8 a11
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;               //GPIO�ٶ�50MHZ
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);                           //��ʼ������ �øո����õĲ��� ���뵽��Ӧ�Ĵ�������
	
    TIM_OCStructInit(&TIM_OCInitStruct);                         //�Ժ�������䣬����������벻����BUG

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;               //PWM1ģʽ
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;       //������Ը�
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;   //�ò���/�ȽϼĴ���ʹ��
	TIM_OCInitStruct.TIM_Pulse = 0;      //��ʼ��ռ�ձ�0   ռ�ձȿ�������TIM_Period�������� ������Χ�ھͺ���
	
	TIM_OC1Init(TIM1,&TIM_OCInitStruct); //��ʼ������ �øո����õĲ��� ���뵽��Ӧ�Ĵ�������
	TIM_OC4Init(TIM1,&TIM_OCInitStruct); //��ʼ������ �øո����õĲ��� ���뵽��Ӧ�Ĵ�������
	TIM_Cmd(TIM1,ENABLE);                //ʹ�ܶ�ʱ��TIM1
	TIM_CtrlPWMOutputs(TIM1,ENABLE);     //ȷ����TIM1����PWM
    
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);   //�ò���/�Ƚ�1�Ĵ��� Ԥװ�ع���ʹ�� ͬʱ����CC1ͨ��Ϊ���
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);   //�ò���/�Ƚ�1�Ĵ��� Ԥװ�ع���ʹ�� ͬʱ����CC4ͨ��Ϊ���
	TIM_ARRPreloadConfig(TIM1,ENABLE);   //�Զ���װ��Ԥװ������
}

void PWM_Restrict(int *Motor_1, int *Motor_2) {
    if(*Motor_1 > 300)
        *Motor_1 = 300;
    if(*Motor_1 < -300)
        *Motor_1 = -300;

    if(*Motor_2 > 300)
        *Motor_2 = 300;
    if(*Motor_2 < -300)
        *Motor_2 = -300;
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
