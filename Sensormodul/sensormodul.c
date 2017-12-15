#define F_CPU 14745600
#include <util/delay.h>
#include <avr/io.h>
#include "i2c_master.h"
#include "lidar.h"
#include "gyro.h"
#include "UART.h"
#include "sensorFunctions.h"
#include <avr/interrupt.h>
volatile int start = 0;
volatile unsigned char new_rotate_left = 0;
volatile unsigned char new_rotate_right = 0;
volatile unsigned char klar = 0;
volatile unsigned int framDist;
volatile unsigned char receive = 0;

void send_to_styr();

void Port_Init()
{
	DDRB = 0b11111111;
	DDRD = 0b01100110;
	PORTD &= ~(1 << trigger); //nolla trigger detta startar lidarn i continuous mode
}
void send_to_com() //s�nder alla sensorv�rden till com
{
	//l�gg in lidarn f�rst
	//unsigned int d = distance_lidar_precise(16);
	//Tryck in status och fel f�r sensormod
	sensor[11] = 'A'; // sensor prog
	sensor[12] = 'B'; // sensor fel
	for(int i=0;i<14; i++) // 11 totalt ju
	{
		//_delay_ms(100);
		UART_Transmit_kom(sensor[i]);
	}
	//d1 = distance_lidar_precise(2);
	//start = 0;
}
void rot_left() // Roterar vänster, kom skicka rot till styr, styr skickar rot till sens. sens skickar klar till kom
{
	PORTB = 0x03;
	sensor[9] = (unsigned char)51;
	//sensor[13] = 0b00000000;
	//send_to_com();
	uint32_t time1;
	long int vinkel = 0;
	int result;
	while (vinkel < 112000000)	   //125000000 testad gradsekundkonstant
	{
		TCNT1 = 0;
		result = getGyros_precise(8);
		time1 = TCNT1;
		vinkel  = vinkel + (result * time1);
	}
	Get_Sensor_Value();
	while((sensor[0] - 17) > sensor[1])
	{
		Get_Sensor_Value();
	}
	sensor[13] = 0b11111111;
//	send_to_com();
	PORTB = 0x01;
	//while (receive != 0b00001111)
// 	while (!(UCSR1A & (1<<RXC1)))
// 	{
// 		Get_Sensor_Value();
// 		send_to_styr();
// 	}
	PORTB = 0x02;
		Get_Sensor_Value();
		sens_to_centi();
		framDist = distance_lidar(); //dela upp lidarvärdets övre och undre del då hela inte får plats i en char
		sensor[6] = (unsigned char)framDist;
		sensor[7] = (unsigned char)(framDist >> 8);
//		sensor[13] = 0b00001111;
		send_to_com();
	sensor[13] = 0b00000000;
	klar = 0;
	PORTB = 0x00;
	
}
void rot_right() //Roterar 90 grader höger
{
	PORTB = 0x03;
	sensor[9] = (unsigned char)51;
	
	long int vinkel = 0;
	uint32_t time1;
	int result;
	while (vinkel > -125000000)	   //-123000000 också testad (inte kalkylerad) gradsekundkonstant som när nådd skickar klarsignal till kom-modul
	{
		TCNT1 = 0;
		result = getGyros_precise(8); //_precise filtrerar värdet genom att ta avarage över inparameterns gånger
		time1 = TCNT1;
		vinkel  = vinkel + (result * time1);
	}
	sensor[13] = 0b11111111;  //klarsignal till komunikation
	Get_Sensor_Value();
	sens_to_centi();
	framDist = distance_lidar();
	sensor[6] = (unsigned char)framDist;
	sensor[7] = (unsigned char)(framDist >> 8);
	send_to_com();
	//UART_Transmit_styr(0b00000000);
	sensor[13] = 0b00000000;
	PORTB = 0x00;
}
ISR(USART1_RX_vect) //Ta emot instruktion
{
	unsigned char data = UDR1;
	if (data == 0b00000001) //Startsignal
	{
		start = 1;
	}
	else if (data == 0b00000010)
	new_rotate_left = 1;//rot_left();
	else if (data == 0b00000011)
	new_rotate_right = 1; //rot_right();
	else if (data == 0b00010000)
	start = 0;
	else if (data == 0b11111111)
	klar = 1;
	else if(data == 0b00001111)
	receive = 0b00001111;
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
	sensor[9] = (unsigned char)40; //Send to styr
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
	//inits klar
	sei();
	centimeter_values(); //fyller i med hash-tabellen.
	unsigned int update_laptop = 0;
	while(1)//Main loop
	{
		Get_Sensor_Value();
		send_to_styr();
		update_laptop = update_laptop + 1;
		if(update_laptop == 10) //skickar sensorvärden 10 ggr oftare till styrmodulen då den behöver värden snabbare pga reglering
		{
			framDist = distance_lidar();
			sensor[6] = (unsigned char)framDist;
			sensor[7] = (unsigned char)(framDist >> 8);
			
			Get_Sensor_Value();
			sens_to_centi();
			send_to_com();
			
			update_laptop = 0;
		}
		if (new_rotate_left == 1) //Om interruptdriven mottagen instruktion är rot-left
		{
			rot_left();
			new_rotate_left = 0;
			update_laptop = 9;
		}
		if (new_rotate_right == 1)//Om interruptdriven mottagen instruktion är rot-right
		{
			rot_right();
			new_rotate_right = 0;
			update_laptop = 9;
		}
	}
}