
#ifndef _RADIO_DRV_INIT_H_
#define _RADIO_DRV_INIT_H_

void Tuner_I2C_Init(void);
uint16_t Tuner_Init(void);
unsigned char Tuner_WriteBuffer(unsigned char *buf,uint16_t len);
unsigned char Tuner_ReadBuffer(unsigned char *buf, uint16_t len);

#endif
