#pragma once
#include <stm32f4xx_hal.h>

#define DHT11_IO_SET GPIOE->BSRR = GPIO_PIN_2
#define DHT11_IO_RESET GPIOE->BSRR = (uint32_t)GPIO_PIN_2 << 16U
#define DHT11_IO_READBIT HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)

union
{
	uint32_t VALUE;

	struct{
		uint8_t Hum_H;
		uint8_t Hum_L;
		uint8_t Temp_H;
		uint8_t Temp_L;
	};

}DHT11_Data;

void DHT11_Init(void);
uint8_t* DHT11_GetResult(void);

static void Mode_Input(void);
static void Mode_Output(void);
static uint8_t DHT11_ReadByte(void);
static _Bool DHT11_UpdateData(void);

extern void Delay_us(uint32_t);
extern void Delay_ms(uint16_t);
