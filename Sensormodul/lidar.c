#define F_CPU 14745600
#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include "lidar.h"
#include <stdint.h>
#include <avr/interrupt.h>
unsigned int dist_old = 100;

unsigned int distance_lidar()
{
	unsigned int distance = 1;
	while(PIND & 0b100000000); //Fast när hög
	while(!(PIND & 0b10000000)); //fast när låg
	
	unsigned int pwm = 1;
	while(pwm != 0)
	{
		distance = distance + 1;
		_delay_us(10);
		pwm = PIND & 0b10000000;
		
		if (distance > 1100) //abort om över 11 meter (fel)
		{
			distance = distance_lidar();
			pwm = 0;
		}
	}
	if (dist_old + 4   < distance || dist_old - 4 > distance ) //Filtrera bort värden som skiljer sig för mycket
		{
			dist_old = distance;
			distance = distance_lidar();
		}
	dist_old = distance;
	return distance;
}
unsigned int distance_lidar_precise(unsigned int precision) //avg över precise gånger (funkar)
{
	unsigned int d = 0;
	for(int i = 0; i< precision; i++)
	{
		d = d + distance_lidar();
	}
	return (unsigned int)(d/precision);
}

