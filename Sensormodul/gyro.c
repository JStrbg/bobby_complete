#include "i2c_master.h"
#include "gyro.h"
void gyros_init() //Sätter igång mätningar på gyroskopet och sätter känsligheten till 500 dps.
{
	i2c_start(GYRO_WRITE);
	i2c_write(0x20);
	i2c_write(0x00);
	i2c_stop();
	i2c_start(GYRO_WRITE);
	i2c_write(0x20);
	i2c_write(0x0F);
	i2c_stop();
	i2c_start(GYRO_WRITE);
	i2c_write(0x23);
	i2c_write(0x02);
	i2c_stop();
}

long int getGyros() //Hämtar hög och låg byte av resultatet.
{
	long int result = 0;

	i2c_start(GYRO_WRITE);
	i2c_write(0xAC);
	i2c_start(GYRO_READ);
	result = i2c_read_ack();
	result |= i2c_read_nack()<<8;
	i2c_stop();
	return result-10647; //returnerar kalibrerad offset.
}

long int getGyros_precise(int p) //Hämtar flera mätningar och returnerar medelvärdet.
{
	long int samples =0;
	for(int i = 0;i<p;i++)
	{
		samples = samples + getGyros();
	}
	return samples/p;
}
