#include "tef6686.h"

void setup() {
  int r = 0;
  
  Wire.begin(D1, D2);
  delay(1000);
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("Begin");

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
}

void loop() {
  /*
  byte error, address;
  int nDevices;

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
  Serial.print(".");
   delay(1000); 
}
