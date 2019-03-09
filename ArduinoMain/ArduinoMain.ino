#include <SPI.h>
#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#define READ_ERR -1
#define READ_SUCCESS 0
#define DEFAULT_PORT 1337
#define DEBUG_LED LED_BUILTIN
const char SSID[] = "DIL";
const char SSID_PW[] = "TheHumanC3nt1p3d3";
const int sensors[4] = {A0, A1, A2, A3};
int readings[4] = {0, 0, 0, 0};

/*
 * WiFi module source: https://github.com/arduino-libraries/WiFi101/blob/master/examples/SimpleWebServerWiFi/SimpleWebServerWiFi.ino
 * Modified to support SD project 
 */

WiFiServer server(DEFAULT_PORT);
 
void setup()
{
	Serial.begin(9600);
	Serial.flush();
	pinMode(DEBUG_LED, OUTPUT);

	// Check that WiFi exists
	if (WiFi.status() == WL_NO_SHIELD)
	{
		Serial.println("NOT PRESENT");
		exit(0);
	}

	Serial.println("Attempting to connect to WiFi");

	// attempt to connect to Wifi network:
	for (int status = WL_IDLE_STATUS; status != WL_CONNECTED; )
		status = WiFi.begin(SSID, SSID_PW);

	delay(100);
	server.begin();                           // start the web server on port 80
	printWifiStatus();                        // you're connected now, so print out the status
	digitalWrite(DEBUG_LED, HIGH);
}

void httpOK(WiFiClient * client)
{
	Serial.println("\nRESPONDING WITH CLIENT OK");
	(* client).println("HTTP/1.1 200 OK");
	(* client).println("Content-type:text/html");
	(* client).println();
}

void printSensorData(WifiClient * client)
{
	client.println("[");
	client.println("\t{");
	client.print("s1: ");
	client.println(readings[0]);
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
		Serial.println("new client");           // print a message out the serial port
		String lineFeed = "";                // make a String to hold incoming data from the client

		while (client.connected())
		{
			if (client.available())
			{ 								// if there's bytes to read from the client
				char c = client.read();		// read a byte, then
				Serial.write(c);			// print it out the serial monitor
				if (c == '\n')
				{							// if the byte is a newline character
											// if the current line is blank, you got two newline characters in a row.
											// that's the end of the client HTTP request, so send a response:
					if (lineFeed.length() == 0)
					{
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						httpOK(&client);
 
						// the content of the HTTP response follows the header:
						client.print("Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>");
						client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");
 
						// The HTTP response ends with another blank line:
						client.println();
						break;
					}
					else // if you got a newline, then clear lineFeed:
						lineFeed = "";
				}
				else if (c != '\r')   	// if you got anything else but a carriage return character,
					lineFeed += c;  	// add it to the end of the lineFeed
 
				// Check to see if the client request was "GET /H" or "GET /L":
				if (lineFeed.endsWith("GET /H"))
				{
					digitalWrite(DEBUG_LED, HIGH);               // GET /H turns the LED on
				}
				if (lineFeed.endsWith("GET /L"))
				{
					digitalWrite(DEBUG_LED, LOW);                // GET /L turns the LED off
				}
				if (lineFeed.endsWith("GET /data"))
				{
					client.println("DATA requested");
					getSensorData();
					printSensorData();
					
					digitalWrite(DEBUG_LED, LOW);                // GET /L turns the LED off
					client.stop();
				}
			}
		}

		client.stop();
		Serial.println("client disonnected");
	}
}
 
void printWifiStatus()
{
	// print the SSID of the network you're attached to:

	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
 
	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
 
	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");

	Serial.print("http://");
	Serial.print(ip);
	Serial.print(":");
	Serial.println(DEFAULT_PORT);
}
