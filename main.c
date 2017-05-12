#include "main.h"
#include "Hardware\LCD\lcd.h"

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
	
	GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	HAL_UART_Receive_DMA(&huart1, Rx_Data, UART1_RX_BUFFER_SIZE);

	for (;;)
	{
		if (UART_RecvEnd)
		{
			HAL_UART_Transmit_DMA(&huart1, Rx_Data, strlen(Rx_Data));
			UART_RecvEnd = 0;
			Rx_Len = 0; 
		}

		//uint8_t* str = DS18B20_GetTemperature();
		
		Sweep_Frequency();

		//LCD_Set_Window(0, 0, 240, 24);
		
	}
}

static void Sweep_Frequency(void)
{
	uint16_t x, y, i;
	__IO uint16_t ADC_Value = 0;

	LCD_DrawLine(8, 216, 313, 216);
	LCD_DrawLine(8, 4, 313, 4);
	LCD_DrawLine(8, 4, 8, 216);
	LCD_DrawLine(313, 4, 313, 216);

	POINT_COLOR = YELLOW;
	for (i = 48; i < 305; i+= 42)
	{
		LCD_DrawLine(i, 5, i, 215);
	}

	for (i = 48; i < 216; i += 42)
	{
		LCD_DrawLine(8, i, 313, i);
	}

	POINT_COLOR = WHITE;

	HAL_ADC_Start_DMA(&hadc1, &ADC_Value, 1);

	for (i = 0; i < 100; i++)
	{
		AD9833_WriteFreq(i * 100 + 100);
		Delay_ms(50);
		//printf("%u\n", ADC_Value);

		ADC_Result[i] = ADC_Value / 1.24121f;
		x = i * 3 + 9;
		y = (4096 - ADC_Value) * 0.05127f + 6;

		LCD_Fast_DrawPoint(x, y, RED);
		LCD_Fast_DrawPoint(x+1, y, RED);
		LCD_Fast_DrawPoint(x-1, y, RED);
		LCD_Fast_DrawPoint(x, y+1, RED);
		LCD_Fast_DrawPoint(x, y-1, RED);
		LCD_ShowNum(280, 220, ADC_Result[i], 4, 16);
		
		GPIOB->BSRR = GPIO_PIN_0;
		Delay_ms(50);
		GPIOB->BSRR = (uint32_t)GPIO_PIN_0 << 16U;
	}

	HAL_ADC_Stop_DMA(&hadc1);
	LCD_ColorFill(9, 5, 300, 211, BACK_COLOR);
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

