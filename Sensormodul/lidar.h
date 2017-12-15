#ifndef LIDAR_H_
#define LIDAR_H_
#define trigger PORTD6
#define monitor PIND7
unsigned int distance_lidar();
unsigned int distance_lidar_precise(unsigned int precision);

#endif /* LIDAR_H_ */
