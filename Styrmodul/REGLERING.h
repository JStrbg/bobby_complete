#ifndef REGLERING_H_
#define REGLERING_H_

volatile unsigned int K;
volatile unsigned int KD;
volatile unsigned int KR;
volatile unsigned char programState;

void Pwm_Gen( char Right_Pwm,  char Left_Pwm,  char Right_dir ,  char Left_dir );
int mult_kd(int tot_e);
int mult_kr(int tot_e);
int mult_k(int tot_e);
void Regler_Func(int dir);
void Get_Sensor(unsigned char* sens);
void pwm_init();

#endif /* REGLERING_H_ */