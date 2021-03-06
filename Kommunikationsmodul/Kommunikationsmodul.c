#ifndef F_CPU
#define F_CPU 14745600UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "UART.h"

//Globals
volatile int control_signal;
volatile unsigned char laptop_in = 0b00010000;	// styrvector
volatile unsigned char sensor_in[15];	// //kort högerfram 0, kort h?gerbak 1, kortfram 2, lång vänster fram 3, lång höger fram 4, lång vänster bak 5, framlåg 6, framH?G 7, sens prog 8, styr progr 9, kom prog 10, rot 11, rob_y 12 , rob_x 13, direction 14
volatile int sensor_in_length;
volatile int laptop_in_length;
volatile unsigned char matrix[25][50] = {{0}};
volatile unsigned char rows = 0;
volatile unsigned char column = 24;
volatile unsigned char bot_y = 0; // ska vara 0
volatile unsigned char bot_x = 25;
volatile int direction = 0;
volatile unsigned char wall = 11;
volatile unsigned char unknown = 00;
volatile unsigned char empty = 01;
volatile unsigned char drive = 10;
volatile int state = 0;
volatile unsigned char check = 0;
volatile unsigned int counter2 = 0;
volatile unsigned char recieve_renewed = 0;
volatile unsigned char kokStart_x = 0;
volatile unsigned char kokStart_y = 0;
#define rotate_left 0b00000100 
#define rotate_right 0b00001000 
#define forward 0b00000001// k?r fram
#define stop 0b00000000 //stannar
#define back 0b00000011 //k?r bak
#define klar 0b1000100 //rotation klar
#define knapp_U 0b00110000 //U knappen på laptop
#define Auto_knapp_U 0b00100001 // laptop auto update
#define man 0b00010000 // manuell frpn laptop

volatile unsigned int ny_fram = 0x0000;
volatile unsigned int low = 0x0000;
volatile unsigned int high = 0x0000;
volatile unsigned char AUTO = 0;
volatile unsigned char MANUELL = 0;
volatile unsigned char harRot = 0;
volatile unsigned char New_sens = 0;
volatile unsigned int start_dist = 0;
volatile unsigned int driven = 0;
volatile unsigned int KorKok = 0;
volatile unsigned int KokKlart = 0;
volatile unsigned int KokFarFarAway = 0;
volatile unsigned int first = 1;
volatile unsigned int stop_dist = 15;
volatile unsigned char sens_safe[8];

void update_position();
void kolla_efter_kok_left();
void state1();
void baby_proof_and_update();

void Init()
{
	DDRD = 0b01100110; //D-port initiering 1 utg?ng 0 ing?ng 00100110
	DDRB = 0b11111111;

	UART0_Init(); //styr och sensor
	UART1_Init(); // bl?tand
	sei(); //enable interrupts
}

void update_direction(int dir_dir) //sett från robo start är 0 fram, 1 höger, 2 bakåt, 3 vänster. input: 0 efter höger_rot, 1 efter vänster_rot
{
	switch(dir_dir)
	{
		case 0:
		direction = direction + 1;
		if(direction == 4)
		{
			direction = 0;
		}
		break;
		case 1:
		direction = direction - 1;
		if (direction < 0)
		{
			direction = 3;
		}
		break;
		
	}
	sensor_in[14] = direction;
	sensor_in[11] = dir_dir;
}

void send_matrix()
{
	for (unsigned char y = 0; y < 25; ++y)
	{
		for (unsigned char x = 0; x < 50; ++x)
		{
			if (matrix[y][x] == wall)
			{
				laptop_in = 0;
				UART1_Transmit(y);
				while(laptop_in != 33);
				
				laptop_in = 0;
				UART1_Transmit(x);
				while(laptop_in != 33);
			}
		}
	}
	UART1_Transmit(0b11111111);
}

void send_to_laptop() //inte kalla för tätt
{
	baby_proof_and_update();
	sensor_in[10] = KokFarFarAway;
	for(int i = 0; i < 8 ; ++i)
	{
		laptop_in = 0;
		UART1_Transmit(sens_safe[i]);
		while(laptop_in != 33);
	}
	for(int i = 8; i < 15; ++i)
	{
		laptop_in = 0;
		UART1_Transmit(sensor_in[i]);
		while(laptop_in != 33);
	}
	laptop_in = 0;
	recieve_renewed = 0;
}

ISR(USART1_RX_vect) // tAR EMOT FR?N LAPTOP
{
	laptop_in = UDR1;
	recieve_renewed = 1;
}

ISR(USART0_RX_vect)	// tAR EMOT FR?N SENSOR
{
	New_sens = 0;
	sensor_in[counter2] = UDR0;				// tar emot alla sensorv?rden efter vi har k?rt 40cm och l?gger in i sensorvectorn
	counter2 = counter2 + 1;
	if (counter2 == 10)
	{
		counter2 = 0;
		New_sens = 1;
	}
	
}

void get_low_high()
{
	low = sens_safe[6];
	high = sens_safe[7];
}

void get_ny_fram()
{
	get_low_high();
	ny_fram = (low + (high*256)); // tar nytt fram värde som vi jämför med gamla
}

void rot_right() // rotera höger och väntar till den är klar
{	
	UART0_Transmit(stop);
		if (driven >= 13)
		{
			update_position();
			driven = 0;
		}
	harRot = 1;
	UART0_Transmit(rotate_right);
	sensor_in[8] = 'O';
	sensor_in[10] = 'R'; //com kod
	while (sensor_in[8] != klar);
	
	UART0_Transmit(stop);
	update_direction(0);
	_delay_ms(200); //låt den stanna och få in nya lidarvärdet framåt
	
	send_to_laptop(); //uppdaterar ny fram också
	start_dist = ny_fram;
	UART0_Transmit(forward);	
}

void rot_left() // rotera vänster och väntar till den är klar
{
	UART0_Transmit(stop);
		if (driven >= 13)
		{
			update_position();
			driven = 0;
		}
	harRot = 1;
	_delay_ms(10);
	UART0_Transmit(rotate_left);

	sensor_in[8] = 'O';
	sensor_in[10] = 'L'; //com kod
	while (sensor_in[8] != klar);

	UART0_Transmit(stop);
	update_direction(1);
	_delay_ms(200); //låt lidarn mäta när rotten stannat
	
	switch(direction)
	{
	case 0:
	if ((sens_safe[4] < 40) && matrix[bot_y][bot_x + 1] != drive )
		matrix[bot_y][bot_x + 1] = wall;
	break;
	case 1:
	if ((sens_safe[4] < 40) && matrix[bot_y - 1][bot_x] != drive)
		matrix[bot_y - 1][bot_x] = wall;
	break;
	case 2:
	if ((sens_safe[4] < 40) && matrix[bot_y][bot_x - 1] != drive)
		matrix[bot_y][bot_x - 1] = wall;
	break;
	case 3:
	if ((sens_safe[4] < 40) && matrix[bot_y + 1][bot_x] != drive)
		matrix[bot_y + 1][bot_x] = wall;
	break;
	}
	
	send_to_laptop();
	start_dist = ny_fram;
}

void go_forward() // tar nytt fram värde samt skickar kör fram signal och resetar roteratsignalen
{	
	if (first == 1)
	{
		UART0_Transmit(forward);
		first = 0;
	}

	if ((sens_safe[0] < 20) && (sens_safe[1] < 20 ))
	{
		harRot = 0;
		first = 1;
	}
	
}

void baby_proof_and_update() //safear sensorvärden, inte programkoder
{
	while(New_sens != 1); //Vänta på hel bytearray
	cli();
		for(int i=0; i<8;i++)
		{
			sens_safe[i] = sensor_in[i];
		}
	sei();
	get_ny_fram();
}
void navigation() // testa att bara svänga när väggar inte finns, ej 40cm
{
	baby_proof_and_update();
	
	if (ny_fram < (start_dist))
	{
		driven = (start_dist - ny_fram);
	}
	
	if (((driven) >= 36) && (harRot == 0)) // var 35 innan // funka nästan vid 37
	{
	 	update_position();
		start_dist = ny_fram; 
		driven = 0;
	
	}


	if ( (sens_safe[0] > 30) && (sens_safe[1] > 30) && (sens_safe[4] > 40) && (harRot == 0))
	{
		_delay_ms(2);
		rot_right();
	}

	else if ((sens_safe[4] < 40) && (ny_fram < stop_dist) && (sens_safe[2] < 30) &&  (harRot == 0)) // 18 13dec. 21 14 dec
	{
		rot_left();
		stop_dist = 13;
	}

	else if (harRot == 1) // ska köra fram om vi rotera förra gången
	{
		go_forward();
	}
	
 	if ((state == 2) && (harRot == 0 ))
	kolla_efter_kok_left();
	
}
void handle_incoming_data()
{
	switch(laptop_in)
	{
		case forward:
			AUTO = 1;
			MANUELL = 0;
			baby_proof_and_update();
			start_dist = ny_fram;
			UART0_Transmit(forward);
			
		break;
		
		case man:
			AUTO = 0;
			MANUELL = 1;
			UART0_Transmit(stop);
		break;
		
		case 0b00110000:
			AUTO = 0;
			MANUELL = 1;
			send_matrix();
		break;	
		
		default:
		break;		
	}
	recieve_renewed = 0;
}

void kokHittad(){
	UART0_Transmit(forward); //kör fram till ö
	do
	{
		baby_proof_and_update();
	}while(ny_fram > 12);
	
	UART0_Transmit(stop);
	harRot = 0;
	driven = 0;
	
	if (KokFarFarAway == 1)
	{
		switch (direction)
		{
			case 0:
				bot_y = bot_y + 1;
			break;
			case 1:
				bot_x = bot_x + 1;
			break;
			case 2:
				bot_y = bot_y - 1;
			break;
			case 3:
				bot_x = bot_x - 1;
			break;
		}
	 //rotera vänster
	}
	rot_left();
	kokStart_x = bot_x;
	kokStart_y = bot_y;
	UART0_Transmit(stop);
	state = 1;
	KorKok = 1;
	//stanna
	
	//kör runt som vanligt till position
}

void go_home()
{
	driven = 0;
	rot_left();
	UART0_Transmit(forward); //kör fram till ö
	do
	{
		baby_proof_and_update();
	}while(ny_fram > 11);
	
	UART0_Transmit(stop);
	harRot = 0;
	driven = 0;
	if (KokFarFarAway == 1)
	{
		sensor_in[15] = 10;
		switch (direction)
			{
				// om kört 80 cm,
				case 0:
				bot_y = (bot_y + 1);
				break;
				case 1:
				bot_x = (bot_x + 1);
				break;
				case 2:
				bot_y = (bot_y - 1);
				break;
				case 3:
				bot_x = (bot_x - 1);
				break;
			}
	 //rotera vänster
	}
	rot_left();
	state = 1;
	KokKlart = 1;
}

void kolla_efter_kok_left()
{

	baby_proof_and_update();
	switch (direction)
	{
		case 0: //norr
		if (matrix[bot_y][bot_x - 1] == unknown || (matrix[bot_y][bot_x - 1] == drive))
		{
			if ((sens_safe[5] < 25 ) && (sens_safe[3] < 25) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) && (matrix[bot_y][bot_x - 1] == unknown) ) 
			{	
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y][bot_x - 1] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
				state = 1;
			}
			else if((sens_safe[5] > 25 ) && (sens_safe[3] > 25) && (sens_safe[5] < 75) && (sens_safe[3] < 75) && (matrix[bot_y][bot_x - 2] == unknown) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) )
			{
				KokFarFarAway = 1;
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y][bot_x - 2] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 1: //ÖST
		if (matrix[bot_y + 1][bot_x] == unknown || (matrix[bot_y + 1][bot_x] == drive))
		{
			if ((sens_safe[5] < 25) && (sens_safe[3] < 25) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) && (matrix[bot_y + 1][bot_x] == unknown) )
			{
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				
				matrix[bot_y + 1][bot_x] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((sens_safe[5] > 25 ) && (sens_safe[3] > 25) && (sens_safe[5] < 70) && (sens_safe[3] < 70) && (matrix[bot_y + 2][bot_x] == unknown) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) )
			{
				KokFarFarAway = 1;
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				
				matrix[bot_y + 2][bot_x] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 2:
		if (matrix[bot_y][bot_x + 1] == unknown || (matrix[bot_y][bot_x + 1] == drive))
		{
			if ((sens_safe[5] < 25) && (sens_safe[3] < 25) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) && (matrix[bot_y][bot_x + 1] == unknown)) // ökat från 20
			{
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y][bot_x + 1] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
				
			}
			else if((sens_safe[5] < 70) && (sens_safe[3] < 70) && (matrix[bot_y][bot_x + 2] == unknown) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) ) // ökat från 60
			{
				KokFarFarAway = 1;
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y][bot_x + 2] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 3:
		if ((matrix[bot_y - 1][bot_x] == unknown) || (matrix[bot_y - 1][bot_x] == drive))
		{
			if ((sens_safe[5] < 25 && (sens_safe[3] < 25) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20) && (matrix[bot_y - 1][bot_x] == unknown)))
			{
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y - 1][bot_x] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((sens_safe[5] < 70) && (sens_safe[3] < 70) && (matrix[bot_y - 2][bot_x] == unknown) && (sens_safe[0] < 20) && (sens_safe[1] < 20) && (sens_safe[4] < 20)  )
			{
				KokFarFarAway = 1;
				if (driven >= 30)
				{
					update_position();
					driven = 0;
				}
				matrix[bot_y - 2][bot_x] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
	}
}

void state1()
{
		switch (direction)
		{
			case 0: //norr
			bot_y = (bot_y + 1); // uppdaterar robotens position
			
			if (matrix[bot_y][(bot_x + 1)] == unknown) // kollar åt höger
			{
				if (sens_safe[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y][(bot_x + 1)] = wall;
				}
			}		
			break;
			
			case 1: //ÖST
			bot_x = (bot_x + 1);
			if (matrix[bot_y - 1][bot_x] == unknown) // kollar åt höger
			{
				if (sens_safe[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y - 1][bot_x] = wall;
				}
			}		
			break;
			
			case 2:
			bot_y = (bot_y - 1);
			if (matrix[bot_y][bot_x - 1] == unknown) // kollar åt höger
			{
				if (sens_safe[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y][bot_x - 1] = wall;
				}
			}			
			break;
			
			case 3:
			bot_x = (bot_x - 1);
			if (matrix[bot_y + 1][bot_x] == unknown) // kollar åt höger
			{
				if (sens_safe[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y + 1][bot_x] = wall;
				}
			}
			break;
		}
}

void update_position() // gör switch case senare
{
	
	stop_dist = 15;
	get_ny_fram();
	state1();
	matrix[bot_y][bot_x] = drive;
	sensor_in[12] = (unsigned char)bot_y;
	sensor_in[13] = (unsigned char)bot_x;
	
if (state == 0)
{
	state = 1;
}
	if ((state == 1) && (KorKok == 1) && (bot_x == kokStart_x) && (kokStart_y == bot_y)){
		KorKok = 0;
		go_home();
		UART0_Transmit(stop);
// 		MANUELL = 1;
// 		AUTO = 0;
	}
	else if ((state == 1) && (bot_y == 0) && (bot_x == 25))
	{
		if (KokKlart == 1)
		{
			driven = 0;
			rot_left();
 			AUTO = 0;
 			MANUELL = 1;
			UART0_Transmit(stop);
		}
		else
			state = 2;
	}
	
//  	else if (state == 2) // har kört ett varv, leta efter kökön
//  	{
// 		 kolla_efter_kok_left();
// // 		 if (KorKok == 0)
// // 		 kolla_efter_kok_fram();
// 
// 	}
}
	
int main(void)
{

Init();

while(1)
    {
		if(recieve_renewed)
			{
				 handle_incoming_data();
			}
			
		if(AUTO == 1 )
		{
			navigation(); 
		}
		if(MANUELL == 1)
		{ 
			UART0_Transmit(laptop_in);
		}
 }
}