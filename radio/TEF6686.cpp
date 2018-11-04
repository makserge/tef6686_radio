#include "TEF6686.h"

char* ptyLUT[51] = { 
      "      None      ",
      "      News      ",
      "  Information   ",
      "     Sports     ",
      "      Talk      ",
      "      Rock      ",
      "  Classic Rock  ",
      "   Adult Hits   ",
      "   Soft Rock    ",
      "     Top 40     ",
      "    Country     ",
      "     Oldies     ",
      "      Soft      ",
      "   Nostalgia    ",
      "      Jazz      ",
      "   Classical    ",
      "Rhythm and Blues",
      "   Soft R & B   ",
      "Foreign Language",
      "Religious Music ",
      " Religious Talk ",
      "  Personality   ",
      "     Public     ",
      "    College     ",
      " Reserved  -24- ",
      " Reserved  -25- ",
      " Reserved  -26- ",
      " Reserved  -27- ",
      " Reserved  -28- ",
      "     Weather    ",
      " Emergency Test ",
      "  !!!ALERT!!!   ",
      "Current Affairs ",
      "   Education    ",
      "     Drama      ",
      "    Cultures    ",
      "    Science     ",
      " Varied Speech  ",
      " Easy Listening ",
      " Light Classics ",
      "Serious Classics",
      "  Other Music   ",
      "    Finance     ",
      "Children's Progs",
      " Social Affairs ",
      "    Phone In    ",
      "Travel & Touring",
      "Leisure & Hobby ",
      " National Music ",
      "   Folk Music   ",
      "  Documentary   "};

TEF6686::TEF6686() {
}

uint8_t TEF6686::init() {
  uint8_t result;
  uint8_t counter = 0;
  uint8_t status;

  Tuner_I2C_Init();
  
  delay(5);
  while (true) {
    result = devTEF668x_APPL_Get_Operation_Status(&status);
    if (result == 1) {
      Tuner_Init();
      powerOff();
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

uint16_t TEF6686::getLevel() {
  return Radio_Get_Level(1);
}

uint8_t TEF6686::getStereoStatus() {
  return Radio_CheckStereo();
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

uint8_t TEF6686::readRDS() {
  char status;
  uint8_t rdsBHigh, rdsBLow, rdsCHigh, rdsCLow, rdsDHigh, isReady, rdsDLow;

  uint16_t rdsStat, rdsA, rdsB, rdsC, rdsD, rdsErr;
  uint16_t result = devTEF668x_Radio_Get_RDS_Data(1, &rdsStat, &rdsA, &rdsB, &rdsC, &rdsD, &rdsErr);

  rdsBHigh = (uint8_t)(rdsB >> 8);
  rdsBLow = (uint8_t)rdsB;
  rdsCHigh = (uint8_t)(rdsC >> 8);
  rdsCLow = (uint8_t)rdsC;
  rdsDHigh = (uint8_t)(rdsD >> 8);
  rdsDLow = (uint8_t)rdsD;
 
  uint8_t programType = ((rdsBHigh & 3) << 3) | ((rdsBLow >> 5) & 7);
  strcpy(rdsProgramType, (programType >= 0 && programType < 32) ? ptyLUT[programType] : "    PTY ERROR   ");

  uint8_t type = (rdsBHigh >> 4) & 15;
  uint8_t version = bitRead(rdsBHigh, 4);
  
  // Groups 0A & 0B
  // Basic tuning and switching information only
  if (type == 0) {
    uint8_t address = rdsBLow & 3;
    // Groups 0A & 0B: to extract PS segment we need blocks 1 and 3
    if (address >= 0 && address <= 3) {
      if (rdsDHigh != '\0') {
        rdsProgramService[address * 2] = rdsDHigh;
      }  
      if (rdsDLow != '\0') {
        rdsProgramService[address * 2 + 1] = rdsDLow;
      }  
      isReady = (address == 3) ? 1 : 0;
    } 
    rdsFormatString(rdsProgramService, 8);
  }
  // Groups 2A & 2B
  // Radio Text
  else if (type == 2) {
    uint16_t addressRT = rdsBLow & 15;
    uint8_t ab = bitRead(rdsBLow, 4);
    uint8_t cr = 0;
    uint8_t len = 64;
    if (version == 0) {
      if (addressRT >= 0 && addressRT <= 15) {
        if (rdsCHigh != 0x0D) {
          rdsRadioText[addressRT*4] = rdsCHigh;
        }  
        else {
          len = addressRT * 4;
          cr = 1;
        }
        if (rdsCLow != 0x0D) {
          rdsRadioText[addressRT * 4 + 1] = rdsCLow;
        }  
        else {
          len = addressRT * 4 + 1;
          cr = 1;
        }
        if (rdsDHigh != 0x0D) {
          rdsRadioText[addressRT * 4 + 2] = rdsDHigh;
        }  
        else {
          len = addressRT * 4 + 2;
          cr = 1;
        }
        if (rdsDLow != 0x0D) {
          rdsRadioText[addressRT * 4 + 3] = rdsDLow;
        }
        else {
          len = addressRT * 4 + 3;
          cr = 1;
        }
      }
    }
    else {
      if (addressRT >= 0 && addressRT <= 7) {
        if (rdsDHigh != '\0') {
          rdsRadioText[addressRT * 2] = rdsDHigh;
        }  
        if (rdsDLow != '\0') {
          rdsRadioText[addressRT * 2 + 1] = rdsDLow;
        }
      }
    }
    if (cr) {
      for (uint8_t i = len; i < 64; i++) {
        rdsRadioText[i] = ' ';
      }
    }
    if (ab != rdsAb) {      
      for (uint8_t i = 0; i < 64; i++) {
        rdsRadioText[i] = ' ';
      }
      rdsRadioText[64] = '\0';     
      isRdsNewRadioText = 1;
    }
    else {
      isRdsNewRadioText = 0;
    }
    rdsAb = ab;
    rdsFormatString(rdsRadioText, 64);
  }
  delay(5);
  return isReady; 
}
 
void TEF6686::getRDS(RdsInfo* rdsInfo) {
  strcpy(rdsInfo->programType, rdsProgramType);
  strcpy(rdsInfo->programService, rdsProgramService);
  strcpy(rdsInfo->radioText, rdsRadioText); 
}

void TEF6686::rdsFormatString(char* str, uint16_t length) {  
  for (uint16_t i = 0; i < length; i++) {    
    if ((str[i] != 0 && str[i] < 32) || str[i] > 126 ) {
      str[i] = ' ';  
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
