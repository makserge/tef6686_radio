#include "TEF6686.h"

int frequency;
int volume;

uint8_t isRDSReady;  

char programTypePrevious[17] = "                ";
char programServicePrevious[9];
char radioTextPrevious[65];

boolean isFmSeekMode;
boolean isFmSeekUp;

TEF6686 radio;
RdsInfo rdsInfo;

void setup() {
  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Begin");
  radio.init();
  radio.powerOn();
  radio.setFrequency(10700);
  frequency = radio.getFrequency();
  displayInfo();  
}

void loop() {
  readRds();
  showFmSeek();
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
    else if (ch == 'u') {
      frequency = radio.seekUp();
      displayInfo();
    } 
    else if (ch == 'd') {
      frequency = radio.seekDown();
      displayInfo();
    }
    else if (ch == 'c') {
      Serial.println("Seeking up");
      isFmSeekMode = true;
      isFmSeekUp = true;
    } 
    else if (ch == 'e') {
      Serial.println("Seeking down");
      isFmSeekMode = true;
      isFmSeekUp = false;
    }
    else if (ch == '+') {
      volume += 4;
      if (volume >= 24) volume = 24;
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
  }
}

void readRds() {
  isRDSReady = radio.readRDS(); 
  radio.getRDS(&rdsInfo);

  showPTY();
  showPS();
  showRadioText(); 
}

void showPTY() {
  if ((isRDSReady == 1) && !strcmp(rdsInfo.programType, programTypePrevious, 16)) { 
    Serial.print(rdsInfo.programType);
    strcpy(programTypePrevious, rdsInfo.programType);
    Serial.println();  
  }  
}

void showPS() {
  if ((isRDSReady == 1) && (strlen(rdsInfo.programService) == 8) && !strcmp(rdsInfo.programService, programServicePrevious, 8)) {
    Serial.print("-=[ ");
    Serial.print(rdsInfo.programService);
    Serial.print(" ]=-");
    strcpy(programServicePrevious, rdsInfo.programService);
    Serial.println();
  }
}

void showRadioText() {
  if ((isRDSReady == 1) && !strcmp(rdsInfo.radioText, radioTextPrevious, 65)){
    Serial.print(rdsInfo.radioText);
    strcpy(radioTextPrevious, rdsInfo.radioText);
    Serial.println();  
  }
}

bool strcmp(char* str1, char* str2, int length) {
  for (int i = 0; i < length; i++) {
    if (str1[i] != str2[i]) {
      return false;
    }    
  }  
  return true;
}

void displayInfo() {
   delay(10);
   Serial.print("Frequency:"); 
   Serial.print(frequency); 
   Serial.print(" Volume:"); 
   Serial.println(volume);
   Serial.print("Level:"); 
   Serial.println(radio.getLevel() / 10);
   Serial.print("Stereo:"); 
   Serial.println(radio.getStereoStatus());
}

void showFmSeek() {
  if (isFmSeekMode) {
    if (radio.seekSync(isFmSeekUp)) {
      isFmSeekMode = false;
      Serial.println("Seek stopped");
      frequency = radio.getFrequency();
      displayInfo();
    }
  }
}
