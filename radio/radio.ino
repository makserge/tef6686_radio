#include <Wire.h>
#include "TEF6686.h"

int frequency;
int volume;

char rdsname[9];
char rdsrt[65];
char previousRadioText[65];
uint8_t lastChar;

TEF6686 radio;

TwoWire WIRE2 (2, I2C_FAST_MODE);
#define Wire WIRE2

void setup() {
  //Wire.begin(D1, D2);
  delay(10000);
  Wire.begin();
  delay(1000);
  Serial.begin(115200);
  //while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("Begin");

  radio.init();
  Serial.println("???");
  radio.powerOn();
  radio.setFrequency(10000);
  Serial.println(radio.getFrequency());
  Serial.println("Done");
}

void loop() {
  if (radio.readRDSRadioText(rdsrt)) {
    if (strcmp(rdsrt, previousRadioText)) {
      Serial.println(rdsrt);          
      strcpy(previousRadioText, rdsrt);
    }
  }
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'm') {
      radio.setMute();
      displayInfo();
    }
    else if (ch == 'n') {
      radio.setUnMute();
      displayInfo();
    }
    else if (ch == 'p') {
      radio.powerOn();
      displayInfo();
    } 
    else if (ch == 'o') {
      radio.powerOff();
      displayInfo();
    }
    if (ch == 'u') {
      frequency = radio.seekUp();
      displayInfo();
    } 
    else if (ch == 'd') {
      frequency = radio.seekDown();
      displayInfo();
    } 
    else if (ch == '+') {
      volume += 4;
      if (volume >=24) volume = 24;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == '-') {
      volume -= 4;
      if (volume < -60) volume = -60;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == 'a') {
      frequency = radio.tuneUp();
      displayInfo();
    }
    else if (ch == 'b') {
      frequency = radio.tuneDown();
      displayInfo();
    }
    else if (ch == 'r') {
      // The calling of readRDS and printing of rdsname really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
      radio.readRDSRadioStation(rdsname);
      Serial.println(rdsname);
    }
    else if (ch == 'R') {
      // The calling of readRDS and printing of rdsrt really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
      //Serial.println("RDS listening - screen");
      
      radio.readRDS(rdsname, rdsrt, &lastChar);
      Serial.println(rdsrt); 
    }        
  }
}

void displayInfo() {
   Serial.print("Frequency:"); 
   Serial.print(frequency); 
   Serial.print(" Volume:"); 
   Serial.println(volume); 
}
