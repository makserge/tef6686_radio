#include "TEF6686.h"

int frequency;
int volume;

bool ps_rdy;  
char pty_prev[17]="                ";
char ps_prev[9];
char radio_text_prev[16];

TEF6686 radio;
Station tuned;

void setup() {
  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Begin");
  radio.init();
  radio.powerOn();
  radio.setFrequency(10360);
  frequency = radio.getFrequency();
  displayInfo();  
}

void loop() {
  readRds();
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
  }
}

void readRds() {
  ps_rdy = radio.readRDS(); 
  radio.getRDS(&tuned);

  //showPTY();
  showPS();
  showRadioText(); 
}

void showPTY() {
  if(!strcmp(tuned.programType,pty_prev,16)){ 
    Serial.print(tuned.programType);
    strcpy(pty_prev,tuned.programType);
    Serial.println();  
  }  
}

void showPS(){
        if (strlen(tuned.programService) == 8){
            if(ps_rdy){      
                if(!strcmp(tuned.programService,ps_prev,8)){         
                  Serial.print("-=[ ");
                  Serial.print(tuned.programService); 
                  Serial.print(" ]=-");     
                        strcpy(ps_prev,tuned.programService);
                  Serial.println();      
          }  
            }    
        }
}

bool strcmp(char* str1, char* str2, int length){
  bool same=true;
  for(int i=0;i<length;i++){
    if(str1[i]!=str2[i]){
      same=false;
      break;
    }    
  }  
  return same;
}

void showRadioText(){
  if(!strcmp(tuned.radioText,radio_text_prev,16)){ 
    Serial.print(tuned.radioText);
    strcpy(radio_text_prev,tuned.radioText);
    Serial.println();  
  }
}

void displayInfo() {
   Serial.print("Frequency:"); 
   Serial.print(frequency); 
   Serial.print(" Volume:"); 
   Serial.println(volume); 
}
