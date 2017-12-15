#ifndef F_CPU
#define F_CPU 14745600UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "REGLERING.h"

#define Man_speed 220U   //PWM till motorerna för manuellt läge
#define Turn_speed 80U   //PWM för för svängar
#define Rotate_speed 120U//PWM för roteringar


//GLOBALS
volatile unsigned char errorMess;
volatile unsigned char programState;


void Auto_Rotate(int dir) //Kollar vilket håll vi ska rotera och starta roteringsprogrammet 
{
	
	if (dir==1)  // höger
	{
		UART_Transmit_sens(0b00000011);
		// while(( !(UCSR0A & (1<<RXC0)))) //Stoppa nr roterat 90,
		//	 {
		Pwm_Gen(Rotate_speed,Rotate_speed,1, 0);
		
		// }
	}
	else //vänster
	{
		UART_Transmit_sens(0b00000010);
		//	 while(( !(UCSR0A & (1<<RXC0)))) //Stoppa nr roterat 90
		//	 {
		Pwm_Gen(Rotate_speed,Rotate_speed,0, 1 );
		
		// }
	}
	
}

void Auto_Cont(unsigned char inst){        //Kollar vilken typ av automanöver som skall ske
 	unsigned char Auto = inst & 0b00001111;
	if(Auto == 0b00000001){ //fram
		UART_Transmit_sens(0b00000001);
		//	UART_Transmit_com('F');
		Regler_Func(0);
		
		//kolla om vi krt 40?
	}
	else if(Auto == 0b00000010){ //bak
		Regler_Func(1);
		//kolla om vi krt 40? delay first?
	}
	else if(Auto == 0b00000100){ //Rot vnster
		Auto_Rotate(0);
	}
	else if(Auto == 0b00001000){ //Rot hger
		Auto_Rotate(1);
	}
}

void Man_Cont(unsigned char inst) // Kollar typ av manuell styrinstruktion och sätter PWM till motorerna
{
	UART_Transmit_sens(0b00010000);
	char dir = (inst >> 1) & 0b00000001;
	char Man = inst & 0b00001111;
	if( Man == 0b00000001 || Man == 0b00000011)// || inst == 0b00110000){ Fram/bak
		{
			if (dir==1)
			{UART_Transmit_com('F');}
			else
			{UART_Transmit_com('B');}
			Pwm_Gen(Man_speed,Man_speed, dir, dir);
		}
		else if(Man == 0b00000100)		//Rot hger
		{
			UART_Transmit_com('H');
			Pwm_Gen(Man_speed,Man_speed,0,1); // 01
		}
		else if( Man == 0b00001000)			// Rot vnster
		{
			UART_Transmit_com('V');
			Pwm_Gen(Man_speed,Man_speed,1,0);
		}
		else if(Man == 0b00000111)			// Svng hger framt
		{
			Pwm_Gen(Man_speed,Turn_speed,dir,dir);
		}
		else if (Man == 0b00001011)               // Svng vnster framt
		{
			Pwm_Gen(Turn_speed,Man_speed,dir,dir);
		}
		else if(Man == 0b00000101)
		{
			Pwm_Gen(Turn_speed,Man_speed,dir,dir); // Svng hger bakt
		}
		else if (Man == 0b00001001)
		{
			Pwm_Gen(Man_speed,Turn_speed,dir,dir); // Svng vnster bakt
		}
		else
		{
			Pwm_Gen(0,0,dir,dir);
		}
	}
	
	//ISR(USART0_RX_vect)
	//{
	//inst = UDR0;
	// reti();
	//}
	int main(void)
	{
		UART_Init_sens();
		pwm_init();
		UART_Init_com();
		//sei();
		programState = 'S';
		errorMess = 'E';
		K = 30;
		KR = 0;
		KD = 30;
		unsigned char inst = 0b00000000;
		int Mode;
		{
			while(1)
			{
				//inst = 0b00000001;
				inst = UART_Recieve_com();  //Hämtar instruktion från kommunikationsmodulen
				
				Mode = (inst>>4) & 0b00000001;
				
				if ( inst == 0b00000000)
				{
					Pwm_Gen(0x00,0x00,0,0);
				}
				if ( ((inst>>6) & 0b00000011) == 2)
				{
					K = inst & 0b01111111;
					
					Pwm_Gen(0x00,0x00,0,0);
				}
				else if (( ((inst>>6) & 0b00000011) == 1))
				{
					KD = inst & 0b00111111;
				}
				else if (( ((inst>>6) & 0b00000011) == 3))
				{
					KR = inst & 0b00111111;
				}
				else if(Mode == 1 )
				{
					UART_Transmit_com('M');
					Man_Cont(inst);
				}
				else if(Mode == 0 )
				{
					UART_Transmit_com('A');

					Auto_Cont(inst);
				}
				
				else {
					inst = 0b00000000;
					Mode = 0;
				}
			}
		}
	}
