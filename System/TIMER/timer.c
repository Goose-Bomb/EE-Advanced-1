#include "timer.h"

TIM_HandleTypeDef htim1;

void Delay_ms(uint16_t ms)
{
	uint32_t temp;
	SysTick->LOAD = ms * 21000;
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	do
	{
		temp = SysTick->CTRL;
	} while (temp & 0x01 && !(temp & (1 << 16)));
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
}

void Delay_us(uint32_t us)
{
	uint32_t temp;
	SysTick->LOAD = us * 21; //168Mhz 8 分频                      
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	do
	{
		temp = SysTick->CTRL;
	} while (temp & 0x01 && !(temp & (1 << 16)));
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
}

void TIM1_PWM_Init(uint16_t Period)
{
	__HAL_RCC_TIM1_CLK_ENABLE();

	TIM_OC_InitTypeDef OC_Config;
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	htim1.Instance = TIM1;						//定时器3
	htim1.Init.Prescaler = 83;					//1us 递进
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;	//向上计数模式
	htim1.Init.Period = Period;					//自动重装载值
	htim1.Init.ClockDivision = 0;				//分频?不存在的!
	HAL_TIM_PWM_Init(&htim1);					//初始化PWM

	OC_Config.OCMode = TIM_OCMODE_PWM1;
	OC_Config.Pulse = Period >> 1;
	OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
	OC_Config.OCFastMode = TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(&htim1, &OC_Config, TIM_CHANNEL_1);
}
