#include "main.h"
#include "Hardware\LCD\lcd.h"

uint32_t frequency = 1000;
uint16_t ADC_Result[100];

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	DMA_Init();
	UART1_Init();
	ADC1_Init();
	Key_Init();
	AD9833_Init();
	LCD_Init();
	
	//DHT11_Init();
	//TIM1_PWM_Init(100000);
	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	//HAL_UART_Receive_DMA(&huart1, Rx_Data, UART1_RX_BUFFER_SIZE);

	for (;;)
	{
		/*
		if (UART_RecvEnd)
		{
			HAL_UART_Transmit_DMA(&huart1, Rx_Data, strlen(Rx_Data));
			UART_RecvEnd = 0;
			Rx_Len = 0; 
		}*/

		//uint8_t* str = DS18B20_GetTemperature();
		//Sweep_Frequency();
		GPIOA->BSRR = GPIO_PIN_5;
		Delay_ms(100);
		GPIOA->BSRR = (uint32_t)GPIO_PIN_5 << 16U;
		Delay_ms(100);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	Delay_ms(5);
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_Pin)) return;

	switch (GPIO_Pin)
	{
	case GPIO_PIN_3:
		if (frequency >= 5000000) break;
		frequency += 100;
		AD9833_WriteFreq(frequency);
		break;

	case GPIO_PIN_4:
		if (frequency <= 100) break;
		frequency -= 100;
		AD9833_WriteFreq(frequency);
		break;

	case GPIO_PIN_5:
		AD9833_SwitchMode();
		break;

	case GPIO_PIN_6:
		Sweep_Frequency();
		AD9833_WriteFreq(frequency);
		break;
	}
	LCD_ShowString(10, 10, 100, 16, 16, "DDS Frequency");
	LCD_ShowString(165, 10, 32, 16, 16, "Hz");
	LCD_ShowNum(120, 10, frequency, 5, 16);
}


static void Sweep_Frequency(void)
{
	uint16_t x, y, i, j, freq;
	__IO uint16_t ADC_Value = 0;
	uint32_t ADC_ValueSum = 0;
	
	LCD_Clear(BACK_COLOR);
	LCD_DrawLine(39, 216, 315, 216);
	LCD_DrawLine(39, 4, 315, 4);
	LCD_DrawLine(39, 4, 39, 216);
	LCD_DrawLine(315, 4, 315, 216);

	POINT_COLOR = YELLOW;
	for (i = 94; i < 305; i+= 55)	LCD_DrawLine(i, 5, i, 215);
	for (i = 48; i < 216; i += 42)	LCD_DrawLine(40, i, 314, i);

	POINT_COLOR = WHITE;
	LCD_ShowString(3, 0, 32, 16, 16, "  0");
	LCD_ShowString(3, 42, 32, 16, 16, "-10");
	LCD_ShowString(3, 84, 32, 16, 16, "-20");
	LCD_ShowString(3, 126, 32, 16, 16, "-30");
	LCD_ShowString(3, 168, 32, 16, 16, "-40");
	LCD_ShowString(3, 210, 32, 16, 16, "-50");

	for (i = 0; i < 100; i++)
	{
		ADC_ValueSum = 0;

		freq = i * 100 + 100;
		AD9833_WriteFreq(freq);

		Delay_ms(40);
		HAL_ADC_Start_DMA(&hadc1, &ADC_Value, 1);
		Delay_ms(1);
		//printf("%u\n", ADC_Value);

		for (j = 0; j < (1 << 14); j++)
		{
			ADC_ValueSum += ADC_Value;
			Delay_us(1); 
		}
		ADC_ValueSum >>= 14;
		

		ADC_Result[i] = ADC_ValueSum * 1.2303f;

		float dB = 20.0f * log10(ADC_Result[i] / 1465.0f); //»ù×¼µçÑ¹
		printf("%u %u HZ dB: %f\n", ADC_ValueSum, i * 100 + 100, dB);
		
		if (ADC_ValueSum != 0)
		{
			x = 137 * log10(freq / 100) + 40;
			y = -3.5f * dB + 6;

			LCD_Fast_DrawPoint(x, y, RED);
			LCD_Fast_DrawPoint(x + 1, y, RED);
			LCD_Fast_DrawPoint(x - 1, y, RED);
			LCD_Fast_DrawPoint(x, y + 1, RED);
			LCD_Fast_DrawPoint(x, y - 1, RED);
		}

		LCD_ShowNum(275, 220, ADC_Result[i], 5, 16);
		LCD_ShowString(256, 220, 20, 16, 16, "Vp");
		
		HAL_ADC_Stop_DMA(&hadc1);
		Delay_ms(25);
	}
 
	Delay_ms(1000);
	//LCD_ColorFill(9, 5, 304, 211, BACK_COLOR);
	LCD_ShowNum(180, 20, ADC_Result[9], 5, 16);
	LCD_ShowNum(180, 40, ADC_Result[33], 5, 16);
	LCD_ShowNum(180, 60, ADC_Result[39], 5, 16);
	LCD_ShowNum(180, 80, ADC_Result[49], 5, 16);

	while (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6))
	{
		Delay_ms(50);
	}
	LCD_Clear(BACK_COLOR);
}

static void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 8000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();		
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
}

void Error_Handler(void)
{
	while (1); //Something went wrong!
}

