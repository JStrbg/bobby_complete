#ifndef GYRO_H_
#define GYRO_H_
#define GYRO_WRITE 0xD6
#define GYRO_READ  0xD7
void gyros_init();
long int getGyros();
long int getGyros_precise(int p);


#endif /* GYRO_H_ */