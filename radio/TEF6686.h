#ifndef TEF6686_h
#define TEF6686_h

#include <Arduino.h>

#include <Wire.h>
#include <stdint.h>

#include "Tuner_Api.h"
#include "Tuner_Drv_Lithio.h"
#include "Tuner_Interface.h"

#define I2C_ADDR 0x64

class TEF6686
{
  public:
    TEF6686();
    uint8_t init();
    void powerOn();					// call in setup
    void powerOff();				
    void setFrequency(uint16_t frequency);    // frequency as int, i.e. 100.00 as 10000
    uint16_t getFrequency(); // returns the current frequency
    uint16_t seekUp();      // returns the tuned frequency or 0
    uint16_t seekDown();    // returns the tuned frequency or 0
    uint16_t tuneUp();      // returns the tuned frequency or 0
    uint16_t tuneDown();    // returns the tuned frequency or 0
    void setVolume(uint16_t volume); 	//-60 -- +24 dB volume
    void setMute();
    void setUnMute();
    uint16_t readRDS(char* ps, char* rt,uint8_t* pos);
    char readRDSRadioText(char* rt);
    char readRDSRadioStation(char* rs);
     
  private:
    uint16_t seek(uint8_t up);
    uint16_t tune(uint8_t up);
    char checkValidText(char* rt);
};

#endif
