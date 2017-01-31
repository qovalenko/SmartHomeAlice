#include <RCSwitch.h>
#include <dht.h>

dht DHT;

#define RELAY_PIN    5

#define RECEIVER_PIN_INTER 1 // D3
#define RECEIVER_POW       4

#define SENDER_PIN   10
#define SENDER_POW   6

#define DHT_PIN      2 // 3.3v

#define ALL_STATIONS   1
#define CUR_STATION_ID 2

int temperature = 0;
int humidity    = 0;

RCSwitch rc_switch = RCSwitch();

byte debug = 1;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SENDER_POW, OUTPUT);
  pinMode(RECEIVER_POW, OUTPUT);
  randomSeed(analogRead(0));
  
  digitalWrite(RECEIVER_POW, 1);
  rc_switch.enableReceive(RECEIVER_PIN_INTER); // D3 
  Serial.begin(9600);
  Serial.println("Started! Listening..");
  delay(500);
  get_temp_hum();
  Serial.println(digitalRead(RELAY_PIN));
}

void send_status() {
  // Wait until sensor data arrives
  while (!get_temp_hum());
   
  String tmp = (String) CUR_STATION_ID;
  if (temperature == 0 && humidity == 0) 
   tmp += "00000";  
  else {
   tmp += temperature;
   tmp += humidity;
  }
  tmp += (digitalRead(RELAY_PIN)+1);
  Serial.print("Data prepared to send: ");
  Serial.println(tmp);
 
  send_data(tmp.toInt());
  Serial.println("Data packet sent.");
}

// Send data to Raspberry and switch the Receiver ON
void send_data(long data) {
 delay(random(200, 500));
 rc_switch.disableReceive();
 Serial.println("NOT Listening...Sending.");
 delay(1);
 digitalWrite(SENDER_POW, 1);
 delay(1);
 rc_switch.enableTransmit(SENDER_PIN);
 rc_switch.setPulseLength(320);
 rc_switch.setProtocol(1);
 rc_switch.setRepeatTransmit(15);
 rc_switch.send(data, 24);
 delay(10);
 digitalWrite(SENDER_POW, 0);
 digitalWrite(RECEIVER_POW, 1);
 delay(1);
 Serial.println("Done sending...Listening.");
 rc_switch.enableReceive(RECEIVER_PIN_INTER);
}

void parse_data(long data, int length) {
 int station_id  = data / 1000000;
 long relay      = data % 10;

 if (station_id == ALL_STATIONS) {
  Serial.println("Got a request for all stations. Switching the relay."); 
  digitalWrite(RELAY_PIN, (relay - 1));
  Serial.println("Relay switched.");
  Serial.println("Sending status an raspberry...");
  send_status();
 }

 if (station_id == CUR_STATION_ID) {
  Serial.println("Got a request for the current station. Parsing..."); 
  if (relay == 0) {
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
   send_status();
  } else {
   digitalWrite(RELAY_PIN, (relay - 1));
   Serial.println("Relay switched.");
   Serial.println("Sending status an raspberry...");
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
   Serial.println("Got sensor data!");
   Serial.print("H: ");
   Serial.println(humidity);
   Serial.print("T: ");
   Serial.println(temperature);
   break;

  // Error, try again
  default:
   dht_fail_count++;
   if (dht_fail_count > 5) {
    Serial.println("Failed to get HUM and TEMP.");
    humidity = 0;
    temperature = 0;
    dht_fail_count = 0;
   } else {
    delay(100);
    goto start;
   }
   break;
 }
 return 1;
}

void loop() {

  if (rc_switch.available()) {
    parse_data(rc_switch.getReceivedValue(), rc_switch.getReceivedBitlength());
    delay(500);
    rc_switch.resetAvailable(); 
  }
}