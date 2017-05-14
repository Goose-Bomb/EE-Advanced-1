#pragma once
#include <stm32f4xx_hal.h>
#include <string.h>
#include <math.h>

#define UART1_RX_BUFFER_SIZE 1024

extern UART_HandleTypeDef huart1;
extern uint8_t Rx_Data[];
extern uint16_t Rx_Len;
extern _Bool UART_RecvEnd;

extern ADC_HandleTypeDef hadc1;

static void SystemClock_Config(void);
static void Error_Handler(void);
static void Sweep_Frequency(void);
extern void DMA_Init(void);
extern void UART1_Init(void);
extern void ADC1_Init(void);
extern void Key_Init(void);

extern void LCD_Init(void);

extern void AD9833_Init(void);
extern void AD9833_SwitchMode(void);
extern void AD9833_WriteFreq(uint32_t);

extern void DHT11_Init(void);
extern uint8_t* DHT11_GetResult(void);

extern uint8_t* DS18B20_GetTemperature(void);

extern TIM_HandleTypeDef htim1;
extern void Delay_ms(uint16_t);
extern void Delay_us(uint32_t);
extern void TIM1_PWM_Init(uint16_t);
