// https://www.alanzucconi.com/2015/10/07/how-to-integrate-arduino-with-unity/
// https://medium.com/@yifeiyin/communication-between-arduino-and-unity-9fdcccc2be3f
//#include <SerialCommand.h>
#include <ArduinoJson.h>
#define DEBUG 0

enum STANCES {
	NOOP,
	FLAT,
	HEELS,
	BALLET,
	LEN
};

int stanceReadings[LEN][6];

int leftSensors[3] = {A0, A1, A2};
int rightSensors[3] = {A3, A4, A5};

int pressure = A0;

int led0 = 9;
int led1 = 10;
int led2 = 11;

int buttonState = 0;

short packetNumber = 0;
byte stance = 0;
byte prevStance = -1;

//StaticJsonBuffer<200> jsonBuffer;
//JsonObject json = jsonBuffer.createObject();
//SerialCommand sCmd;

void setup()
{
	Serial.begin(9600);

	pinMode(4, INPUT_PULLUP);

	Serial.println("Begin");
}

void loop()
{
	buttonState = digitalRead(4);

	Serial.flush();

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& readings = jsonBuffer.createObject();

	// Left - Front Left Sensor
	readings["l-fl"] = analogRead(leftSensors[0]);

	// Left - Front Right Sensor
	readings["l-fr"] = analogRead(leftSensors[1]);

	// Left - Back Sensor
	readings["l-b"] = analogRead(leftSensors[2]);

	// Right - Front Left Sensor
	readings["r-fl"] = analogRead(rightSensors[0]);

	// Right - Front Right Sensor
	readings["r-fr"] = analogRead(rightSensors[1]);

	// Right - Back Sensor
	readings["r-b"] = analogRead(rightSensors[2]);

	if (DEBUG)
	{
		String packetData = "\n";
		readings.printTo(packetData); //Serial, packetData
		Serial.println(packetData);
	}
	else
		detectStance(readings);
/**
	if (Serial.available() > 0)
		sCmd.readSerial();
*/
	if (DEBUG)
		delay(1000);
	else
		delay(100);
}

void detectStance(JsonObject& readings)
{
	if (readings["l-fl"].as<int>() < 100 && readings["l-fr"].as<int>() < 100 && readings["l-b"].as<int>() < 100)
		stance = NOOP;
	else if (readings["l-fl"].as<int>() > 400 && readings["l-fr"].as<int>() > 400 && readings["l-b"].as<int>() < 100)
		stance = BALLET;
	else if (readings["l-fl"].as<int>() > 400 && readings["l-fr"].as<int>() > 400 && readings["l-b"].as<int>() > 400)
		stance = FLAT;
	else if (readings["l-fl"].as<int>() < 100 && readings["l-fr"].as<int>() < 100 && readings["l-b"].as<int>() > 400)
		stance = HEELS;


	if (stance != prevStance)
	{
		prevStance = stance;

		if (stance == NOOP)
			Serial.println("No reading");
		else if (stance == BALLET)
			Serial.println("Tightroping");
		else if (stance == FLAT)
			Serial.println("Flat feet");
		else if (stance == HEELS)
			Serial.println("Heels only");
	}
}

void calibrate()
{
	Serial.println("This will run a calibration routine \
		to establish specialized readings for different stances \
	  \nPlease ensure the device is completely attached to your foot \
		before continuing." );

	Serial.println("Hit any key and enter to continue...");
	while(!Serial.available()) {}

	Serial.println("Please set your feet lightly flat on the ground, \
		like you are standing. \
	  \nHit any key and enter when ready for a reading...");
	while(!Serial.available()) {}

	for (int i = A0; i < A6; i++)
		stanceReadings[FLAT][0] = analogRead(i);

	Serial.println("Please lift the back of your feet and balance, \
		only on your toes. \
	  \nHit any key and enter when ready for a reading...");
	while(!Serial.available()) {}

	for (int i = A0; i < A6; i++)
		stanceReadings[BALLET][0] = analogRead(i);

	Serial.println("Please lift the front of your feet and balance, \
		only on your heels. \
	  \nHit any key and enter when ready for a reading...");
	while(!Serial.available()) {}

	for (int i = A0; i < A6; i++)
		stanceReadings[HEELS][0] = analogRead(i);
}

// Takes a serial String and deconstructs the packet
// to check for validity, then executes the command
String getCMD(String packet)
{
	// De-JSONify packet

	//int sum = checkSum(packet[0:packet["len"]]);
	//if (sum != packetChecksum)
		// Request retransmission of packet
		// return NULL;
	//return packet["data"];

	return NULL;
}

// Reads the Serial data stream and calls getCMD()
// Then executes the specific command returned
void commandHandler()
{
	// String cmd = Serial.Readline();
}

// Returns a reading from each sensor on
// the left foot in a JSONified object.
String getLeftFoot()
{
	return NULL;
}

// Returns a reading from each sensor on
// the right foot in a JSONified object.
String getRightFoot()
{
	return NULL;
}

// Returns the status of the Arduino in a JSONified
// object. Status 1 means working, 0 means error.
String getArduinoStatus()
{
	return NULL;
}

// Returns the status of each sensor in a JSONified
// object. Status 1 means working, 0 means error. 
String getSensorStatus()
{
	return NULL;
}

int buildPacket(String data)
{
	char formatStr[5];

	// First 2 bytes are the packet number
	sprintf(formatStr, "%.2d", ++packetNumber);

	String packet(formatStr);

	sprintf(formatStr, "%.4d", data.length());
	packet += formatStr;

	packet += checkSum(packetNumber, data);
	packet += data;
}

int checkSum(int number, String data)
{
	int sum = -number ^ data.length();
	// sum = sum ^ data;
	return 1000;
}

/*
void pingHandler()
{
  Serial.println("COMMUNICATION SUCCESSFUL");
}


void echoHandler ()
{
	char *arg;
	arg = sCmd.next();

	if (arg != NULL)
		Serial.println(arg);
//	if (arg == "pressure")
//		Serial.println(json.toString());``
	else
		Serial.println("nothing to echo");
}

arg errorHandler (const char *command)
{
	Serial.println("COMMUNICATION ERROR");
}
*/