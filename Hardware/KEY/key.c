#include "key.h"

uint32_t frequency;

void Key_Init(void)
{
	frequency = 1000;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_5))
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
		__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_5);
		return;
	}

	if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_6))
	{
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
		__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
		return;
	}
	
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	Delay_ms(50);

	switch (GPIO_Pin)
	{
	case GPIO_PIN_3:
		if (frequency >= 6000000) break;
			frequency += 100;
			AD9833_WriteFreq(frequency);
		break;

	case GPIO_PIN_4:
		if (frequency <= 100) break;
			frequency -= 100;
			AD9833_WriteFreq(frequency);
		break;

	case GPIO_PIN_6:
			AD9833_SwitchMode();
		break;
	}
}
