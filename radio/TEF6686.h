#ifndef TEF6686_h
#define TEF6686_h

#include <Wire.h>

#include "Tuner_Api.h"
#include "Tuner_Drv_Lithio.h"
#include "Tuner_Interface.h"

#define I2C_PORT 2
#define I2C_ADDR 0x64

struct RdsInfo {
  char programType[17]; 
  char programService[9];
  char radioText[65];
  bool newRadioText;
};

class TEF6686 {
  public:
    TEF6686();
    uint8_t init();
    void powerOn();					// call in setup
    void powerOff();				
    void setFrequency(uint16_t frequency);    // frequency as int, i.e. 100.00 as 10000
    uint16_t getFrequency(); // returns the current frequency
    uint16_t getLevel();
    uint8_t getStereoStatus();
    uint16_t seekUp();      // returns the tuned frequency or 0
    uint16_t seekDown();    // returns the tuned frequency or 0
	uint16_t seekSync(uint8_t up); // returns 1 if seek was completed and 0 if seek in progress
    uint16_t tuneUp();      // returns the tuned frequency or 0
    uint16_t tuneDown();    // returns the tuned frequency or 0
    void setVolume(uint16_t volume); 	//-60 -- +24 dB volume
    void setMute();
    void setUnMute();
    uint8_t readRDS();
    void getRDS(RdsInfo* rdsInfo);
     
  private:
    uint16_t seek(uint8_t up);
    uint16_t tune(uint8_t up);
    void rdsFormatString(char* str, uint16_t length);

    char rdsRadioText[65];
    char rdsProgramService[9];
    uint8_t rdsAb;
    char rdsProgramType[17];
    uint8_t isRdsNewRadioText;
	
	uint16_t seekMode;
	uint16_t seekStartFrequency;
};

#endif
