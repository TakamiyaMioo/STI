#ifndef _AD9833_H
#define _AD9833_H

#include "gpio.h"
#define TRI_WAVE 	0  		//输出三角波
#define SIN_WAVE 	1		//输出正弦波
#define SQU_WAVE 	2		//输出方波


//void AD9833_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase );

//void AD9833_Init_GPIO(void);
//void AD9833_AmpSet(unsigned char amp);

void AD9833_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase, unsigned char channel );
void AD9833_WaveSeting_Double(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase);


#endif



