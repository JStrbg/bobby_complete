#ifndef F_CPU
#define F_CPU 14745600UL
#endif
#include "UART.h"
#include "REGLERING.h"
#include <avr/io.h>

#define W_distance 110U
#define base_pwm 90U

void Pwm_Gen( char Right_Pwm,  char Left_Pwm,  char Right_dir ,  char Left_dir ) //Program för att sätta pwm och riktning till motorerna
{
	UART_Transmit_sens(programState); // skicka state
	//stll in riktning + motorstyrka
	char direct;
	direct = Left_dir;
	direct = (direct << 1) + Right_dir;
	direct = direct << 4;
	PORTB = direct; //(Left_dir<<5) | (Right_dir<<4);  // vnster sida kra fram/bak
	//PORTB = PR(Right_dir<<PORTB6); // Hger sida kra fram/bak
	OCR3A = Left_Pwm;  //Pwm till vnster sida via port B7
	OCR3B = Right_Pwm; //Pwm till hger sida via port B8
}
int mult_kd(int tot_e) // Justerar KD-värdet för PD-regulator
{
	int tempKD;
	tempKD = tot_e*KD;
	tempKD = (tempKD>>6);
	return tempKD;
}
int mult_kr(int tot_e) // Justerar Kr-värdet.. Används inte just nu
{
	int tempKR;
	tempKR = tot_e*KR;
	tempKR = (tempKR>>6);
	return tempKR;
}
int mult_k(int tot_e) //Justerar K-värdet för PD-regulatorn
{
	int tempE;
	tempE = tot_e*K;
	tempE = (tempE>>6);
	return tempE;
}
void Regler_Func(int dir)//Program för att reglera mot en vägg
{  
	programState = 'S';
	int Pwm_left,Pwm_right; 
	char pwmLeftChar, pwmRightChar;
	int EV_old=0;
	int EH_old=0;
	unsigned char sen[2];  
	int Ev, Eh, StyrSignalHoger, StyrSignalVanster = 0;
	int EFram, EBak,E3,sensorFram,sensorBak;
	USART1_Flush();
	USART0_Flush();
	Get_Sensor(sen);  //Hämta sensorvärden
	while( !(UCSR0A & (1<<RXC0))) //sålänge den inte mottar en ny instruktion från com
	{
		
		if ((sen[0] < 65) || (sen[1] < 65) )
		{  //Ifall roboten inte ser en vägg på högersida, kör rakt fram
			Pwm_Gen(base_pwm,base_pwm,1,1);
		}
		else
		{
			
			sensorFram = 255 - sen[0];		    //F?r n?ra -> l?gt v?rde och vice versa
			sensorBak = 255 - sen[1];
			
			EFram = (sensorFram - W_distance); //Reglerfel: Avstånd
			EBak = (sensorBak - W_distance);
			E3 = EFram - EBak;				   //Reglerfel: SensorSkillnad 
			Ev =  E3 + EFram;                 //Totala felet
			Eh =  - E3 - EFram;

			StyrSignalVanster = mult_k(2*Ev) + mult_kd(Ev - EV_old);  //Y = e*K + KD * e*d/dt 
			StyrSignalHoger = mult_k(2*Eh) + mult_kd(Eh - EH_old);
				
			EV_old = Ev;  //Gammla styrsignalen
			EH_old = Eh;
			
			//else{
			if (StyrSignalVanster + base_pwm > 170) //Begränsingar för att undvika OverFlow och att roboten rusar iväg
			{
				Pwm_left = 170;
			}
			else if (StyrSignalVanster + base_pwm < 20)
			{
				Pwm_left = 20;
			}
			else
			{
				Pwm_left = (StyrSignalVanster + base_pwm);
			}
			/////////////////////////////////////
			if ( StyrSignalHoger + base_pwm > 170)
			{
				Pwm_right =170;
			}
			else if (StyrSignalHoger + base_pwm < 20)
			{
				Pwm_right = 20;
			}
			else
			{
				Pwm_right = (StyrSignalHoger + base_pwm);
			}
			//}
			if ((Pwm_right > base_pwm + 10) && (Pwm_left > base_pwm + 10)){
				Pwm_right = base_pwm;
				Pwm_left = 40;
			}
			pwmLeftChar = Pwm_left;
			pwmRightChar = Pwm_right;
			Pwm_Gen(pwmRightChar,pwmLeftChar,1,1);
		}
		Get_Sensor(sen);  //Hämta sensorvärden
	}
	Pwm_Gen(0,0,0,0);
}
void Get_Sensor(unsigned char* sens) // ta emot sensorvärden (för reglering)
{
	unsigned char sanity;
	sanity = UART_Recieve_sens();
	
	if (sanity == 0b00000000)
	{
		for(int i = 0;2>i;i = i+1)
		{
			sens[i] = UART_Recieve_sens();
		}
	}
	else 
		Get_Sensor(sens);//Återkallas tills den är i sync
}
void pwm_init(){
	TCCR3A=0b10100001; //Stll in pwm
	TCCR3B=0b00000011;
	TCNT0 = 0;  //PWM 0-255 (minmax)
	DDRB = 0b11111111; //B utportar
	DDRD = 0b00001010;
}
