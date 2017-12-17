#define F_CPU 14745600
#include <util/delay.h>
#include <avr/io.h>
#include "i2c_master.h"
#include "lidar.h"
#include "gyro.h"
#include "UART.h"
#include "sensorFunctions.h"
#include <avr/interrupt.h>
volatile unsigned char new_rotate_left = 0;
volatile unsigned char new_rotate_right = 0;
volatile unsigned int framDist;

void send_to_styr();

void Port_Init()
{
	DDRB = 0b11111111;
	DDRD = 0b01100110;
	PORTD &= ~(1 << trigger); //nolla trigger detta startar lidarn i continuous mode
}
void send_to_com() //s�nder alla sensorv�rden till com
{
	for(int i=0;i<10; i++) 
	{
		UART_Transmit_kom(sensor[i]);
	}
}
void rot_left() // Roterar vänster, kom skicka rot till styr, styr skickar rot till sens. sens skickar klar till kom
{
	sensor[8] = 'L'; //programkod rotera vänster
	uint32_t time1;
	long int vinkel = 0;
	int result;
	while (vinkel < 130000000)	   //125000000 testad gradsekundkonstant
	{
		TCNT1 = 0;
		result = getGyros_precise(8);
		time1 = TCNT1;
		vinkel  = vinkel + (result * time1);
	}
	do //Försök uppräta mot vägg på högersidan
	{
		Get_Sensor_Value();
	} while (((sensor[0] - 10) > sensor[1]) && ((sensor[0] > 80) && sensor[1] > 60)); //Fast här sålänge inte robot är rak och väggen inte är för långt bort
	//88 råvärde är 13 cm, 20 råvärde under 13cm är ungefär 3 - 4 cm
	sensor[8] = 'D';
	sens_to_centi();
	send_to_com();
	sensor[8] = 'F';
}
void rot_right() //Roterar 90 grader höger	
{
	sensor[8] = 'R'; //sätt sensprogramkod
	long int vinkel = 0;
	uint32_t time1;
	int result;
	while (vinkel > -129000000)	   //-123000000 också testad (inte kalkylerad) gradsekundkonstant som när nådd skickar klarsignal till kom-modul
	{
		TCNT1 = 0;
		result = getGyros_precise(8); //_precise filtrerar värdet genom att ta avarage över inparameterns gånger
		time1 = TCNT1;
		vinkel  = vinkel + (result * time1);
	}
	sensor[8] = 'D'; //klarsignal till komunikation
	Get_Sensor_Value();
	sens_to_centi();
	send_to_com();
	sensor[8] = 'F';
}
ISR(USART1_RX_vect) //Ta emot instruktion STYR
{
	sensor[9] = UDR1;

	if (sensor[9] == 'L')
		new_rotate_left = 1;//rot_left();
	else if (sensor[9] == 'R')
		new_rotate_right = 1; //rot_right();
	return;
}
// ADC Initiering
void ADC_Init()
{
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR); // v�nsterjustera resultat s� att allting hamnar i ADCH, 8 bitar
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0); //ADC enable och prescaler
}
void send_to_styr() // S�nder reglersensorer
{
	UART_Transmit_styr(0x00);
	UART_Transmit_styr(sensor[0]);
	UART_Transmit_styr(sensor[1]);
}
int main(void)
{
	//inits
	i2c_init();
	gyros_init();
	UART_Init_styr();
	Port_Init();
	ADC_Init();
	UART_Init_kom();
	TCCR1B |= (1<<CS10)|(1<<CS12); //timer init
	
	sei();
	centimeter_values(); //fyller i med hash-tabellen.
	unsigned int update_com = 0;
	while(1)//Main loop
	{
		Get_Sensor_Value();
		send_to_styr();
		update_com = update_com + 1;
		if(update_com == 10) //skickar sensorvärden 16 ggr oftare till styrmodulen då den behöver värden snabbare pga reglering
		{
			framDist = distance_lidar();
			sensor[6] = (unsigned char)framDist;
			sensor[7] = (unsigned char)(framDist >> 8);
			sens_to_centi();
			send_to_com();
			update_com = 0;
		}
		if (new_rotate_left == 1) //Om interruptdriven mottagen instruktion är rot-left
		{
			rot_left();
			new_rotate_left = 0;
			update_com = 0;
		}
		if (new_rotate_right == 1)//Om interruptdriven mottagen instruktion är rot-right
		{
			rot_right();
			new_rotate_right = 0;
			update_com = 0;
		}
	}
}