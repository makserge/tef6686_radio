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

void TEF6686::setChannel(uint16_t frequency) {
    Radio_SetFreq(Radio_PRESETMODE, FM1_BAND, frequency);
}
/*
uint16_t TEF6686::seekUp()
{
	return seek(SEEK_UP);
}
*/
/*
uint16_t TEF6686::seekDown()
{
	return seek(SEEK_DOWN);
}
*/
/*
void TEF6686::setVolume(uint16_t volume)
{
  readRegisters(); //Read the current register set
  if(volume < 0) volume = 0;
  if (volume > 15) volume = 15;
  si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
  si4703_registers[SYSCONFIG2] |= volume; //Set new volume
  updateRegisters(); //Update
}
*/
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




//To get the Si4703 inito 2-wire mode, SEN needs to be high and SDIO needs to be low after a reset
//The breakout board has SEN pulled high, but also has SDIO pulled high. Therefore, after a normal power up
//The Si4703 will be in an unknown state. RST must be controlled
void TEF6686::si4703_init() 
{
  pinMode(_resetPin, OUTPUT);
  pinMode(_sdioPin, OUTPUT); //SDIO is connected to A4 for I2C
  digitalWrite(_sdioPin, LOW); //A low SDIO indicates a 2-wire interface
  digitalWrite(_resetPin, LOW); //Put Si4703 into reset
  delay(1); //Some delays while we allow pins to settle
  digitalWrite(_resetPin, HIGH); //Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
  delay(1); //Allow Si4703 to come out of reset

  Wire.begin(); //Now that the unit is reset and I2C inteface mode, we need to begin I2C

  readRegisters(); //Read the current register set
  //si4703_registers[0x07] = 0xBC04; //Enable the oscillator, from AN230 page 9, rev 0.5 (DOES NOT WORK, wtf Silicon Labs datasheet?)
  si4703_registers[0x07] = 0x8100; //Enable the oscillator, from AN230 page 9, rev 0.61 (works)
  updateRegisters(); //Update

  delay(500); //Wait for clock to settle - from AN230 page 9

  readRegisters(); //Read the current register set
  si4703_registers[POWERCFG] = 0x4001; //Enable the IC
  //  si4703_registers[POWERCFG] |= (1<<SMUTE) | (1<<DMUTE); //Disable Mute, disable softmute
  si4703_registers[SYSCONFIG1] |= (1<<RDS); //Enable RDS

//  si4703_registers[SYSCONFIG1] |= (1<<DE); //50kHz Europe setup
// Uncomment this for European (100kHz) channel spacing, default (no setting, or zero bit 4 setting) is US channel spacing (200kHz)
//  si4703_registers[SYSCONFIG2] |= (1<<SPACE0); //100kHz channel spacing for Europe

  si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
  si4703_registers[SYSCONFIG2] |= 0x0001; //Set volume to lowest
  updateRegisters(); //Update

  delay(110); //Max powerup time, from datasheet page 13
}

//Read the entire register control set from 0x00 to 0x0F
void TEF6686::readRegisters(){

  //Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00.
  Wire.requestFrom(SI4703, 32); //We want to read the entire register set from 0x0A to 0x09 = 32 bytes.

  while(Wire.available() < 32) ; //Wait for 16 words/32 bytes to come back from slave I2C device
  //We may want some time-out error here

  //Remember, register 0x0A comes in first so we have to shuffle the array around a bit
  for(uint16_t x = 0x0A ; ; x++) { //Read in these 32 bytes
    if(x == 0x10) x = 0; //Loop back to zero
    si4703_registers[x] = Wire.read() << 8;
    si4703_registers[x] |= Wire.read();
    if(x == 0x09) break; //We're done!
  }
}

//Write the current 9 control registers (0x02 to 0x07) to the Si4703
//It's a little weird, you don't write an I2C addres
//The Si4703 assumes you are writing to 0x02 first, then increments
byte TEF6686::updateRegisters() {

  Wire.beginTransmission(SI4703);
  //A write command automatically begins with register 0x02 so no need to send a write-to address
  //First we send the 0x02 to 0x07 control registers
  //In general, we should not write to registers 0x08 and 0x09
  for(uint16_t regSpot = 0x02 ; regSpot < 0x08 ; regSpot++) {
    byte high_byte = si4703_registers[regSpot] >> 8;
    byte low_byte = si4703_registers[regSpot] & 0x00FF;

    Wire.write(high_byte); //Upper 8 bits
    Wire.write(low_byte); //Lower 8 bits
  }

  //End this transmission
  byte ack = Wire.endTransmission();
  if(ack != 0) { //We have a problem! 
    return(FAIL);
  }

  return(SUCCESS);
}
*/
//Seeks out the next available station
//Returns the freq if it made it
//Returns zero if failed
/*
uint16_t TEF6686::seek(byte seekDirection){
  readRegisters();
  //Set seek mode wrap bit
  si4703_registers[POWERCFG] |= (1<<SKMODE); //Allow wrap
  //si4703_registers[POWERCFG] &= ~(1<<SKMODE); //Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first
  if(seekDirection == SEEK_DOWN) si4703_registers[POWERCFG] &= ~(1<<SEEKUP); //Seek down is the default upon reset
  else si4703_registers[POWERCFG] |= 1<<SEEKUP; //Set the bit to seek up

  si4703_registers[POWERCFG] |= (1<<SEEK); //Start seek
  updateRegisters(); //Seeking will now start

  //Poll to see if STC is set
  while(1) {
    readRegisters();
    if((si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
  }

  readRegisters();
  uint16_t valueSFBL = si4703_registers[STATUSRSSI] & (1<<SFBL); //Store the value of SFBL
  si4703_registers[POWERCFG] &= ~(1<<SEEK); //Clear the seek bit after seek has completed
  updateRegisters();

  //Wait for the si4703 to clear the STC as well
  while(1) {
    readRegisters();
    if( (si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
  }

  if(valueSFBL) { //The bit was set indicating we hit a band limit or failed to find a station
    return(0);
  }
	// clear radiotext buffer on seek
	for (uint16_t i=0; i < 65; i++) {
	 radiotext[i] = 0;
	}
return getChannel();
}
*/
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
