#ifndef UART_H_
#define UART_H_
void UART_Init_styr();
unsigned char UART_Recieve_styr();
void UART_Transmit_styr(unsigned char data);
void UART_Init_kom();
void UART_Transmit_kom(unsigned char data);

#endif /* UART_H_ */