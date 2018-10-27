
#ifndef _RADIO_DRV_INIT_H_
#define _RADIO_DRV_INIT_H_

unsigned char Tuner_WriteBuffer(unsigned char *buf,uint16_t len);
unsigned char Tuner_ReadBuffer(unsigned char *buf, uint16_t len);

int Tuner_Init(void);

#endif
