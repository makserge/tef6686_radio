#ifndef TEF6686_h
#define TEF6686_h

#include <Wire.h>

#include "Tuner_Api.h"
#include "Tuner_Drv_Lithio.h"
#include "Tuner_Interface.h"

#define I2C_PORT 2
#define I2C_ADDR 0x64

typedef struct Station {
  char callSign[5];
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
    uint16_t seekUp();      // returns the tuned frequency or 0
    uint16_t seekDown();    // returns the tuned frequency or 0
    uint16_t tuneUp();      // returns the tuned frequency or 0
    uint16_t tuneDown();    // returns the tuned frequency or 0
    void setVolume(uint16_t volume); 	//-60 -- +24 dB volume
    void setMute();
    void setUnMute();
    bool readRDS(void);
    void getRDS(Station * tunedStation);
     
  private:
    uint16_t seek(uint8_t up);
    uint16_t tune(uint8_t up);
    void ptystr(byte);
    void printable_str(char * str, int length);

    char _disp[65];
    char _ps[9];
    char _csign[5];
    bool _ab;
    char _pty[17];
    bool _newRadioText;
    byte _year;
    byte _month;        //Contains the year
    byte _day;          //Contains the day
    byte _hour;         //Contains the hour
    byte _minute;
};

#endif
