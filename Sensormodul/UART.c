#define F_CPU 14745600
#define BAUD 31 // 11, 15, 23, 31, 47, 63, 95 ökande är lägre hastighet
#include <avr/io.h>
#include "UART.h"
void UART_Init_styr()
{
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	//Baudrate
	UBRR1H = (unsigned char)(BAUD>>8);
	UBRR1L = (unsigned char)BAUD;
	//enable transmit/recieve
	
	//stopbitar, 1
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}
unsigned char UART_Recieve_styr()
{
	while( !(UCSR1A & (1<<RXC1)) );
	
	return UDR1;
}

void UART_Transmit_styr(unsigned char data)
{
	while( !(UCSR1A & (1<<UDRE1)) );

	UDR1 = data;
}
void UART_Init_kom()
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);//|(1<<RXCIE0);
	//Baudrate
	UBRR0H = (23>>8);
	UBRR0L = 23;
	//enable transmit/recieve
	//stopbitar, 1
	UCSR0C =  (1<<USBS0)|(3<<UCSZ00);
}
void UART_Transmit_kom(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0)) );
	
	UDR0 = data;
}

unsigned char UART_Recieve_styr_kom()
{
	while( !(UCSR0A & (1<<RXC0)) );
	
	return UDR0;
}