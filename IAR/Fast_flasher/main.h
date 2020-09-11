#pragma once

/*##############################################################################
#
# ОПИСАНИЕ: ВСПОМОГАТЕЛЬНЫЕ ЭЛЕМЕНТЫ МОДУЛЯ MAIN
# ФАЙЛ: main.h
#
##############################################################################*/

#define ACK 0xAA
#define FAIL 0xAB
#define COMPLETE 0xAC
#define BUSY 0xAD
#define WAIT 0xAE

volatile buf[259]; // буфер
uint8_t pbuf;      // указатель на буфер
uint8_t state;     // текущий статус программатора

volatile uint8_t udata;
volatile bool isData;
bool dataFlow;

void usart(uint8_t data);
uint8_t spi(uint8_t data);
void ACK();