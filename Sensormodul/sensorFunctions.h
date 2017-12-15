
unsigned char styr_in[2];
unsigned char sensor[14]; //kort h�gerfram, kort h�gerbak, v�nsterkort, bak, h�ger l�ng, v�nster l�ng, framl�g, framH�G, hastighet, program styr, felmeddelande styr, program sens, fel sens , klarbyte rotate
unsigned char kortsens[255];
unsigned char longsens[255];

void Get_Sensor_Value();
void sens_to_centi();
void centimeter_values();
