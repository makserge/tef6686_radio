#include "TEF6686.h"

#define High_16bto8b(a)  ((uint8_t)((a) >> 8))
#define Low_16bto8b(a)  ((uint8_t)(a ))
#define MAKEINT(msb, lsb) (((msb) << 8) | (lsb))

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

bool TEF6686::readRDS() {
  char status;
  char response[16]; 
  bool ps_rdy = false;

  uint16_t rdsStat, rdsA, rdsB, rdsC, rdsD, rdsErr;
  uint16_t result = devTEF668x_Radio_Get_RDS_Data(1, &rdsStat, &rdsA, &rdsB, &rdsC, &rdsD, &rdsErr);

  response[4] = High_16bto8b(rdsA);    
  response[5] = Low_16bto8b(rdsA);
  response[6] = High_16bto8b(rdsB);
  response[7] = Low_16bto8b(rdsB);
  response[8] = High_16bto8b(rdsC);
  response[9] = Low_16bto8b(rdsC);
  response[10] = High_16bto8b(rdsD);
  response[11] = Low_16bto8b(rdsD);
 
  byte pty = ((response[6]&3) << 3) | ((response[7] >> 5)&7);
  ptystr(pty);

  byte type = (response[6]>>4) & 15;
  bool version = bitRead(response[6], 4);
  bool tp = bitRead(response[6], 5);  
  int pi;

  if (version == 0) {
    pi = MAKEINT(response[4], response[5]);
  } else {
    pi = MAKEINT(response[8], response[9]);
  }
  if(pi>=21672){
    _csign[0]='W';
    pi-=21672;
  }
  else if(pi<21672 && pi>=4096){
    _csign[0]='K';
    pi-=4096;
  }
  else{
    pi=-1;
  }
  if(pi>=0){
    _csign[1]=char(pi/676+65);//char(pi/676);
    _csign[2]=char((pi - 676*int(pi/676))/26+65);//char((pi-676*(_csign[1]))/26+65);
    _csign[3]=char(((pi - 676*int(pi/676))%26)+65);//char((pi-676*(_csign[1]))%26+65);
    _csign[4]='\0';   
  }
  else{
    _csign[0]='U';
    _csign[1]='N';
    _csign[2]='K';
    _csign[3]='N';
    _csign[4]='\0';
  }
  
  // Groups 0A & 0B
  // Basic tuning and switching information only
  if (type == 0) {
    bool ta = bitRead(response[7], 4);
    bool ms = bitRead(response[7], 3);
    byte addr = response[7] & 3;
    bool diInfo = bitRead(response[7], 2);
 
    // Groups 0A & 0B: to extract PS segment we need blocks 1 and 3
    if (addr >= 0 && addr<= 3) {
      if (response[10] != '\0')
        _ps[addr*2] = response[10];
      if (response[11] != '\0')
        _ps[addr*2+1] = response[11];
      ps_rdy=(addr==3);
    } 
    printable_str(_ps, 8);
  }
  // Groups 2A & 2B
  // Radio Text
  else if (type == 2) {
    // Get their address
    int addressRT = response[7] & 15; // Get rightmost 4 bits
    bool ab = bitRead(response[7], 4);
    bool cr = 0; //indicates that a carriage return was received
    byte len = 64;
    if (version == 0) {
      if (addressRT >= 0 && addressRT <= 15) {
        if (response[8] != 0x0D )
          _disp[addressRT*4] = response[8];
        else{
          len=addressRT*4;
          cr=1;
        }
        if (response[9] != 0x0D)
          _disp[addressRT*4+1] = response[9];
        else{
          len=addressRT*4+1;
          cr=1;
        }
        if (response[10] != 0x0D)
          _disp[addressRT*4+2] = response[10]; 
        else{
          len=addressRT*4+2;
          cr=1;
        }
        if (response[11] != 0x0D)
          _disp[addressRT*4+3] = response[11];
        else{
          len=addressRT*4+3;
          cr=1;
        }
      }
    } else {
      if (addressRT >= 0 && addressRT <= 7) {
        if (response[10] != '\0')
          _disp[addressRT*2] = response[10];
        if (response[11] != '\0')
          _disp[addressRT*2+1] = response[11];
      }
    }
    if(cr){
      for (byte i=len; i<64; i++) _disp[i] = ' ';
    }
    if (ab != _ab) {      
      for (byte i=0; i<64; i++) _disp[i] = ' ';
      _disp[64] = '\0';     
      _newRadioText=1;
    }
    else{
      _newRadioText=0;
    }
    _ab = ab;
    printable_str(_disp, 64);
  }
  // Group 4A Clock-time and Date
  //Note the time is localized but the date is the UTC date
  //Setting offset to 0 will make the time referenced to UTC
  else if (type == 4 && version == 0){  
    unsigned long MJD = (response[7]&3<<15 | u_int(response[8])<<7 | (response[9]>>1)&127)&0x01FFFF;
    u_int Y=(MJD-15078.2)/365.25;
    u_int M=(MJD-14956.1-u_int(Y*365.25))/30.6001;    
    byte K;
    if(M==14||M==15)
      K=1;
    else
      K=0;
    _year=(Y+K)%100;
    _month=M-1-K*12;
    _day=MJD-14956-u_int(Y*365.25)-u_int(M*30.6001);    
    
    char sign=(((response[11]>>5)&1)/(-1));
    if(sign==0)sign=1; //Make sign a bipolar variable
    char offset=sign*(response[11]&31);

    _hour=((response[9]&1)<<4) | ((response[10]>>4)&15);
    _minute=((response[10]&15)<<2 | (response[11]>>6)&3);     
    _hour= (_hour + (offset/2) + 24)%24;    
    _minute=(_minute + (offset)%2*30 + 60)%60;
  } 
  delay(40);
  //This is a simple way to indicate when the ps data has been fully refreshed.
  return ps_rdy; 
}
 
void TEF6686::getRDS(Station * tunedStation) {
  strcpy(tunedStation->programService, _ps);
  strcpy(tunedStation->radioText, _disp); 
  strcpy(tunedStation->programType, _pty);
  strcpy(tunedStation->callSign, _csign);
  tunedStation->newRadioText=_newRadioText;   
}

void TEF6686::ptystr(byte pty) {  
  // Translate the Program Type bits to the RBDS 16-character fields  
  if(pty>=0 && pty<32){
    char* pty_LUT[51] = { 
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
      strcpy(_pty, pty_LUT[pty]);
  }
  else{
    strcpy(_pty, "    PTY ERROR   ");
  } 
}

void TEF6686::printable_str(char * str, int length){  
  for(int i=0;i<length;i++){    
    if( (str[i]!=0 && str[i]<32) || str[i]>126 ) str[i]=' ';  
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
