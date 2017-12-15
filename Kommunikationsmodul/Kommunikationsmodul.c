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
volatile unsigned char sensor_in[21];	// //kort högerfram 0, kort h?gerbak 1, v?nsterkort 2, bak 3, h?ger l?ng 4, v?nster l?ng 5, framl?g 6, framH?G 7, PWM höger 8, PWM vänster 9, sens prog 10, sens fel 11, styr prog 12 , klarbyte rotate (sens prog) 13, styr fel 14,  kom prog 15, kom fel 16, roterat höger 17, rob_y 18, rob_x 19, direction 20
volatile int sensor_in_length;
volatile int laptop_in_length;
volatile int counter;
volatile int counter2 = 0;
volatile unsigned char matrix[25][50] = {{0}};// {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
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
volatile unsigned char recieve_renewed = 0;
volatile unsigned char kokStart_x = 0;
volatile unsigned char kokStart_y = 0;
#define rotate_left 0b00000100 
#define rotate_right 0b00001000 
#define forward 0b00000001// k?r fram
#define stop 0b00000000 //stannar
#define back 0b00000011 //k?r bak
#define klar 0b11111111 //rotation klar
#define knapp_U 0b00110000 //U knappen på laptop
#define Auto_knapp_U 0b00100001 // laptop auto update
#define man 0b00010000 // manuell frpn laptop
#define rak_mot_vagg 0b00001111 // be styr räta upp sig mot väggen

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
volatile unsigned int stop_dist = 19;


void update_position();
void kolla_efter_kok_left();
void state1();

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
	sensor_in[20] = direction;
	sensor_in[17] = dir_dir;
	
}

void send_matrix()
{
	//	UART1_Transmit(0b11111111);
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
	UART1_Transmit(klar);
}

void send_to_laptop() //inte kalla för tätt
{
	cli();

	unsigned char sens_safe[21];
	for(int i = 0;i<21;i++) // kopierar sensor_in
	{
		sens_safe[i] = sensor_in[i];
	}
	sei();
	//UART1_Transmit((unsigned char)sensor_index_low); // Säger till laptopen att vi börjar skicka bytes
	//UART1_Transmit((unsigned char)(sensor_index_high - sensor_index_low)); // Antal bytes/värden vi vill skicka

	//skickar mellan dom valda indexen i sensor_in. Detta ger oss möjlighet att skicka mer eller mindre beroende på hur mycket tid vi har
	for(int i = 0; i<21 ; i++)
	{
		laptop_in = 0;
		UART1_Transmit(sens_safe[i]);
		while(laptop_in != 33);
	}
		laptop_in = 0;
		recieve_renewed = 0;
	
	
}

void send_koordinater(unsigned char y, unsigned char x)
{
	UART1_Transmit(y);
	check = UART1_Recieve();
		if (check != 0b00110000)
		{
			sensor_in[9] = 1;
			sensor_in[10] = 1;			
		}
			
	UART1_Transmit(x);
	check = UART1_Recieve();
		if (check != 0b00110000)
		{
			sensor_in[9] = 1;
			sensor_in[10] = 1;
		}
}

ISR(USART1_RX_vect) // tAR EMOT FR?N LAPTOP
{
	laptop_in = UDR1;
	recieve_renewed = 1;
}

ISR(USART0_RX_vect)		// tAR EMOT FR?N SENSOR
{
	sensor_in[counter2] = UDR0;				// tar emot alla sensorv?rden efter vi har k?rt 40cm och l?gger in i sensorvectorn
	counter2 = counter2 + 1;
	if (counter2 == 14)
	{
		counter2 = 0;
	}
	New_sens = 1;
}

void get_low_high()
{
	cli();
	low = sensor_in[6];
	high = sensor_in[7];
	sei();
}

void get_ny_fram()
{
	get_low_high();
	ny_fram = (low + (high*256)); // tar nytt fram värde som vi jämför med gamla
}

void rot_right() // rotera höger och väntar till den är klar
{	
	//sensor_in[14] = (unsigned char)50; //satus för kom till laptop 50  = rot_left

	UART0_Transmit(stop);
		if (driven >= 13)
		{
			update_position();
			driven = 0;
		}
	harRot = 1;
	//_delay_ms(100);
	UART0_Transmit(rotate_right);

	while (sensor_in[13] != klar);
	
	UART0_Transmit(stop);
	update_direction(0);
	_delay_ms(100); //låt den stanna och få in nya lidarvärdet framåt
	get_ny_fram();
	start_dist = ny_fram;
	send_to_laptop();
	
	UART0_Transmit(forward);
// 	while(start_dist - 24 < ny_fram)
// 	{
// 		get_ny_fram();
// 	}	
}

void rot_left() // rotera vänster och väntar till den är klar
{
	//sensor_in[14] = (unsigned char)51;  //satus för kom till laptop 51 = rot_left

	UART0_Transmit(stop);
		if (driven >= 13)
		{
			update_position();
			driven = 0;
		}
	harRot = 1;
	_delay_ms(10);
	UART0_Transmit(rotate_left);

	while (sensor_in[13] != klar);
	
		
// 	UART0_Transmit(stop);
// 	UART0_Transmit(rak_mot_vagg);
// 	while (sensor_in[13] != rak_mot_vagg);
	
	
	UART0_Transmit(stop);
	update_direction(1);
	_delay_ms(100); //låt lidarn mäta när rotten stannat
	
	switch(direction)
	{
	case 0:
	if (sensor_in[4] < 40)
		matrix[bot_y][bot_x + 1] = wall;
	break;
	case 1:
	if (sensor_in[4] < 40)
		matrix[bot_y - 1][bot_x] = wall;
	break;
	case 2:
	if (sensor_in[4] < 40)
		matrix[bot_y][bot_x - 1] = wall;
	break;
	case 3:
	if (sensor_in[4] < 40)
		matrix[bot_y + 1][bot_x] = wall;
	break;
	}
	
	get_ny_fram();
	start_dist = ny_fram;
	send_to_laptop();
}

void go_forward() // tar nytt fram värde samt skickar kör fram signal och resetar roteratsignalen
{	
	//sensor_in[14] = (unsigned char)52;  //satus för kom till laptop 52 = forward
	sensor_in[13] = 0b00000000;
	if (first == 1)
	{
		UART0_Transmit(forward);
		first = 0;
	}
	
	//_delay_ms(5);
	//if(fram>25)
		//send_to_laptop();
	//else
	if ((sensor_in[0] < 20) && (sensor_in[1] < 20 ))
	{
		harRot = 0;
		first = 1;
	}
	
	
}

void navigation2() // testa att bara svänga när väggar inte finns, ej 40cm
{
	get_ny_fram();
	cli();

	unsigned char sens_safe[20];
	for(int i=0; i<20;i++)
	{
		sens_safe[i] = sensor_in[i];
	}
	sei();
	
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
if ((state == 2) && (harRot == 0 ))
kolla_efter_kok_left();

if ( (sens_safe[0] > 30) && (sens_safe[1] > 30) && (sens_safe[4] > 40) && (harRot == 0))
{
	_delay_ms(2);
	rot_right();
}

else if ((sens_safe[4] < 40) && (ny_fram < stop_dist) && (sensor_in[2] < 30) &&  (harRot == 0)) // 18 13dec. 21 14 dec
{
	rot_left();
	stop_dist = 15;
}

else if (harRot == 1) // ska köra fram om vi rotera förra gången
{
	go_forward();
}
} 

void handle_incoming_data()
{
	cli();
	unsigned char incoming_safe = laptop_in;
	sei();
	switch(incoming_safe)
	{
		case forward:
			AUTO = 1;
			MANUELL = 0;
			get_ny_fram();
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
	get_ny_fram();
	while(ny_fram > 15){
		get_ny_fram();
	}
	UART0_Transmit(stop);
	harRot = 0;
	driven = 0;
	sensor_in[13] = 0x00;
	
	if (KokFarFarAway == 1)
	{
	switch (direction)
	{
	sensor_in[15] = 20;

		// om kört 80 cm,
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
	rot_left(); //rotera vänster
	}
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
	get_ny_fram();
	while(ny_fram > 15){
		get_ny_fram();
	}
	UART0_Transmit(stop);
	harRot = 0;
	driven = 0;
	sensor_in[13] = 0x00;

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
				rot_left(); //rotera vänster
		}
	state = 1;
	KokKlart = 1;
}

void kolla_efter_kok_left()
{
	switch (direction)
	{
		case 0: //norr
		if (matrix[bot_y][bot_x - 1] == unknown)
		{
			if ((sensor_in[5] < 25) && (sensor_in[3] < 25))
			{
				matrix[bot_y][bot_x - 1] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((sensor_in[5] < 70) && (sensor_in[3] < 70) && (matrix[bot_y][bot_x - 2] == unknown) )
			{
				KokFarFarAway = 1;
				sensor_in[16] = KokFarFarAway;
				matrix[bot_y][bot_x - 2] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 1: //ÖST
		if (matrix[bot_y + 1][bot_x] == unknown)
		{
			if ((sensor_in[5] < 25) && (sensor_in[3] < 25))
			{
				matrix[bot_y + 1][bot_x] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((sensor_in[5] < 80) && (sensor_in[3] < 80) && (matrix[bot_y + 2][bot_x] == unknown) )
			{
				KokFarFarAway = 1;
				matrix[bot_y + 2][bot_x] = wall; // köksö hittad
		//		driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 2:
		if (matrix[bot_y][bot_x + 1] == unknown)
		{
			if ((sensor_in[5] < 25) && (sensor_in[3] < 25)) // ökat från 20
			{
				matrix[bot_y][bot_x + 1] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
				
			}
			else if((sensor_in[5] < 80) && (sensor_in[3] < 80) && (matrix[bot_y][bot_x + 2] == unknown) ) // ökat från 60
			{
				KokFarFarAway = 1;
				matrix[bot_y][bot_x + 2] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
		
		case 3:
		if (matrix[bot_y - 1][bot_x] == unknown)
		{
			if ((sensor_in[5] < 25 && (sensor_in[3] < 25)))
			{
				matrix[bot_y - 1][bot_x] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((sensor_in[5] < 80) && (sensor_in[3] < 80) && (matrix[bot_y - 2][bot_x] == unknown) )
			{
				KokFarFarAway = 1;
				matrix[bot_y - 2][bot_x] = wall; // köksö hittad
				driven = 0; // för att inte gå in i update igen inuti rot_left
				UART0_Transmit(stop);
				rot_left();
				sensor_in[13] = 0;
				UART0_Transmit(stop);
				kokHittad();
			}
		}
		break;
	}
}

void kolla_efter_kok_fram()
{
	switch (direction)
	{
		case 0: //norr
		//bot_y = (bot_y + 1); // uppdaterar robotens position
		if (matrix[bot_y + 1][bot_x] == unknown)
		{
			if (ny_fram < 25)
			{
				matrix[bot_y + 1][bot_x] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((ny_fram < 70) && (matrix[bot_y + 2][bot_x] == unknown) )
			{
				KokFarFarAway = 1;
				matrix[bot_y + 2][bot_x] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
// 			else
// 			{
// 				matrix[bot_y + 1][bot_x] = empty;
// 			}
		}
		break;
		
		case 1: //ÖST
		//bot_x = (bot_x + 1);
		if (matrix[bot_y][bot_x + 1] == unknown)
		{
			if (ny_fram < 25)
			{
				matrix[bot_y][bot_x + 1] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((ny_fram < 70) && (matrix[bot_y][bot_x + 2] == unknown) )
			{
				KokFarFarAway = 1;
				matrix[bot_y][bot_x + 2] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
// 			else
// 			{
// 			//	matrix[bot_y][bot_x + 1] = empty;
// 			}
		}
		break;
		
		case 2:
		//bot_y = (bot_y - 1);
		if (matrix[bot_y - 1][bot_x] == unknown)
		{
			if (ny_fram < 25) // ökat från 20
			{
				matrix[bot_y - 1][bot_x] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
				
			}
			else if((ny_fram < 70) && (matrix[bot_y - 2][bot_x] == unknown) ) // ökat från 60
			{
				KokFarFarAway = 1;
				matrix[bot_y - 2][bot_x] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
// 			else
// 			{
// 				//matrix[bot_y - 1][bot_x] = empty;
// 			}
		}
		break;
		
		case 3:
		//bot_x = (bot_x - 1);
		if (matrix[bot_y][bot_x - 1] == unknown)
		{
			if (ny_fram < 25)
			{
				matrix[bot_y][bot_x - 1] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
			else if((ny_fram < 70) && (matrix[bot_y][bot_x - 2] == unknown) )
			{
				KokFarFarAway = 1;
				matrix[bot_y][bot_x - 2] = wall; // köksö hittad
				UART0_Transmit(stop);
				kokHittad();
			}
// 			else
// 			{
// 			//	matrix[bot_y][bot_x - 1] = empty;
// 			}
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
				if (sensor_in[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y][(bot_x + 1)] = wall;
					sensor_in[15] = bot_y;
					sensor_in[16] = matrix[bot_y][(bot_x + 1)];
				}
			}		
			break;
			
			case 1: //ÖST
			bot_x = (bot_x + 1);
			if (matrix[bot_y - 1][bot_x] == unknown) // kollar åt höger
			{
				if (sensor_in[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y - 1][bot_x] = wall;
				}
			}		
			break;
			
			case 2:
			bot_y = (bot_y - 1);
			if (matrix[bot_y][bot_x - 1] == unknown) // kollar åt höger
			{
				if (sensor_in[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y][bot_x - 1] = wall;
				}
			}			
			break;
			
			case 3:
			bot_x = (bot_x - 1);
			if (matrix[bot_y + 1][bot_x] == unknown) // kollar åt höger
			{
				if (sensor_in[4] < 40) // uppdaterar matrisen om det är vägg på höger sida
				{
					matrix[bot_y + 1][bot_x] = wall;
				}
			}
			break;
		}
}

void update_position() // gör switch case senare
{
	//sensor_in[15] = sensor_in[15] + 1;
	stop_dist = 19;
	get_ny_fram();
	state1();
	matrix[bot_y][bot_x] = drive;
	sensor_in[19] = (unsigned char)bot_x;
	sensor_in[18] = (unsigned char)bot_y;
	
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
/////////   VARIABLER    ///////////////////////////
sensor_in_length = sizeof(sensor_in) / sizeof(sensor_in[0]);			// l?ngd av sensor vector
counter = 0;
counter2 = 0;
PORTB = 0x01;
  
while(1)
    {
		if(recieve_renewed)
			{
				 handle_incoming_data();
			}
			
		if(AUTO == 1 && New_sens == 1)
		{
			navigation2(); 
			New_sens = 0;
		}
		if(MANUELL == 1)
		{ 
			UART0_Transmit(laptop_in);
		}
 }
}