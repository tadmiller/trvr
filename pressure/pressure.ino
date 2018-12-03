// https://www.alanzucconi.com/2015/10/07/how-to-integrate-arduino-with-unity/
// https://medium.com/@yifeiyin/communication-between-arduino-and-unity-9fdcccc2be3f
//#include <SerialCommand.h>
#include <ArduinoJson.h>

int leftSensors[3] = {A0, A1, A2};
int rightSensors[3] = {A3, A4, A5};

int pressure = A0;

int led0 = 9;
int led1 = 10;
int led2 = 11;

int buttonState = 0;

//StaticJsonBuffer<200> jsonBuffer;
//JsonObject json = jsonBuffer.createObject();
//SerialCommand sCmd;

void setup()
{
/**	while (!Serial);

	sCmd.addCommand("PING", pingHandler);
	sCmd.addCommand("ECHO", echoHandler);
	sCmd.setDefaultHandler(errorHandler);
*/
	Serial.begin(9600);

	pinMode(4, INPUT_PULLUP);
	pinMode(led0, OUTPUT);
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);

	Serial.println("Begin");
}

void loop()
{
	buttonState = digitalRead(4);

	Serial.flush();

	if (buttonState == LOW)
	{
		Serial.println(1);
	}
	else
	{
		Serial.println(0);
	}

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& readings = jsonBuffer.createObject();
	readings["l0"] = analogRead(leftSensors[0]);
	readings["l1"] = analogRead(leftSensors[1]);
	readings["l2"] = analogRead(leftSensors[2]);

	readings["r0"] = analogRead(rightSensors[0]);
	readings["r1"] = analogRead(rightSensors[1]);
	readings["r2"] = analogRead(rightSensors[2]);
	readings.printTo(Serial);

/**	json["lPressure0"] = p0;
	json["lPressure1"] = p1;
	json["lPressure2"] = p2;
	json["lPressure3"] = p3;
*/
/**
	Serial.print("p0: ");
	Serial.println(p0);
	Serial.println(p1);
	Serial.println(p2);
	
	analogWrite(led0, p0 / 3);
	analogWrite(led1, p1 / 3);
	analogWrite(led2, p2 / 3);

	if (Serial.available() > 0)
		sCmd.readSerial();
*/
	delay(50);
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