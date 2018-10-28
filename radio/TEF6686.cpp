#include "Arduino.h"
#include "TEF6686.h"

uint16_t readRDSState = 0;
char postStation[9];
uint8_t position = 0;
char rdsData[9];
char radioText[65];
uint16_t rdsChanged = 0;

#define CLEARBUFFER 0
#define READUNTILEOL 1
#define READUNTILBEGIN 2
#define READUNTILEND 3
#define RDS_PS 0
#define RDS_RT 2
#define RDS_NO 0
#define RDS_AVAILABLE 2

TEF6686::TEF6686() {

}

void TEF6686::setup() {  
  if (init() == 1) {
    Tuner_Init();
    devTEF668x_APPL_Set_OperationMode(1);
  }  
}

void TEF6686::powerOn() {
  devTEF668x_APPL_Set_OperationMode(0);
}

void TEF6686::powerOff() {
  devTEF668x_APPL_Set_OperationMode(1);
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

uint16_t TEF6686::tuneUp() {
  return tune(1);
}

uint16_t TEF6686::tuneDown() {
  return tune(0);
}

void TEF6686::setVolume(uint16_t volume) {
  devTEF668x_Audio_Set_Volume(volume);
}

void TEF6686::setMute() {
  devTEF668x_Audio_Set_Mute(1);
}

void TEF6686::setUnMute() {
  devTEF668x_Audio_Set_Mute(0);
}

char TEF6686::readRDSRadioStation(char* rs) {
  strcpy(rs, postStation);
}

char TEF6686::readRDSRadioText(char* rt) { 
  memset(postStation, 0, 9);
  char status = 0;
  switch(readRDSState) {
    case CLEARBUFFER:
      memset(rt, 0, 65);
      memset(radioText, 0, 65);
    
      readRDSState++;
      break;
    case READUNTILEOL:
      if (strchr(rt, 0x0D) == 0) {//When no CR(Carriage Return) is found => read RDS
        readRDS(postStation, rt, &position);
      }
      else {
        memset(rt, 0, 65);
        memset(radioText, 0, 65);
        readRDSState++;
        position = 100;
      }
      break;
    case READUNTILBEGIN:      
      if (position) {
        readRDS(postStation, rt, &position);
      }
      else {
        readRDSState++;
        position = 100;
        memset(rt, 0, 65);
        memset(radioText, 0, 65);
      }
      break;
    case READUNTILEND:
      if (strchr(rt, 0x0D) == 0) {
        readRDS(postStation, rt, &position);
      }
      else {
        if (checkValidText(rt)) {
          status = 1; //Text found
          readRDSState = 0;
        }   
      }
      break;
    default:
      break;
  }
  return status;
}

static inline void considerRDSChar(char *buf, uint16_t place, char ch) { 
  if (ch < 0x10 || ch > 0x7F) {
    if (ch != 0x0D) {
        return;
    }    
  }
  buf[place] = ch;
}

char TEF6686::checkValidText(char* rt) {
  char* endPos = strchr(rt,0x0D);
  char* i = 0;
  char* previousSpacePos = rt;
  for( i = rt; i < endPos; i++) {
    if (*i == 0x20) {
      if (++previousSpacePos < i) {
        previousSpacePos = i;
      }
      else {
        return false;
      }
    }
  }
  return true;
}

uint16_t TEF6686::readRDS(char* ps, char* rt, uint8_t* pos) {
  uint16_t rdsStat, rdsA, rdsB, rdsC, rdsD, rdsErr;
  uint16_t result = devTEF668x_Radio_Get_RDS_Data(1, &rdsStat, &rdsA, &rdsB, &rdsC, &rdsD, &rdsErr);
    
  if (result && rdsB != 0x0 && (rdsStat & 0x8000) != 0x0) {
    const uint8_t groupId = (rdsB & 0xF000) >> 12;
    uint8_t version = rdsB & 0x10;
    
    switch(groupId) {
      case RDS_PS: {
        const uint8_t index = (rdsB & 0x3)*2;
        char Dh = (rdsD & 0xFF00) >> 8;
        char Dl = rdsD;
        
        considerRDSChar(rdsData, index, Dh);
        considerRDSChar(rdsData, index + 1, Dl);
        
        rdsChanged = 1;
      }
        break;
      case RDS_RT: {
        uint8_t index = (rdsB & 0xF)*4;
        *pos = index; 
        char Ch = (rdsC & 0xFF00) >> 8;
        char Cl = rdsC;
        char Dh = (rdsD & 0xFF00) >> 8;
        char Dl = rdsD;
        
        considerRDSChar(radioText, index, Ch);
        considerRDSChar(radioText, index + 1, Cl);
        considerRDSChar(radioText, index + 2, Dh);
        considerRDSChar(radioText, index + 3, Dl);

        rdsChanged = 1;
      }
        break;
    }
  }
  
  const int change = rdsChanged;
  if (change) {
    strcpy(ps, rdsData);
    strcpy(rt, radioText);
  }
  rdsChanged = 0;
  return (change) ? RDS_AVAILABLE : RDS_NO;
}

uint8_t TEF6686::init() {
  uint8_t counter = 0;
  uint8_t status;
  
  delay(5);
  while(true) {
    if (devTEF668x_APPL_Get_Operation_Status(&status) == 1) {
      return 1; //Ok
    }
    else if (++counter > 50) {
      return 2; //Doesn't exist
    }
    else {
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

uint16_t TEF6686::tune(uint8_t up) {
  Radio_ChangeFreqOneStep(up);

  Radio_SetFreq(Radio_PRESETMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
  Radio_ClearCurrentStation();
  return Radio_GetCurrentFreq();
}
