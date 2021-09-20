
/***************************************************************************/
//	Function: Measure the distance to obstacles in front and print the distance
//			  value to the serial terminal.The measured distance is from
//			  the range 0 to 400cm(157 inches).
//	Hardware: Grove - Ultrasonic Ranger
//	Arduino IDE: Arduino-1.0
/*****************************************************************************/

#include "Ultrasonic.h"
Ultrasonic Oultrasonic(5);
Ultrasonic Fultrasonic(14);

long SWert_rein;
long SWert_raus;
String sequence ="";
int currentPeople = 0;
  
void setup()
{
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

void loop()
{
	long RangeInCentimeters;
  long FRange;

	
	RangeInCentimeters = Oultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
	FRange = Fultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
  Serial.print("Octopus:");
  Serial.print(RangeInCentimeters);//0~400cm
	Serial.println(" cm");
  Serial.print("Frei:");
  Serial.print(FRange);//0~400cm
  Serial.println(" cm");
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
