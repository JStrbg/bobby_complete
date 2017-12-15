#ifndef F_CPU
#define F_CPU 14745600
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "sensorFunctions.h"

void Get_Sensor_Value() //h�mtar sensorv�rden
{
//cli();
	for(int i = 0; i < 6; i++ ) // 6 st sensorer 
	{
		ADMUX = 0b11100000 + i;
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		sensor[i] = ADCH;
		if(i == 1 && sensor[i] < 245)
		{
			sensor[i] = sensor[i] + 10;
		}
	}
//sei();
	
}
void sens_to_centi()// g�r om sensor v�rden till centimeter
{

	for(int i=0;i<6; i++)
	{
		if(i<3)
		{
			sensor[i] = kortsens[(int)sensor[i]];
		}
		else
		{
			sensor[i] = longsens[(int)sensor[i]];
		}
	}
}

void centimeter_values() //fyller tabeller f�r 5-30 cm och 10-80 cm sensor
{
longsens[	255	]=	10	;
longsens[	254	]=	10	;
longsens[	253	]=	10	;
longsens[	252	]=	10	;
longsens[	251	]=	10	;
longsens[	250	]=	10	;
longsens[	249	]=	10	;
longsens[	248	]=	10	;
longsens[	247	]=	10	;
longsens[	246	]=	10	;
longsens[	245	]=	10	;
longsens[	244	]=	10	;
longsens[	243	]=	10	;
longsens[	242	]=	10	;
longsens[	241	]=	10	;
longsens[	240	]=	10	;
longsens[	239	]=	10	;
longsens[	238	]=	10	;
longsens[	237	]=	10	;
longsens[	236	]=	10	;
longsens[	235	]=	10	;
longsens[	234	]=	10	;
longsens[	233	]=	10	;
longsens[	232	]=	10	;
longsens[	231	]=	10	;
longsens[	230	]=	10	;
longsens[	229	]=	10	;
longsens[	228	]=	10	;
longsens[	227	]=	10	;
longsens[	226	]=	10	;
longsens[	225	]=	10	;
longsens[	224	]=	10	;
longsens[	223	]=	10	;
longsens[	222	]=	10	;
longsens[	221	]=	10	;
longsens[	220	]=	10	;
longsens[	219	]=	10	;
longsens[	218	]=	10	;
longsens[	217	]=	10	;
longsens[	216	]=	10	;
longsens[	215	]=	10	;
longsens[	214	]=	10	;
longsens[	213	]=	10	;
longsens[	212	]=	10	;
longsens[	211	]=	10	;
longsens[	210	]=	10	;
longsens[	209	]=	10	;
longsens[	208	]=	10	;
longsens[	207	]=	10	;
longsens[	206	]=	10	;
longsens[	205	]=	10	;
longsens[	204	]=	10	;
longsens[	203	]=	10	;
longsens[	202	]=	10	;
longsens[	201	]=	10	;
longsens[	200	]=	10	;
longsens[	199	]=	10	;
longsens[	198	]=	10	;
longsens[	197	]=	10	;
longsens[	196	]=	10	;
longsens[	195	]=	10	;
longsens[	194	]=	10	;
longsens[	193	]=	10	;
longsens[	192	]=	10	;
longsens[	191	]=	10	;
longsens[	190	]=	10	;
longsens[	189	]=	10	;
longsens[	188	]=	10	;
longsens[	187	]=	10	;
longsens[	186	]=	10	;
longsens[	185	]=	10	;
longsens[	184	]=	10	;
longsens[	183	]=	10	;
longsens[	182	]=	10	;
longsens[	181	]=	10	;
longsens[	180	]=	10	;
longsens[	179	]=	10	;
longsens[	178	]=	10	;
longsens[	177	]=	10	;
longsens[	176	]=	10	;
longsens[	175	]=	10	;
longsens[	174	]=	10	;
longsens[	173	]=	10	;
longsens[	172	]=	10	;
longsens[	171	]=	10	;
longsens[	170	]=	10	;
longsens[	169	]=	10	;
longsens[	168	]=	10	;
longsens[	167	]=	10	;
longsens[	166	]=	11	;
longsens[	165	]=	11	;
longsens[	164	]=	11	;
longsens[	163	]=	11	;
longsens[	162	]=	11	;
longsens[	161	]=	11	;
longsens[	160	]=	11	;
longsens[	159	]=	12	;
longsens[	158	]=	12	;
longsens[	157	]=	12	;
longsens[	156	]=	12	;
longsens[	155	]=	12	;
longsens[	154	]=	12	;
longsens[	153	]=	12	;
longsens[	152	]=	12	;
longsens[	151	]=	12	;
longsens[	150	]=	13	;
longsens[	149	]=	13	;
longsens[	148	]=	13	;
longsens[	147	]=	13	;
longsens[	146	]=	13	;
longsens[	145	]=	13	;
longsens[	144	]=	14	;
longsens[	143	]=	14	;
longsens[	142	]=	14	;
longsens[	141	]=	14	;
longsens[	140	]=	14	;
longsens[	139	]=	14	;
longsens[	138	]=	14	;
longsens[	137	]=	12	;
longsens[	136	]=	12	;
longsens[	135	]=	15	;
longsens[	134	]=	15	;
longsens[	133	]=	15	;
longsens[	132	]=	15	;
longsens[	131	]=	16	;
longsens[	130	]=	16	;
longsens[	129	]=	16	;
longsens[	128	]=	16	;
longsens[	127	]=	16	;
longsens[	126	]=	17	;
longsens[	125	]=	17	;
longsens[	124	]=	17	;
longsens[	123	]=	17	;
longsens[	122	]=	18	;
longsens[	121	]=	18	;
longsens[	120	]=	18	;
longsens[	119	]=	19	;
longsens[	118	]=	19	;
longsens[	117	]=	19	;
longsens[	116	]=	19	;
longsens[	115	]=	20	;
longsens[	114	]=	20	;
longsens[	113	]=	20	;
longsens[	112	]=	20	;
longsens[	111	]=	20	;
longsens[	110	]=	21	;
longsens[	109	]=	21	;
longsens[	108	]=	21	;
longsens[	107	]=	21	;
longsens[	106	]=	22	;
longsens[	105	]=	22	;
longsens[	104	]=	22	;
longsens[	103	]=	22	;
longsens[	102	]=	23	;
longsens[	101	]=	23	;
longsens[	100	]=	24	;
longsens[	99	]=	24	;
longsens[	98	]=	25	;
longsens[	97	]=	25	;
longsens[	96	]=	25	;
longsens[	95	]=	26	;
longsens[	94	]=	26	;
longsens[	93	]=	27	;
longsens[	92	]=	28	;
longsens[	91	]=	28	;
longsens[	90	]=	29	;
longsens[	89	]=	29	;
longsens[	88	]=	30	;
longsens[	87	]=	30	;
longsens[	86	]=	31	;
longsens[	85	]=	31	;
longsens[	84	]=	32	;
longsens[	83	]=	32	;
longsens[	82	]=	33	;
longsens[	81	]=	33	;
longsens[	80	]=	34	;
longsens[	79	]=	34	;
longsens[	78	]=	35	;
longsens[	77	]=	36	;
longsens[	76	]=	36	;
longsens[	75	]=	37	;
longsens[	74	]=	37	;
longsens[	73	]=	38	;
longsens[	72	]=	39	;
longsens[	71	]=	40	;
longsens[	70	]=	40	;
longsens[	69	]=	40	;
longsens[	68	]=	45	;
longsens[	67	]=	45	;
longsens[	66	]=	45	;
longsens[	65	]=	45	;
longsens[	64	]=	45	;
longsens[	63	]=	45	;
longsens[	62	]=	45	;
longsens[	61	]=	55	;
longsens[	60	]=	55	;
longsens[	59	]=	55	;
longsens[	58	]=	55	;
longsens[	57	]=	55	;
longsens[	56	]=	65	;
longsens[	55	]=	65	;
longsens[	54	]=	65	;
longsens[	53	]=	70	;
longsens[	52	]=	70	;
longsens[	51	]=	75	;
longsens[	50	]=	75	;
longsens[	49	]=	80	;
longsens[	48	]=	80	;
longsens[	47	]=	81	;
longsens[	46	]=	81	;
longsens[	45	]=	81	;
longsens[	44	]=	81	;
longsens[	43	]=	81	;
longsens[	42	]=	81	;
longsens[	41	]=	81	;
longsens[	40	]=	81	;
longsens[	39	]=	81	;
longsens[	38	]=	81	;
longsens[	37	]=	81	;
longsens[	36	]=	81	;
longsens[	35	]=	81	;
longsens[	34	]=	81	;
longsens[	33	]=	81	;
longsens[	32	]=	81	;
longsens[	31	]=	81	;
longsens[	30	]=	81	;
longsens[	29	]=	81	;
longsens[	28	]=	81	;
longsens[	27	]=	81	;
longsens[	26	]=	81	;
longsens[	25	]=	81	;
longsens[	24	]=	81	;
longsens[	23	]=	81	;
longsens[	22	]=	81	;
longsens[	21	]=	81	;
longsens[	20	]=	81	;
longsens[	19	]=	81	;
longsens[	18	]=	81	;
longsens[	17	]=	81	;
longsens[	16	]=	81	;
longsens[	15	]=	81	;
longsens[	14	]=	81	;
longsens[	13	]=	81	;
longsens[	12	]=	81	;
longsens[	11	]=	81	;
longsens[	10	]=	81	;
longsens[	9	]=	81	;
longsens[	8	]=	81	;
longsens[	7	]=	81	;
longsens[	6	]=	81	;
longsens[	5	]=	81	;
longsens[	4	]=	81	;
longsens[	3	]=	81	;
longsens[	2	]=	81	;
longsens[	1	]=	81	;
longsens[	0	]=	81	;

kortsens[	255	]=	0	;
kortsens[	254	]=	0	;
kortsens[	253	]=	0	;
kortsens[	252	]=	0	;
kortsens[	251	]=	0	;
kortsens[	250	]=	0	;
kortsens[	249	]=	0	;
kortsens[	248	]=	0	;
kortsens[	247	]=	0	;
kortsens[	246	]=	0	;
kortsens[	245	]=	0	;
kortsens[	244	]=	0	;
kortsens[	243	]=	0	;
kortsens[	242	]=	1	;
kortsens[	241	]=	1	;
kortsens[	240	]=	1	;
kortsens[	239	]=	1	;
kortsens[	238	]=	1	;
kortsens[	237	]=	1	;
kortsens[	236	]=	1	;
kortsens[	235	]=	1	;
kortsens[	234	]=	1	;
kortsens[	233	]=	1	;
kortsens[	232	]=	1	;
kortsens[	231	]=	1	;
kortsens[	230	]=	1	;
kortsens[	229	]=	1	;
kortsens[	228	]=	1	;
kortsens[	227	]=	1	;
kortsens[	226	]=	1	;
kortsens[	225	]=	1	;
kortsens[	224	]=	1	;
kortsens[	223	]=	1	;
kortsens[	222	]=	1	;
kortsens[	221	]=	1	;
kortsens[	220	]=	1	;
kortsens[	219	]=	1	;
kortsens[	218	]=	1	;
kortsens[	217	]=	1	;
kortsens[	216	]=	1	;
kortsens[	215	]=	2	;
kortsens[	214	]=	2	;
kortsens[	213	]=	2	;
kortsens[	212	]=	2	;
kortsens[	211	]=	2	;
kortsens[	210	]=	2	;
kortsens[	209	]=	2	;
kortsens[	208	]=	2	;
kortsens[	207	]=	2	;
kortsens[	206	]=	2	;
kortsens[	205	]=	2	;
kortsens[	204	]=	2	;
kortsens[	203	]=	2	;
kortsens[	202	]=	2	;
kortsens[	201	]=	2	;
kortsens[	200	]=	2	;
kortsens[	199	]=	2	;
kortsens[	198	]=	2	;
kortsens[	197	]=	2	;
kortsens[	196	]=	2	;
kortsens[	195	]=	2	;
kortsens[	194	]=	2	;
kortsens[	193	]=	2	;
kortsens[	192	]=	2	;
kortsens[	191	]=	2	;
kortsens[	190	]=	2	;
kortsens[	189	]=	2	;
kortsens[	188	]=	3	;
kortsens[	187	]=	3	;
kortsens[	186	]=	3	;
kortsens[	185	]=	3	;
kortsens[	184	]=	3	;
kortsens[	183	]=	3	;
kortsens[	182	]=	3	;
kortsens[	181	]=	3	;
kortsens[	180	]=	3	;
kortsens[	179	]=	3	;
kortsens[	178	]=	3	;
kortsens[	177	]=	3	;
kortsens[	176	]=	3	;
kortsens[	175	]=	3	;
kortsens[	174	]=	3	;
kortsens[	173	]=	3	;
kortsens[	172	]=	3	;
kortsens[	171	]=	3	;
kortsens[	170	]=	3	;
kortsens[	169	]=	3	;
kortsens[	168	]=	3	;
kortsens[	167	]=	3	;
kortsens[	166	]=	4	;
kortsens[	165	]=	4	;
kortsens[	164	]=	4	;
kortsens[	163	]=	4	;
kortsens[	162	]=	4	;
kortsens[	161	]=	4	;
kortsens[	160	]=	4	;
kortsens[	159	]=	4	;
kortsens[	158	]=	4	;
kortsens[	157	]=	4	;
kortsens[	156	]=	4	;
kortsens[	155	]=	4	;
kortsens[	154	]=	4	;
kortsens[	153	]=	4	;
kortsens[	152	]=	4	;
kortsens[	151	]=	5	;
kortsens[	150	]=	5	;
kortsens[	149	]=	5	;
kortsens[	148	]=	5	;
kortsens[	147	]=	5	;
kortsens[	146	]=	5	;
kortsens[	145	]=	5	;
kortsens[	144	]=	5	;
kortsens[	143	]=	5	;
kortsens[	142	]=	5	;
kortsens[	141	]=	5	;
kortsens[	140	]=	5	;
kortsens[	139	]=	5	;
kortsens[	138	]=	6	;
kortsens[	137	]=	6	;
kortsens[	136	]=	6	;
kortsens[	135	]=	6	;
kortsens[	134	]=	6	;
kortsens[	133	]=	6	;
kortsens[	132	]=	6	;
kortsens[	131	]=	6	;
kortsens[	130	]=	6	;
kortsens[	129	]=	6	;
kortsens[	128	]=	6	;
kortsens[	127	]=	6	;
kortsens[	126	]=	7	;
kortsens[	125	]=	7	;
kortsens[	124	]=	7	;
kortsens[	123	]=	7	;
kortsens[	122	]=	7	;
kortsens[	121	]=	7	;
kortsens[	120	]=	7	;
kortsens[	119	]=	7	;
kortsens[	118	]=	7	;
kortsens[	117	]=	8	;
kortsens[	116	]=	8	;
kortsens[	115	]=	8	;
kortsens[	114	]=	8	;
kortsens[	113	]=	8	;
kortsens[	112	]=	9	;
kortsens[	111	]=	9	;
kortsens[	110	]=	9	;
kortsens[	109	]=	9	;
kortsens[	108	]=	9	;
kortsens[	107	]=	9	;
kortsens[	106	]=	9	;
kortsens[	105	]=	10	;
kortsens[	104	]=	10	;
kortsens[	103	]=	10	;
kortsens[	102	]=	10	;
kortsens[	101	]=	11	;
kortsens[	100	]=	11	;
kortsens[	99	]=	11	;
kortsens[	98	]=	11	;
kortsens[	97	]=	11	;
kortsens[	96	]=	11	;
kortsens[	95	]=	12	;
kortsens[	94	]=	12	;
kortsens[	93	]=	12	;
kortsens[	92	]=	12	;
kortsens[	91	]=	13	;
kortsens[	90	]=	13	;
kortsens[	89	]=	13	;
kortsens[	88	]=	13	;
kortsens[	87	]=	13	;
kortsens[	86	]=	13	;
kortsens[	85	]=	14	;
kortsens[	84	]=	14	;
kortsens[	83	]=	14	;
kortsens[	82	]=	15	;
kortsens[	81	]=	15	;
kortsens[	80	]=	15	;
kortsens[	79	]=	16	;
kortsens[	78	]=	16	;
kortsens[	77	]=	16	;
kortsens[	76	]=	17	;
kortsens[	75	]=	17	;
kortsens[	74	]=	17	;
kortsens[	73	]=	18	;
kortsens[	72	]=	18	;
kortsens[	71	]=	18	;
kortsens[	70	]=	19	;
kortsens[	69	]=	19	;
kortsens[	68	]=	20	;
kortsens[	67	]=	20	;
kortsens[	66	]=	21	;
kortsens[	65	]=	21	;
kortsens[	64	]=	23	;
kortsens[	63	]=	23	;
kortsens[	62	]=	24	;
kortsens[	61	]=	24	;
kortsens[	60	]=	25	;
kortsens[	59	]=	25	;
kortsens[	58	]=	26	;
kortsens[	57	]=	26	;
kortsens[	56	]=	27	;
kortsens[	55	]=	27	;
kortsens[	54	]=	28	;
kortsens[	53	]=	29	;
kortsens[	52	]=	30	;
kortsens[	51	]=	31	;
kortsens[	50	]=	31	;
kortsens[	49	]=	31	;
kortsens[	48	]=	31	;
kortsens[	47	]=	31	;
kortsens[	46	]=	31	;
kortsens[	45	]=	31	;
kortsens[	44	]=	31	;
kortsens[	43	]=	31	;
kortsens[	42	]=	31	;
kortsens[	41	]=	31	;
kortsens[	40	]=	31	;
kortsens[	39	]=	31	;
kortsens[	38	]=	31	;
kortsens[	37	]=	31	;
kortsens[	36	]=	31	;
kortsens[	35	]=	31	;
kortsens[	34	]=	31	;
kortsens[	33	]=	31	;
kortsens[	32	]=	31	;
kortsens[	31	]=	31	;
kortsens[	30	]=	31	;
kortsens[	29	]=	31	;
kortsens[	28	]=	31	;
kortsens[	27	]=	31	;
kortsens[	26	]=	31	;
kortsens[	25	]=	31	;
kortsens[	24	]=	31	;
kortsens[	23	]=	31	;
kortsens[	22	]=	31	;
kortsens[	21	]=	31	;
kortsens[	20	]=	31	;
kortsens[	19	]=	31	;
kortsens[	18	]=	31	;
kortsens[	17	]=	31	;
kortsens[	16	]=	31	;
kortsens[	15	]=	31	;
kortsens[	14	]=	31	;
kortsens[	13	]=	31	;
kortsens[	12	]=	31	;
kortsens[	11	]=	31	;
kortsens[	10	]=	31	;
kortsens[	9	]=	31	;
kortsens[	8	]=	31	;
kortsens[	7	]=	31	;
kortsens[	6	]=	31	;
kortsens[	5	]=	31	;
kortsens[	4	]=	31	;
kortsens[	3	]=	31	;
kortsens[	2	]=	31	;
kortsens[	1	]=	31	;
kortsens[	0	]=	31	;
}
