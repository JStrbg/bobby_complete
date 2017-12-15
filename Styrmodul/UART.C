#ifndef F_CPU
#define F_CPU 14745600UL
#endif
#include "UART.h"
#include <avr/io.h>
#define BAUD_SENS 31
#define BAUD_COM 23

void UART_Init_com()
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);//|(1<<RXCIE0);
	//Baudrate
	UBRR0H = (BAUD_COM>>8);
	UBRR0L = BAUD_COM;
	//enable transmit/recieve
	//stopbitar, 1
	UCSR0C =  (1<<USBS0)|(3<<UCSZ00);
}
void UART_Init_sens()
{
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);//|(1<<RXCIE1);
	//Baudrate
	UBRR1H = (unsigned char)(BAUD_SENS>>8);
	UBRR1L = (unsigned char)BAUD_SENS;
	//enable transmit/recieve
	//stopbitar, 1
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}
void UART_Transmit_com(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0)) );
	UDR0 = data;
}
unsigned char UART_Recieve_sens()
{
	while( !(UCSR1A & (1<<RXC1)) );
	return UDR1;
}
unsigned char UART_Recieve_com()
{
	while( !(UCSR0A & (1<<RXC0)) );
	return UDR0;
}
void UART_Transmit_sens(unsigned char data)
{
	while( !(UCSR1A & (1<<UDRE1)) );
	UDR1 = data;
}
void USART1_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
	
}
void USART0_Flush( void )
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}