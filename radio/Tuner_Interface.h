
#ifndef _RADIO_DRV_INIT_H_
#define _RADIO_DRV_INIT_H_

unsigned char Tuner_WriteBuffer(unsigned char *buf,unsigned char len);
unsigned char Tuner_ReadBuffer(unsigned char *buf,unsigned char len);

int Tuner_Init(void);

#endif

