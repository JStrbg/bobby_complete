#include "UART.h"

void UART1_Init()
{
	//enable transmit/recieve
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	//Baudrate
	UBRR1H = (BAUD_LAPTOP>>8);
	UBRR1L = BAUD_LAPTOP;

	//stopbitar, 1
	UCSR1C = (3<<UCSZ10);
}

void UART0_Init()
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	//Baudrate
	UBRR0H = (BAUD_AVR>>8);
	UBRR0L = BAUD_AVR;
	//enable transmit/recieve
	//stopbitar, 1
	UCSR0C =  (1<<USBS0)|(3<<UCSZ00);
}

void UART1_Transmit(unsigned char data)
{
	while( !(UCSR1A & (1<<UDRE1)) );

	UDR1 = data;
}

unsigned char UART1_Recieve()
{
	
	while( !(UCSR1A & (1<<RXC1)) );

	return UDR1;
}

void UART0_Transmit(unsigned char data)
{
	
	while( !(UCSR0A & (1<<UDRE0)) );

	UDR0 = data;
}

unsigned char UART0_Recieve()
{
	
	while( !(UCSR0A & (1<<RXC0)) );

	return UDR0;
}

