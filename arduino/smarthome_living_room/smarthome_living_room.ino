#include <RCSwitch.h>

#define RELAY_PIN 2
#define RECEIVER_PIN 3

RCSwitch rc_switch = RCSwitch();

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  

  Serial.begin(9600);
  rc_switch.enableReceive(1); // pin d3, the second (of two) pin that supports interrupts 
}

void loop() {
  if(rc_switch.available()) {
    int value = rc_switch.getReceivedValue();

    Serial.println("Got something");

    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print(rc_switch.getReceivedValue());
      Serial.print(" / ");
      Serial.print(rc_switch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(rc_switch.getReceivedProtocol());
    }

    rc_switch.resetAvailable();
  }
}
