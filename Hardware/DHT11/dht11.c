#include "DHT11.h"

char result[50];

void DHT11_Init(void)
{
	Mode_Output();
	//	DHT11_IO_SET;
	DHT11_Data.VALUE = 0;
}

uint8_t* DHT11_GetResult(void)
{
	if (DHT11_UpdateData())
	{
		sprintf(result, "%u degree, %u %%\n", DHT11_Data.Temp_H, DHT11_Data.Hum_H);
	}
	else
	{
		sprintf(result, "Failed!");
	}

	return result;
}

static _Bool DHT11_UpdateData(void)
{
	Mode_Output();
	DHT11_IO_RESET;
	Delay_ms(20);
	DHT11_IO_SET;
	Delay_us(40);

	Mode_Input();
	
	if (DHT11_IO_READBIT) return 0;
	Delay_us(80);
	if (!DHT11_IO_READBIT) return 0;

	while (DHT11_IO_READBIT);

	DHT11_Data.Hum_H = DHT11_ReadByte();
	DHT11_Data.Hum_L = DHT11_ReadByte();
	DHT11_Data.Temp_H = DHT11_ReadByte();
	DHT11_Data.Temp_L = DHT11_ReadByte();
	uint8_t Check = DHT11_ReadByte();

	return (DHT11_Data.Hum_H + DHT11_Data.Hum_L + DHT11_Data.Temp_H + DHT11_Data.Temp_L == Check);
}

static uint8_t DHT11_ReadByte(void)
{
	uint8_t byte = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		while (!DHT11_IO_READBIT);

		Delay_us(30);
		byte <<= 1;

		if (DHT11_IO_READBIT == 1)
		{
			byte |= 1;
		}

		while (DHT11_IO_READBIT);
	}

	return byte;
}

static void Mode_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_2;

	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static void Mode_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_2;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}