#ifndef TEF6686_h
#define TEF6686_h

#include <Arduino.h>

#include <Wire.h>
#include <stdint.h>

#include "Tuner_Api.h"
#include "Tuner_Proc.h"
#include "Tuner_Api_Lithio.h"
#include "Tuner_Drv_Lithio.h"
#include "Tuner_Interface.h"

#define I2C_ADDR 0x64

class TEF6686
{
  public:
    TEF6686();
    void powerOn();					// call in setup
	  void setChannel(uint16_t frequency);    // frequency as int, i.e. 100.00 as 10000
  
	 /*
   
	uint16_t getChannel();				// returns the tuned channel or 0
	uint16_t seekUp(); 					// returns the tuned channel or 0
	uint16_t seekDown(); 				
	void setVolume(uint16_t volume); 	// 0 to 15
//	void readRDS(char* message, long timeout);
	char readRDSRadioText(char* rt);
	uint16_t readRDS(char* ps, char* rt);
									// message should be at least 9 chars
									// result will be null terminated
									// timeout in milliseconds
	char CheckValidText(char* rt);
#define RDS_NO (0)
#define RDS_FAKE (1)
#define RDS_AVAILABLE (2)
	*/

  protected:
  /*
  // RDS specific stuff
#define RDS_PS (0)
#define RDS_RT (2)
uint16_t rdschanged = 0;
uint16_t fakerds = 0;
uint8_t previousversion =0;
// rds buffering
char rdsdata[9];
char radiotext[65];
    uint16_t _resetPin;
	uint16_t _sdioPin;
	uint16_t _sclkPin;
	void si4703_init();
	void readRegisters();
	byte updateRegisters();
	uint16_t seek(byte seekDirection);
//	uint16_t getChannel();
	uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large
	static const uint16_t  FAIL = 0;
	static const uint16_t  SUCCESS = 1;

	static const uint16_t SI4703 = 0x10; //0b._001.0000 = I2C address of Si4703 - note that the Wire function assumes non-left-shifted I2C address, not 0b.0010.000W
	static const uint16_t  I2C_FAIL_MAX = 10; //This is the number of attempts we will try to contact the device before erroring out
	static const uint16_t  SEEK_DOWN = 0; //Direction used for seeking. Default is down
	static const uint16_t  SEEK_UP = 1;

	//Define the register names
	static const uint16_t  DEVICEID = 0x00;
	static const uint16_t  CHIPID = 0x01;
	static const uint16_t  POWERCFG = 0x02;
	static const uint16_t  CHANNEL = 0x03;
	static const uint16_t  SYSCONFIG1 = 0x04;
	static const uint16_t  SYSCONFIG2 = 0x05;
	static const uint16_t  STATUSRSSI = 0x0A;
	static const uint16_t  READCHAN = 0x0B;
	static const uint16_t  RDSA = 0x0C;
	static const uint16_t  RDSB = 0x0D;
	static const uint16_t  RDSC = 0x0E;
	static const uint16_t  RDSD = 0x0F;

	//Register 0x02 - POWERCFG
	static const uint16_t  SMUTE = 15;
	static const uint16_t  DMUTE = 14;
	static const uint16_t  SKMODE = 10;
	static const uint16_t  SEEKUP = 9;
	static const uint16_t  SEEK = 8;

	//Register 0x03 - CHANNEL
	static const uint16_t  TUNE = 15;

	//Register 0x04 - SYSCONFIG1
	static const uint16_t  RDS = 12;
	static const uint16_t  DE = 11;

	//Register 0x05 - SYSCONFIG2
	static const uint16_t  SPACE1 = 5;
	static const uint16_t  SPACE0 = 4;

	//Register 0x0A - STATUSRSSI
	static const uint16_t  RDSR = 15;
	static const uint16_t  STC = 14;
	static const uint16_t  SFBL = 13;
	static const uint16_t  AFCRL = 12;
	static const uint16_t  RDSS = 11;
	static const uint16_t  STEREO = 8;
	*/
  private:
    uint8_t init();
};

#endif
