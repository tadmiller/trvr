// https://www.alanzucconi.com/2015/10/07/how-to-integrate-arduino-with-unity/
// https://medium.com/@yifeiyin/communicationbetween-arduino-and-unity-9fdcccc2be3f
//#include <SerialCommand.h>
#include <ArduinoJson.h>
int DEBUG = 1;

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
	readings["lfl"] = analogRead(leftSensors[0]);

	// Left - Front Right Sensor
	readings["lfr"] = analogRead(leftSensors[1]);

	// Left - Back Sensor
	readings["lb"] = analogRead(leftSensors[2]);

	// Right - Front Left Sensor
	readings["rfl"] = analogRead(rightSensors[0]);

	// Right - Front Right Sensor
	readings["rfr"] = analogRead(rightSensors[1]);

	// Right - Back Sensor
	readings["rb"] = analogRead(rightSensors[2]);

	if (DEBUG)
	{
		String packetData = "";
		readings.printTo(packetData); //Serial, packetData
		Serial.println(packetData);
	}
	else
		detectStance(readings);
/**
	if (Serial.available() > 0)
		sCmd.readSerial();
*/
	// if (DEBUG)
	// 	delay(0);
	// else
	// 	delay(100);
}

void detectStance(JsonObject& readings)
{
	if (readings["lfl"].as<int>() < 100 && readings["lfr"].as<int>() < 100 && readings["lb"].as<int>() < 100)
		stance = NOOP;
	else if (readings["lfl"].as<int>() > 400 && readings["lfr"].as<int>() > 400 && readings["lb"].as<int>() < 100)
		stance = BALLET;
	else if (readings["lfl"].as<int>() > 400 && readings["lfr"].as<int>() > 400 && readings["lb"].as<int>() > 400)
		stance = FLAT;
	else if (readings["lfl"].as<int>() < 100 && readings["lfr"].as<int>() < 100 && readings["lb"].as<int>() > 400)
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

	// Call validatePacket()
	//int sum = checkSum(packet[0:packet["len"]]);
	//if (sum != packetChecksum)
	// if validatePacket()
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

// Size in bytes below for sections
// | Number | Length | Checksum | Data |
// | 3      | 3      | 2        | ?    |
int buildPacket(String data)
{
	char formatStr[5];

	// First 2 bytes are the packet number
	sprintf(formatStr, "%.2d", ++packetNumber);

	String packet(formatStr);

	sprintf(formatStr, "%.4d", data.length());
	packet += formatStr;

	packet += calcSum(packetNumber, data);
	packet += data;
}

// Validate data length
// Validate checksum
// Return 0 if there is an issue, 1 if none
int validatePacket(String data, int len, int num, int checkSum)
{
	return len == data.length() && calcSum(num, data) == checkSum ? 1 : 0;
}

// Takes Packet Number, String
int calcSum(int number, String data)
{
	
	short sum = 0;
	for (int i = 0; i < data.length(); i++)
		sum += data[i];

	sum = (sum ^ -number);
	sum = (sum ^ data.length());
	//sum = sum & (number that makes it only 2 bytes);

	return sum;
	
	// sum = -
	//int sum = -number ^ data.length();
	// sum = sum ^ data;
	//return 1000;
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
