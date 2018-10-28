#include <Wire.h>
#include "TEF6686.h"

TEF6686 radio;

/*
uint16_t clear_rds = 0;
uint16_t fm = 1;


uint8_t r, i; 
  uint16_t rds_stat, rds_a, rds_b, rds_c, rds_d, rds_err;
  char pstext[9];
  char rtext[65]; 
  uint16_t rds_offset;
  uint16_t rds_type = 0;
  char block_c[2];
  char block_d[2];
  char toggleWipe = 0;
 */
  
void setup() {
  Wire.begin(D1, D2);
  delay(1000);
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("Begin");

  //radio.powerOn();
 // radio.setVolume(1);
  //volume=1;
  radio.setChannel(10000);
  //delay(1000);
  //radio.setChannel(9850);
/*
  Tuner_Para_Init(); // This really doesn't do anything useful.
 // Seems to have originally been intended to load the last
  // state from eeprom. Now all it does is sets up the "current"
  // station as the second hard-coded preset.
  while (r == 0) {
    r = Tuner_Power_on(); // power on the tuner.
    Serial.print("Tuner_Power_on: ");
    Serial.println(r);
    // r == 0: busy
    // r == 1: success
    // r == 2: does not exist
  //  if (r == 2) exit(1);
  }
  
  if (!Tuner_Init()){ // load tuner initialization.
    exit(1);
  }
  
  Tuner_Turn_ON(); // this loads initial tuning to first preset.

  Serial.println("Done");
*/
  //memset(rtext,0,65);
  //memset(pstext,0,9);
}
/*
void rdsloop(){
  //while (fm){
    r = devTEF668x_Radio_Get_RDS_Data(0, &rds_stat, &rds_a, &rds_b, &rds_c, &rds_d, &rds_err);
    
    if (clear_rds){
      memset(pstext, 0, 9);
      memset(rtext, 0, 65);
      clear_rds = 0;
    }
//Serial.println(r);
    if (r && rds_b != 0x0 && (rds_stat & 0x8000) != 0x0){
      if ((rds_stat & 0x4000) != 0) Serial.println("RDS packet loss.\n");

      Serial.println(printf("Stat 0x%04X: A 0x%04X / B 0x%04X / C 0x%04X / D 0x%04X, ERR: 0x%04X\n", rds_stat, rds_a, rds_b, rds_c, rds_d, rds_err));

      // Block A: PI code -- station unique identifier. Ignore for now.
      // Block B: b15..b12 -- group type "0000" = station name, "0010" = radio text,
      //     b11 -- 0 = type A, 1 = type B
      //     b10 -- 1 = traffic program
      //      b09..b05 -- program type code (i.e. news, weather, rock, etc.)
      //      b04..b00 -- 

      if (((rds_b >> 8) & 0xf0) == 0x0){ // this is a PS message
        // If there is NOT an uncorrected error in block B or D (we don't care about A or C for PS)
        if ((rds_err & 0x0a00) == 0x0){ // other option is (rds_err & 0f00)
          rds_offset = (rds_b & 0x3) * 2;
          // Only process the update if data actually changes.
          if (pstext[rds_offset] != (rds_d >> 8) || pstext[rds_offset+1] != (rds_d & 0xff)){
            pstext[rds_offset] = rds_d >> 8;
            pstext[rds_offset+1] = rds_d & 0xff;
            Serial.println(sprintf("PS TEXT: %s\n", pstext));
          }
        }
      } else if (((rds_b >> 8) & 0xf0 )== 0x20){ // This is RADIO TEXT
        rds_type = (rds_b & 0x0800) >> 8;
        rds_offset = (rds_b & 0xf);
        if (rds_type == 0){
          rds_offset *= 4; // radio text type A has 4 bytes per update in blocks C and D
          //printf("RDS OFFSET: %d\n", rds_offset);
          // Radio text type A requires valid B, C, and D blocks.
          if ((rds_err & 0x0a00) == 0x0){ // other option is (rds_err & 0f00)
            if ((rds_b & 0x10) != toggleWipe) {
              memset(rtext, 0, 65);
              toggleWipe = (rds_b & 0x10);
            }
            for (i = 0; i < rds_offset; i++) if (rtext[i] == 0) rtext[i] = 0x20;
            block_c[0] = rds_c >> 8;
            block_c[1] = rds_c & 0xff;
            block_d[0] = rds_d >> 8;
            block_d[1] = rds_d & 0xff;
            if (block_c[0] == 0x0d) block_c[0] = 0;
            if (block_c[1] == 0x0d) block_c[1] = 0;
            if (block_d[0] == 0x0d) block_d[0] = 0;
            if (block_d[1] == 0x0d) block_d[1] = 0;
            if (rtext[rds_offset] != block_c[0] || rtext[rds_offset+1] != block_c[1]
                || rtext[rds_offset+2] != block_d[0] || rtext[rds_offset+3] != block_d[1]){

              rtext[rds_offset] = block_c[0];
              rtext[rds_offset+1] = block_c[1];
              rtext[rds_offset+2] = block_d[0];
              rtext[rds_offset+3] = block_d[1];

              Serial.println(sprintf("Radio Text (A): %s\n", rtext));
            }
          }
        } else {
          rds_offset *= 2; // radio text type B has 2 bytes per update in block D
          // Radio text type B requires valid B and D blocks.
          if (!((rds_err & 0x3000) == 0x3000 || (rds_err & 0x0300) == 0x0300)){
            if ((rds_b & 0x10) != toggleWipe) {
              memset(rtext, 0, 65);
              toggleWipe = (rds_b & 0x10);
            }
            for (i = 0; i < rds_offset; i++) if (rtext[i] == 0) rtext[i] = 0x20;
            block_d[0] = rds_d >> 8;
            block_d[1] = rds_d & 0xff;
            if (block_d[0] == 0x0d) block_d[0] = 0;
            if (block_d[1] == 0x0d) block_d[1] = 0;
            if (rtext[rds_offset] != block_d[0] || rtext[rds_offset+1] != block_d[1]){
              rtext[rds_offset] = block_d[0];
              rtext[rds_offset+1] = block_d[1];

              Serial.println(sprintf("Radio Text (B): %s\n", rtext));
            }
          }
        }
      } else {
        Serial.print("Unhandled GTYPE. RDS Block B: ");
        Serial.println(rds_b, HEX);
      }
    }
    if (rds_b == 0x0) delay(100);
  //}
}

*/

void loop() {
  /*
  byte error, address;
  uint16_t nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
  */
  //Serial.print(".");
  // rdsloop();
   // delay(590);
   if (Serial.available())
  {
    char ch = Serial.read();
    if (ch == 'u') 
    {
      //channel = radio.seekUp();
     // displayInfo();
    } 
    else if (ch == 'd') 
    {
     // channel = radio.seekDown();
     // displayInfo();
    } 
    else if (ch == '+') 
    {
     // volume ++;
     // if (volume >=16) volume = 15;
     // radio.setVolume(volume);
    //  displayInfo();
    } 
    else if (ch == '-') 
    {
      //volume --;
     // if (volume < 0) volume = 0;
     // radio.setVolume(volume);
     // displayInfo();
    } 
    else if (ch == 'a')
    {
      //channel = 1015; // Rock FM
//      radio.setChannel(channel);
     // displayInfo();
    }
    else if (ch == 'b')
    {
      //channel = 925; // KBCO in Boulder
      //radio.setChannel(channel);
      //displayInfo();
    }
    else if (ch == 'r')
    {
      // The calling of readRDS and printing of rdsname really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
 //     radio.readRDSRadioStation(rdsname);
//      Serial.println(rdsname);
    }
    else if (ch == 'R')
    {
      // The calling of readRDS and printing of rdsrt really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
      //Serial.println("RDS listening - screen");
      
 //     radio.readRDS(rdsname,rdsrt,&lastChar);
//      Serial.println(rdsrt); 
    }        
  }
}
