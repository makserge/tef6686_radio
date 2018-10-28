#include "Arduino.h"
#include "TEF6686.h"

TEF6686::TEF6686() {
  if (init() == 1) {
    Tuner_Init();
  }  
}

void TEF6686::powerOn() {
  Tuner_Update_ProgCnt(Radio_PRESETMODE);
}

void TEF6686::setFrequency(uint16_t frequency) {
    Radio_SetFreq(Radio_PRESETMODE, FM1_BAND, frequency);
}

uint16_t TEF6686::getFrequency() {
    return Radio_GetCurrentFreq();
}

uint16_t TEF6686::seekUp() {
	return seek(1);
}

uint16_t TEF6686::seekDown() {
	return seek(0);
}

void TEF6686::setVolume(uint16_t volume) {
  AUDIO_Set_Volume(volume);
}

/*
static inline void considerrdschar(char *buf, uint16_t place, char ch)
{	
		if (ch < 0x10 || ch > 0x7F)
		{
			if(ch != 0x0D)
				return;
		}
		buf[place] = ch;
}



char TEF6686::CheckValidText(char* rt)
{
	char* endpos = strchr(rt,0x0D);
	char* i = 0;
	char* previousSpacePos = rt;
	for(i=rt;i<endpos;i++)
	{
		if(*i == 0x20)
		{
			if(++previousSpacePos < i)
			{
				previousSpacePos = i;
			}
			else{
				return false;
			}
		}			
	}
	return true;
}

uint16_t TEF6686::readRDS(char* ps, char* rt)
{ 
	readRegisters();
	if(si4703_registers[STATUSRSSI] & (1<<RDSR)) {
		if (fakerds) {
			memset(rdsdata, 0, 8);
			rdschanged = 1;
		}
		
		fakerds = 0;
		
		//const uint16_t a = si4703_registers[RDSA];
		const uint16_t b = si4703_registers[RDSB];
		const uint16_t c = si4703_registers[RDSC];
		const uint16_t d = si4703_registers[RDSD];
		
		const uint8_t groupid = (b & 0xF000) >> 12;
		uint8_t version = b & 0x10;
		
		switch(groupid) {
			case RDS_PS: {
				const uint8_t index = (b & 0x3)*2;
				char Dh = (d & 0xFF00) >> 8;
				char Dl = d;
				
				considerrdschar(rdsdata, index, Dh);
				considerrdschar(rdsdata, index +1, Dl);
				
				rdschanged = 1;
			};
				break;
			case RDS_RT: {
				rdschanged = 1;
				uint8_t index = (b & 0xF)*4;
				char Ch = (c & 0xFF00) >> 8;
				char Cl = c;
				char Dh = (d & 0xFF00) >> 8;
				char Dl = d;
				
				considerrdschar(radiotext, index, Ch);
				considerrdschar(radiotext, index +1, Cl);
				considerrdschar(radiotext, index +2, Dh);
				considerrdschar(radiotext, index +3, Dl);
			};
				break;
		}
	}
	
	const uint16_t change = rdschanged;
	if (change) {
		strcpy(ps, rdsdata);
		strcpy(rt, radiotext);
	}
	rdschanged = 0;
	return (change) ? ((fakerds) ? (RDS_FAKE) : (RDS_AVAILABLE)) : (RDS_NO);
}

/*
//Reads the current channel from READCHAN
//Returns a number like 973 for 97.3MHz
uint16_t TEF6686::getChannel() {
  readRegisters();
  uint16_t channelr = si4703_registers[READCHAN] & 0x03FF; //Mask out everything but the lower 10 bits
  //Freq(MHz) = 0.100(in Europe) * Channel + 87.5MHz
  //X = 0.1 * Chan + 87.5
// use this for US (0.200) channel spacing
	channelr *= 2;
  channelr += 875; //98 + 875 = 973
  return(channelr);
}
*/

uint8_t TEF6686::init() {
  uint8_t counter = 0;
  TUNER_STATE status;
  
  delay(5);
  while(true)
  {
    if (APPL_Get_Operation_Status(&status) == 1)
    {
      return 1; //Ok
    }
    else if (++counter > 50){
      return 2; //Doesn't exist
    }
    else
    {
      delay(5);
      return 0;  //Busy
    }
  }
}

uint16_t TEF6686::seek(uint8_t up) {
  uint16_t mode = 20;
  uint16_t startFrequency = Radio_GetCurrentFreq();

  while (true) {
    switch(mode){
      case 20:
        Radio_ChangeFreqOneStep(up);
        Radio_SetFreq(Radio_SEARCHMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
      
        mode = 30;
        Radio_CheckStationInit();
        Radio_ClearCurrentStation();
        
        break;
      
      case 30:
        delay(20);
        Radio_CheckStation();
        if (Radio_CheckStationStatus() >= NO_STATION) {
          mode = 40;
        }   
        
        break;

      case 40:
        if (Radio_CheckStationStatus() == NO_STATION) {        
          mode = (startFrequency == Radio_GetCurrentFreq()) ? 50 : 20;
        }
        else if (Radio_CheckStationStatus() == PRESENT_STATION) {
          mode = 50;
        }
        
        break;
      
      case 50:
        Radio_SetFreq(Radio_PRESETMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
        return Radio_GetCurrentFreq();
    }
  }
  return 0;
}
