#ifndef F_CPU
#define F_CPU 14745600UL
#endif
#include <avr/io.h>
#define BAUD_AVR 23
#define BAUD_LAPTOP 7

void UART1_Init();
void UART0_Init();
void UART1_Transmit(unsigned char data);
unsigned char UART1_Recieve();
void UART0_Transmit(unsigned char data);
unsigned char UART0_Recieve();