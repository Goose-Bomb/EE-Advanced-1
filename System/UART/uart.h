#pragma once
#include <stm32f4xx_hal.h>

#define UART1_RX_BUFFER_SIZE 1024

void UART1_Init(void);
static void Error_Handler();