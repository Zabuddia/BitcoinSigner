#ifndef __DEVICE_H
#define __DEVICE_H

#include <bcm2835.h>

// GPIO config
#define LCD_CS 8
#define LCD_RST 27
#define LCD_DC 25
#define LCD_BL 24

// uint8_t device_init(void);
// void device_exit(void);
void delay_ms(unsigned int ms);

uint8_t DEV_ModuleInit(void);
void DEV_ModuleExit(void);

void DEV_GPIO_Init();
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode);
void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);

void DEV_SPI_WriteByte(uint8_t value);

#endif
