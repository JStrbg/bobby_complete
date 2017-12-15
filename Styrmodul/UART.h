#ifndef UART_H_
#define UART_H_
void UART_Init_com();
void UART_Init_sens();
void UART_Transmit_com(unsigned char data);
unsigned char UART_Recieve_sens();
unsigned char UART_Recieve_com();
void UART_Transmit_sens(unsigned char data);
void USART1_Flush( void );
void USART0_Flush( void );
#endif /* UART_H_ */