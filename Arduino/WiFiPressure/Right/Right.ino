/*
 * # Author(s)
 * Tad Miller & Danny Nsouli
 *
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
int DEBUG = 1;

const char SSID[] = "DIL";
const char SSID_PW[] = "TheHumanC3nt1p3d3";
int readings[4] = {0, 0, 0, 0};

WiFiServer server(DEFAULT_PORT);
 
void setup()
{
	digitalWrite(DEBUG_LED, HIGH);
	delay(1000);

	Serial.begin(9600);
	Serial.flush();

	pinMode(DEBUG_LED, OUTPUT);
	debugPrintln("Starting routine...");

	// Check that WiFi exists
	if (WiFi.status() == WL_NO_SHIELD)
	{
		Serial.println("NOT PRESENT");

		// Status indication that the Arduino is failing
		bool debug = false;
		for (int i = 0; i < 99; i++)
		{
			digitalWrite(DEBUG_LED, debug ? HIGH : LOW);
			debug = !debug;
			delay(1000);
		}

		exit(0);
	}

	debugPrintln("Attempting to connect to WiFi");

	// Attempt to connect to Wifi network:
	for (int status = WL_IDLE_STATUS; status != WL_CONNECTED; )
		status = WiFi.begin(SSID, SSID_PW);

	WiFi.hostname("trvr-right");

	// Start the web server on port 80
	server.begin();
	printWifiStatus();
	digitalWrite(DEBUG_LED, LOW);
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

// Perform HTTP 200 OK and indicate a JSON packet is being sent
void jsonOK(WiFiClient * client)
{
	(* client).println("HTTP/1.1 200 OK");
	(* client).println("Content-type:application/json");
	(* client).println();
}

// Print the key/value in a JSON format
void jsonPrint(WiFiClient * client, String key, int val, int next)
{
	(* client).print("\t\"");
	(* client).print(key);
	(* client).print("\": \"");
	(* client).print(val);
	(* client).print(next ? "\",\n" : "\"\n");
}

// Print out the sensor readings using JSON
void printSensorData(WiFiClient * client)
{
	(* client).println("{");
	jsonPrint(client, "front", readings[2], 1);
	jsonPrint(client, "back", readings[0], 1);
	jsonPrint(client, "midl", readings[3], 1);
	jsonPrint(client, "midr", readings[1], 0);
	(* client).println("}");
}

// Store the sensor readings in an array
int getSensorData()
{
	readings[0] = analogRead(A1);
	readings[1] = analogRead(A2);
	readings[2] = analogRead(A3);
	readings[3] = analogRead(A4);

	return READ_SUCCESS;
}

void loop()
{
	WiFiClient client = server.available();
 
	if (client)
	{
		digitalWrite(DEBUG_LED, HIGH);

		// Accept client
		debugPrintln("New client available");
		String lineFeed = "";

		while (client.connected())
		{	// Print JSON data over web to client
			if (client.available())
			{
				debugPrintln("Printing JSON data");

				jsonOK(&client);
				getSensorData();
				printSensorData(&client);

				client.println();
				digitalWrite(DEBUG_LED, LOW);
				client.stop();
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
