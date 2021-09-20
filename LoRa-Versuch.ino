#include "Ultrasonic.h"
Ultrasonic Oultrasonic(5);
Ultrasonic Fultrasonic(14);
#include <lmic.h>
#include <hal/hal.h>
#include <ESP8266WiFi.h>

long SWert_rein;
long SWert_raus;
String sequence ="";
int currentPeople = 0;
long RangeInCentimeters;
long FRange;


static const u1_t PROGMEM DEVEUI[8]={
  0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0x46, 0xE7};
void os_getDevEui (u1_t* buf) { 
  memcpy_P(buf, DEVEUI, 8);
}

static const u1_t PROGMEM APPEUI[8]={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
void os_getArtEui (u1_t* buf) { 
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM APPKEY[16]={
  0x94, 0x26, 0x8F, 0xCA, 0xC7, 0x85, 0x6D, 0x99, 0x53, 0x47, 0x21, 0x81, 0xB8, 0x52, 0x3E, 0xFB};
void os_getDevKey (u1_t* buf) {  
  memcpy_P(buf, APPKEY, 16);
};


void setup() {
  // put your setup code here, to run once:
  WiFi.forceSleepBegin(); // Wifi off
  Serial.begin(115200);
  // -- Initialisiere LoraWAN 
  os_init();             // LMIC LoraWAN
  LMIC_reset();          // Reset the MAC state 
  LMIC.txpow = 27;       // Maximum TX power 
  LMIC.datarate=DR_SF12; // Long Range
  LMIC.rps = updr2rps(LMIC.datarate);

  Serial.begin(115200);

  SWert_rein = Oultrasonic.MeasureInCentimeters();
  SWert_raus = Fultrasonic.MeasureInCentimeters();


  Serial.print("rein:");
  Serial.print(SWert_rein);
  Serial.print("raus:");
  Serial.print(SWert_raus);
  delay(500);

 //Standartwert wird gesetzt,sobald sich der Octopus mit dem LoRaWan verbindet!

}

void loop() {
  
  // put your main code here, to run repeatedly:
  { //Block------------------------------ sende Daten an TTN  
    int port = 1;
    static uint8_t mydata[3];
    int wert=round(54*1000);
    mydata[0] = wert >> 16; 
    mydata[1] = wert >> 8; 
    mydata[2] = wert ;
    // Check if there is not a current TX/RX job running
    //if (LMIC.opmode & OP_TXRXPEND) {
    if (LMIC.opmode & (1 << 7)) { 
      Serial.println(F("OP_TXRXPEND, not sending"));
      Serial.print("fehler:");
    } 
    else {
      // Prepare upstream data transmission at the next possible time.
      LoRaWAN_Tx_Ready = 0;                                 // Merker für ACK
      LMIC_setTxData2(port, mydata, sizeof(mydata), 0);     // Sende         
      Serial.println(F("Packet queued"));
      Serial.print("Frei:");
      while(LoRaWAN_Tx_Ready==0) {
        yield();
        os_runloop_once();
      };  // Warte bis gesendet
    }
  } // Blockende
  delay( 1000 );
  long RangeInCentimeters;

  
  RangeInCentimeters = Oultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
  Serial.print("Octopus:");
  Serial.print(RangeInCentimeters);//0~400cm
  Serial.println(" cm");
  Serial.print("Frei:");
  Serial.println(currentPeople);
  delay(750);

  if(RangeInCentimeters < SWert_rein - 10 && sequence.charAt(0) != '1'){
    sequence += "1";
  }else if(FRange < SWert_raus - 10 && sequence.charAt(0) != '2'){
    sequence += "2";
  }
  
  if(sequence.equals("12")){
    currentPeople++;
    Serial.print("Jmd geht rein"); 
    sequence="";
    delay(550);
  } else if (sequence.equals("21") && currentPeople > 0){
    currentPeople--; 
    Serial.print("Jmd geht raus"); 
    sequence="";
    delay(550);
    }
}
