#include <avr/sleep.h>
#include <VirtualWire.h>

//Define pin
const int transmit_pin = 4;
const int pin_transistor = 7;

int pin2_interrupt_flag = 0;
unsigned long lastISR = 0;

void setup() {
  //Setup mode pin
  pinMode(pin_transistor,OUTPUT);
  pinMode(transmit_pin,OUTPUT);
  
  digitalWrite(pin_transistor,HIGH);
  //Serial.begin(9600);
  //Setup RF module
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);       // Bits per sec
  //Enable deep sleep
  sleepNow();
}

//Function to enable the deep sleep and restore by an interrupt
void sleepNow() {
  sleep_enable();
  attachInterrupt(0, pin2_isr, RISING);
  /* 0, 1, or many lines of code here */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  /* wake up here */
  sleep_disable();
}


//Function called when the interrupt is detected
void pin2_isr() {
   //Do something only if the last interrupt was more than 3 seconds
   if ((millis()-lastISR) > 3000) {
      sleep_disable();
      detachInterrupt(0); 
      //Serial.println("ISR");
      lastISR = millis();
      pin2_interrupt_flag = 1;   
   }
}

void loop() {
  //Turn on RF transmitter
  digitalWrite(pin_transistor,HIGH);
  //Setup a text
  char msg[4] = {'m','a','i','l'};
  //Send the text
  vw_send((uint8_t *)msg, 4);
  vw_wait_tx(); // Wait until the whole message is gone

  //Wait a while 
  delay(2000);
  //Turn off RF transmitter
  digitalWrite(pin_transistor,LOW);
  //Go to sleep
  sleepNow();
}
