#ifndef __usart_h__
#define __usart_h__

#define bool uint8_t
#define true 1
#define false 0

void usart_init();
void processUsart();
void SendUData(uint8_t data);
uint8_t SendSPI(uint8_t data);

#endif