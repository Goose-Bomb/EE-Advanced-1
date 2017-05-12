#pragma once
#include <stm32f4xx_hal.h>

void Key_Init(void);
extern void AD9833_SwitchMode(void);
extern void AD9833_WriteFreq(uint32_t);
extern void Delay_ms(uint16_t);