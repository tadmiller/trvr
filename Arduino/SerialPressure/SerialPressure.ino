/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
 * # Description
 * Allows pressure readings to be transferred over USB serial
 */

// https://www.alanzucconi.com/2015/10/07/how-to-integrate-arduino-with-unity/
// https://medium.com/@yifeiyin/communicationbetween-arduino-and-unity-9fdcccc2be3f

#include <ArduinoJson.h>

void setup()
{
	Serial.begin(9600);

	pinMode(4, INPUT_PULLUP);

	Serial.println("Begin");
}

void loop()
{
	Serial.flush();

  DynamicJsonDocument readings(1024);

	readings["front"] = analogRead(A1);
	readings["back"] = analogRead(A2);
	readings["midl"] = analogRead(A3);
	readings["midr"] = analogRead(A4);

	serializeJson(readings, Serial);
	
	//String packetData = "";
	//readings.printTo(packetData); //Serial, packetData
	//Serial.println(packetData);

	delay(200);
}
