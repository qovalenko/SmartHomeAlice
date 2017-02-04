#include <RCSwitch.h>
#include <EEPROM.h>
#include <dht.h>

dht DHT;
RCSwitch rc_switch = RCSwitch();

#define EE_ADDR            0

#define BUTTON_PIN         7

#define RELAY_PIN          5

// interrupt 0 -> D2, interrupt 1 -> D3
// to switch on the receiver - toggle pow pin
#define RECEIVER_PIN_INTER 1 
#define RECEIVER_POW       4

// to switch on the sender - toggle pow pin
#define SENDER_PIN         10
#define SENDER_POW         6

// use with 3.3v
#define DHT_PIN            2 

// stations id
#define ALL_STATIONS       1
#define CUR_STATION_ID     3

// if the MC asks the state
#define STATUS_INFO        0
#define TOGGLE             5

//#define receiver_power(x) x ? PORTD = (1<<PD4) : PORTD = ~(1<<PD4); 
//#define sender_power(x)   x ? PORTD = (1<<PD6) : PORTD = ~(1<<PD6); 

#define receiver_power(x). digitalWrite(RECEIVER_POW, 1);
#define sender_power(x)    digitalWrite(SENDER_POW, 1);

byte lbutton_state = 0;
#define toggle_lights()    digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
#define check_lbtn_state() lbutton_state = digitalRead(BUTTON_PIN);

// current temp, hum status
int temperature    = 0;
int humidity       = 0;

// just started? send status to the MC
byte started = 1;

// if button is present
// uncomment define
#define lights_button
#define debug

/* Functions
 *  MC - Main Controller (Raspi)
 *  
 *  void configurePins()       			- configure pin modes at startup
 *  void send_status()         			- sends current status of t/h/relay to MC
 *  void send_data(long data)  			- sends data to the MC
 *  void parse_data(long data) 			- parse incoming data from the MC
 *  byte get_temp_hum()        			- get status of t/h/relay (usage: while (!get_temp_hum());)
 *  void change_relay_state(byte state) - do this if relay status has been changed
 *  
 *  (c) chernikovalexey, .dl
 *  04.02.17
 */

void configurePins() {
 pinMode(RELAY_PIN, OUTPUT);
 pinMode(SENDER_POW, OUTPUT);
 pinMode(RECEIVER_POW, OUTPUT); 
 receiver_power(1);
 rc_switch.enableReceive(RECEIVER_PIN_INTER);
 digitalWrite(RELAY_PIN, EEPROM.read(EE_ADDR));
 #ifdef lights_button
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
 #endif
}

void change_relay_state(byte state) { 
 if (state != TOGGLE) 
  digitalWrite(RELAY_PIN, state);
 else
  toggle_lights(); 
 
 EEPROM.write(EE_ADDR, (byte)state);
 send_status();
}

void send_status() {
  while (!get_temp_hum());
  String PKT = (String) CUR_STATION_ID;
  if (temperature == 0 && humidity == 0) 
   PKT += "00000";  
  else {
   PKT += temperature;
   PKT += humidity;
  }
  PKT += (digitalRead(RELAY_PIN)+1);
  #ifdef debug
   Serial.print("Data prepared to send: ");
   Serial.println(PKT);
  #endif 
  send_data(PKT.toInt());
  #ifdef debug
   Serial.println("Data packet sent.");
  #endif
}

void send_data(long data) {
 randomSeed(analogRead(0));
 delay(random(50, 400));
 rc_switch.disableReceive();
 receiver_power(0);
 #ifdef debug
  Serial.println("NOT Listening...Sending.");
 #endif
 sender_power(1);
 rc_switch.enableTransmit(SENDER_PIN);
 rc_switch.setPulseLength(320);
 rc_switch.setProtocol(1);
 rc_switch.setRepeatTransmit(15);
 rc_switch.send(data, 24);
 delayMicroseconds(100);
 sender_power(0);
 receiver_power(1);
 rc_switch.enableReceive(RECEIVER_PIN_INTER);
 #ifdef debug
  Serial.println("Done sending...Listening...");
 #endif
}

void parse_data(long data) {
 int station_id  = data / 1000000;
 long relay      = data % 10;

 // a msg sent to all stations means switch relays
 if (station_id == ALL_STATIONS) {
  change_relay_state(relay - 1);
  #ifdef debug
   Serial.println("Got a request for all stations. Switching the relay."); 
   Serial.print("Relay switched. State: ");
   Serial.println(digitalRead(RELAY_PIN));
   Serial.println("Sending status an raspberry...");
  #endif
  send_status();
 }

 if (station_id == CUR_STATION_ID) {
  #ifdef debug
   Serial.println("Got a request for the current station. Parsing..."); 
  #endif
  if (relay == STATUS_INFO) {
   #ifdef debug
    Serial.println("Got a status request. Preparing."); 
    Serial.print("STATION ID: ");
    Serial.println(CUR_STATION_ID);
    Serial.print("TEMP: ");
    Serial.println(temperature);
    Serial.print("HUMIDITY %:");
    Serial.println(humidity);
    Serial.print("RELAY STATE: ");
    Serial.println(digitalRead(RELAY_PIN));
    Serial.println("");
    Serial.println("Sending status an raspberry...");
   #endif
   send_status();
  } else {
   change_relay_state(relay - 1);
   #ifdef debug
    Serial.println("Relay switched.");
    Serial.println("Sending status an raspberry...");
   #endif
   send_status();
  }
 }
}

byte get_temp_hum() {
 byte dht_fail_count = 0;

 start:
  int chk = DHT.read22(DHT_PIN);
 switch (chk) {
  case DHTLIB_OK:
   humidity = DHT.humidity;
   temperature = DHT.temperature*10;
   #ifdef debug
    Serial.println("Got sensor data.");
    Serial.print("H: ");
    Serial.println(humidity);
    Serial.print("T: ");
    Serial.println(temperature);
   #endif
   break;

  // error, try again
  default:
   dht_fail_count++;
   if (dht_fail_count > 5) {
    #ifdef debug
     Serial.println("Failed to get HUM and TEMP.");
    #endif
    humidity = 0;
    temperature = 0;
    dht_fail_count = 0;
   } else {
    delay(50);
    goto start;
   }
   break;
 }
 return 1;
}

void setup() {
 configurePins();
 #ifdef lights_button
  check_lbtn_state();
 #endif	 
 #ifdef debug
  Serial.begin(9600);
  Serial.println("Started! Getting T/H values..");
 #endif
  // wait until sensor data arrives
  while (!get_temp_hum());
  Serial.println("Started! Listening...");
}

void loop() {
  if (started) {
   started = 0;
   randomSeed(analogRead(0));
   delay(random(100, 500));  
   send_status();
  } 
	
  #ifdef lights_button
	if (lbutton_state != digitalRead(BUTTON_PIN)) {
	 change_relay_state(TOGGLE);
	 send_status();
	 #ifdef debug  
	  Serial.println("Button pressed. Relay switched. Sending data...");
     #endif
	}
  #endif	
	
  if (rc_switch.available()) {
	#ifdef debug  
     Serial.print("Got data: ");
     Serial.println(rc_switch.getReceivedValue());
	#endif
    parse_data(rc_switch.getReceivedValue());
    delay(222);
    rc_switch.resetAvailable(); 
  }
}

