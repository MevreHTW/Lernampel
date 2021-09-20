/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */
#include <ESP8266WiFi.h>
#include <lmic.h>
#include <hal/hal.h>

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

IPAddress myOwnIP; // ownIP for mDNS 

//Keys TTN
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

#include "Ultrasonic.h"
Ultrasonic Oultrasonic(5);
Ultrasonic Fultrasonic(14);

long SWert_rein;
long SWert_raus;
String sequence ="";
int currentPeople = 0;
long RangeInCentimeters;
long FRange;


void setup(){ // Einmalige Initialisierung
  Serial.begin(115200);
  //------------ WLAN initialisieren 
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print ("\nWLAN connect to:");
  Serial.print("FRITZ!Box 6430 Cable EP");
  WiFi.begin("FRITZ!Box 6430 Cable EP","93670237953555842790");
  while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
    delay(500); 
    Serial.print(".");
  };
  Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
  matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
  myOwnIP = WiFi.localIP();
  matrixausgabe_index=0;

  SWert_rein = Oultrasonic.MeasureInCentimeters();
  SWert_raus = Fultrasonic.MeasureInCentimeters();

  Serial.print("Distanz für rein:");
  Serial.println(SWert_rein);
  Serial.print("Distanz für raus:");
  Serial.println(SWert_raus);
  delay(500);

 //Standartwert wird gesetzt,sobald sich der Octopus mit dem LoRaWan verbindet! 
}

void loop() { // Kontinuierliche Wiederholung

  //delay( 1000 );
  
  RangeInCentimeters = Oultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
  FRange = Fultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
  Serial.print("Octopus:");
  Serial.print(RangeInCentimeters);//0~400cm
  Serial.println(" cm");
  Serial.print("Frei:");
  Serial.print(FRange);//0~400cm
  Serial.println(" cm");
  Serial.print("Anzahl: ");
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
