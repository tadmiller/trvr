/*
 * # Description
 * This script can be downloaded to an Arduino MKR1000
 * It hosts an HTTP server that allows a certain type of request
 * to return data on pressure readings from pressure sensors
 *
 * This is used in collaboration with the Unity Engine for a senior capstone project
 *
 * # Sources
 * 		* WiFi Module: https://github.com/arduino-libraries/WiFi101/blob/master/examples/SimpleWebServerWiFi/SimpleWebServerWiFi.ino
 */

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>

#define READ_ERR -1
#define READ_SUCCESS 0
#define DEFAULT_PORT 80
#define DEBUG_LED LED_BUILTIN
int DEBUG = 0;

const char SSID[] = "DIL";
const char SSID_PW[] = "TheHumanC3nt1p3d3";
const int sensors[4] = {A1, A2, A3, A4};
int readings[4] = {0, 0, 0, 0};

WiFiServer server(DEFAULT_PORT);
 
void setup()
{
	delay(1000);
	if (Serial)
	{
		Serial.begin(9600);
		Serial.flush();
		DEBUG = 1;
	}

	pinMode(DEBUG_LED, OUTPUT);
	debugPrintln("Starting routine...");

	// Check that WiFi exists
	if (WiFi.status() == WL_NO_SHIELD)
	{
		Serial.println("NOT PRESENT");
		exit(0);
	}

	debugPrintln("Attempting to connect to WiFi");

	// Attempt to connect to Wifi network:
	for (int status = WL_IDLE_STATUS; status != WL_CONNECTED; )
		status = WiFi.begin(SSID, SSID_PW);

	WiFi.hostname("right.trvr");
	delay(100);
	server.begin();                           // start the web server on port 80
	printWifiStatus();                        // you're connected now, so print out the status
	digitalWrite(DEBUG_LED, HIGH);
}

void debugPrintln(String msg)
{
	if (DEBUG)
		Serial.println(msg);
}

// HTTP headers start with a response code (e.g. HTTP/1.1 200 OK)
// and a content-type so the client knows what's coming, then a blank line
void httpOK(WiFiClient * client)
{
	(* client).println("HTTP/1.1 200 OK");
	(* client).println("Content-type:text/html");
	(* client).println();
}

void jsonOK(WiFiClient * client)
{
	(* client).println("HTTP/1.1 200 OK");
	(* client).println("Content-type:application/json");
	(* client).println();
}

void jsonPrint(WiFiClient * client, String key, int val, int next)
{
	(* client).print("\t\"");
	(* client).print(key);
	(* client).print("\": \"");
	(* client).print(val);
	(* client).print(next ? "\",\n" : "\"\n");
}

void printSensorData(WiFiClient * client)
{
	(* client).println("{");
	jsonPrint(client, "s1", readings[0], 1);
	jsonPrint(client, "s2", readings[1], 1);
	jsonPrint(client, "s3", readings[2], 1);
	jsonPrint(client, "s4", readings[3], 0);
	(* client).println("}");
}

int getSensorData()
{
	readings[0] = analogRead(sensors[0]);
	readings[1] = analogRead(sensors[1]);
	readings[2] = analogRead(sensors[2]);
	readings[3] = analogRead(sensors[3]);

	return READ_SUCCESS;
}

void loop()
{
	WiFiClient client = server.available();
 
	if (client)
	{
		digitalWrite(DEBUG_LED, HIGH);

		debugPrintln("New client available");
		String lineFeed = "";

		while (client.connected())
		{
			if (client.available())
			{
				// If there's bytes to read from the client then read a byte
				char c = client.read();

				if (DEBUG)
					Serial.write(c);
				if (c == '\n')
				{	// If the current line is blank, there are two \n, which is the end of the client HTTP request, so send a response
					if (lineFeed.length() == 0)
					{
						httpOK(&client);
						// The content of the HTTP response follows the header
	
						// Some content would be here if we made a web page
 
						// The HTTP response ends with another blank line
						client.println();
						break;
					}
					else // If there is a \n, then clear our string
						lineFeed = "";
				}
				// If there is anything in the stream add it to the end of the lineFeed
				else if (c != '\r')
					lineFeed += c;

				if (lineFeed.endsWith("GET /data"))
				{
					debugPrintln("DATA requested");

					jsonOK(&client);
					getSensorData();
					printSensorData(&client);

					client.println();
					digitalWrite(DEBUG_LED, LOW);
					client.stop();
				}
			}
		}

		client.stop();
		debugPrintln("Client disonnected");
	}
}
 
void printWifiStatus()
{
	// Print the SSID of the network we're connected to
	Serial.println("-- SSID --");
	Serial.println(WiFi.SSID());
 
	// Print the Arduino's IP address
	IPAddress ip = WiFi.localIP();
	Serial.println("-- IP Address --");
	Serial.println(ip);
 
	// Print the signal strength
	long rssi = WiFi.RSSI();
	Serial.println("-- Signal Strength (RSSI) --");
	Serial.print(rssi);
	Serial.println(" dBm");

	Serial.print("-- URL --\nhttp://");
	Serial.print(ip);
	Serial.print(":");
	Serial.println(DEFAULT_PORT);
}
