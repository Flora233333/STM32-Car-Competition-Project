#include "PWM.h"              

/**
  * @brief  初始化TIM1_CH1,TIM1_CH4作为PWM输出
  * @param  无
  * @retval 无
  */
void PWM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;              //时间基 结构体变量
	GPIO_InitTypeDef GPIO_InitStruct;							 //GPIO初始化 结构体变量
	TIM_OCInitTypeDef TIM_OCInitStruct;							 //通道初始化 结构体变量
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);          //使能TIM1定时器时钟线
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	     //使能GPIOA时钟线
    
	//TIM_InternalClockConfig(TIM1);
    
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;     //这里跟TIM1 产生PWM波功能无关
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //向上计时模式
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;                 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;                 
																							 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);              //初始化函数 让刚刚配置的参数 输入到对应寄存器里面
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                 //GPIO采用复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;         //TIM1同时产生两路PWM波 在管脚a8 a11
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;               //GPIO速度50MHZ
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);                           //初始化函数 让刚刚配置的参数 输入到对应寄存器里面
	
    TIM_OCStructInit(&TIM_OCInitStruct);                         //以后必须带这句，否则会有意想不到的BUG

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;               //PWM1模式
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性高
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;   //让捕获/比较寄存器使能
	TIM_OCInitStruct.TIM_Pulse = 0;      //初始化占空比0   占空比可以依照TIM_Period进行配置 在它范围内就好了
	
	TIM_OC1Init(TIM1,&TIM_OCInitStruct); //初始化函数 让刚刚配置的参数 输入到对应寄存器里面
	TIM_OC4Init(TIM1,&TIM_OCInitStruct); //初始化函数 让刚刚配置的参数 输入到对应寄存器里面
	TIM_Cmd(TIM1,ENABLE);                //使能定时器TIM1
	TIM_CtrlPWMOutputs(TIM1,ENABLE);     //确定让TIM1输入PWM
    
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);   //让捕获/比较1寄存器 预装载功能使能 同时配置CC1通道为输出
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);   //让捕获/比较1寄存器 预装载功能使能 同时配置CC4通道为输出
	TIM_ARRPreloadConfig(TIM1,ENABLE);   //自动重装载预装载允许
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
