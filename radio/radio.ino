#include <Wire.h>
#include "TEF6686.h"

int frequency;
int volume;

TEF6686 radio;

void setup() {
  Wire.begin(D1, D2);
  delay(1000);
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("Begin");

  //radio.powerOn();
 // radio.setVolume(1);
  //volume=1;
  radio.setFrequency(10000);

  Serial.println("Done");
}

void loop() {
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'u') {
      frequency = radio.seekUp();
      displayInfo();
    } 
    else if (ch == 'd') {
      frequency = radio.seekDown();
      displayInfo();
    } 
    else if (ch == '+') {
     // volume ++;
     // if (volume >=16) volume = 15;
     // radio.setVolume(volume);
    //  displayInfo();
    } 
    else if (ch == '-') {
      //volume --;
     // if (volume < 0) volume = 0;
     // radio.setVolume(volume);
     // displayInfo();
    } 
    else if (ch == 'a') {
      //channel = 1015; // Rock FM
//      radio.setChannel(channel);
     // displayInfo();
    }
    else if (ch == 'b') {
      //channel = 925; // KBCO in Boulder
      //radio.setChannel(channel);
      //displayInfo();
    }
    else if (ch == 'r') {
      // The calling of readRDS and printing of rdsname really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
 //     radio.readRDSRadioStation(rdsname);
//      Serial.println(rdsname);
    }
    else if (ch == 'R') {
      // The calling of readRDS and printing of rdsrt really need
      // to be looped to catch all of the data...
      // this will just print a snapshot of what is in the Si4703 RDS buffer...
      //Serial.println("RDS listening - screen");
      
 //     radio.readRDS(rdsname,rdsrt,&lastChar);
//      Serial.println(rdsrt); 
    }        
  }
}

void displayInfo() {
   Serial.print("Frequency:"); Serial.print(frequency); 
   Serial.print(" Volume:"); Serial.println(volume); 
}
