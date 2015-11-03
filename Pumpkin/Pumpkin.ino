#include <SoftwareSerial.h>

static int8_t Send_buf[8] = {0} ;

#define CMD_PLAY_W_INDEX 0X03
#define CMD_SET_VOLUME 0X06
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_SINGLE_CYCLE 0X19
#define SINGLE_CYCLE_ON 0X00
#define SINGLE_CYCLE_OFF 0X01
#define CMD_PLAY_W_VOL 0X22


#define FIRST 0x0001
#define SECOND 0x0002
#define THIRD 0x0003


#define MODE_GLOW 1
#define MODE_FLASH 2

int pirPin = 7;

int mode = MODE_GLOW;

byte songIndex[] = {0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009};

int glowDirection[] =  {5,4,7,8};
int glowStatus[]= {0,10,50,80};
int flashStatus[]= {0,0,0,0};

int leds[]= {3,5,6,9};

#define NUM_LED 4

unsigned long startFlash = 0;
unsigned long stopFlash = 0;

boolean lockLow = true;

int timerOver = 0;
int maxTime = 10000;
int maxPause = 10000;


void setup() 
{
	Serial.begin(9600);

  for (int i = 0; i < NUM_LED;i++) {
      pinMode(leds[i],OUTPUT);
      digitalWrite(leds[i],LOW);
  }
  pinMode(pirPin,INPUT);
  
	delay(500);//Wait chip initialization is complete
    sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
	delay(200);//wait for 200ms
  sendCommand(CMD_SET_VOLUME,0x1416);
  
//  sendCommand(CMD_SET_VOLUME,0x1401);
  delay(200);
  //triggerMusic();
   randomSeed(analogRead(0));
	
}
void loop() 
{

  //PIR Logic
  int triggerPIR = digitalRead(pirPin);
  if (triggerPIR) {
    timerOver = millis()-stopFlash;
    if (timerOver > maxPause) {
    
    if (lockLow) {
       mode = MODE_FLASH;
       startFlash = millis();
       lockLow = false;
       triggerMusic();
    }
    }
  } else {
   // if (!lockLow) {
    //   lockLow = true;
   // }
  }

  //Mode logic
  if (mode == MODE_GLOW) { 
    glowLeds();
 
  }
  else {
     timerOver = millis()-startFlash;
     if (timerOver > maxTime) {
      mode = MODE_GLOW;
      stopFlash = millis();
      lockLow = true;
    }
    flashLeds();
  }

  delay(50);
}

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++)//
  {
    Serial.write(Send_buf[i]) ;
  }
}

void glowLeds() {

  for (int i = 0; i < NUM_LED;i++) {
     
      analogWrite(leds[i],glowStatus[i]);
      glowStatus[i] += glowDirection[i];

       
      if (glowDirection[i] > 0) {
          if (glowStatus[i] > 200) {
            glowDirection[i] *= -1;
          }
      } else {
          if (glowStatus[i] <= 1) {
            glowDirection[i] *= -1;
            glowStatus[i] = 2;
          }
      }    
  }   
}


void flashLeds() {  
   for (int i = 0; i < NUM_LED;i++) {
      digitalWrite(leds[i],flashStatus[i]);
      flashStatus[i] = !flashStatus[i];
  }
}

void triggerMusic() {
 int num = random(0,9);
 sendCommand(CMD_PLAY_W_INDEX, songIndex[num]);//play the first song with volume 15 class
 mode = MODE_FLASH  ;

}

