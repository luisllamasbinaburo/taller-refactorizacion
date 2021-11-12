
#include <DHTesp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "YOUR_SSID"; // update your SSID
const char* PASSWORD = "YOUR_PASSWORD"; // update your SSID password
const char* HOST = "192.168.0.XX"; //not needed immediately, but suggest once known it is updated here and the IP address is fixed in your router
#define SwitchPin 14 // interrupt 0 Pin 2 in Arduino is a must due to interrupts
#define ClockPin  13 // interrupt 1 Pin 3 in Arduino is a must due to interrupts
#define DataPin   12 // this can be any pin
volatile int EncodeCTR;
volatile int EncoderChange;
volatile int SwitchCtr;
int setpoint;
int ledPin = 16;
int insteonstatus;
int val = 0;       // variable to store the value coming from the sensor

// Parameters for counting readings
int numReadings = 10;
int readings[10];
int counter;
//

WiFiServer server(303); //just pick any port number you like
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Set the LCD I2C address. Amend to suit your LCD

#define DHTPIN 2  
#define DHTTYPE DHT11
DHTesp dht;
uint8_t temp1, humidity;


void Switch()
{
	static unsigned long DebounceTimer;
	if((unsigned long)(millis() - DebounceTimer) >= (100))
	{
		DebounceTimer = millis();
		if(!SwitchCtr)
		{
			SwitchCtr++;
			EncodeCTR = 19; // turns this parameter to 19 when the encoder button is pushed
		}
	}
}

void Encode()
{ // we know the clock pin is low so we only need to see what state the Data pin is and count accordingly
	static unsigned long DebounceTimer;
	if((unsigned long)(millis() - DebounceTimer) >= (100))
	{ // standard blink without delay timer
		DebounceTimer = millis();
		if(digitalRead(DataPin) == LOW) // switch to LOW to reverse direction of Encoder counting
		{
			EncodeCTR++;
		}
		else
		{
			EncodeCTR--;
		}
		EncoderChange++;
	}
}

// IMPORTANT NOTE: This code integrates with Insteon on/off module. If another method is used to turn the heater/boiler ON/OFF then this section section will need to be modified to suit the needs (e.g. IoT relays, Vera, etc.)
void InsteonON()
{
	HTTPClient http;    //Declare object of class HTTPClient

	http.begin("http://192.168.0.XX:XXX/3?02621FF5870F11FF=I=3");  //ON for Insteon controller. Amend to suit your system configuration
	http.addHeader("Content-Type", "text/plain");  //Specify content-type header

	int httpCode = http.POST("Message from ESP8266");   //Send the request
	String payload = http.getString();                  //Get the response payload

	Serial.println(httpCode);   //Print HTTP return code
	Serial.println(payload);    //Print request response payload

	http.end();  //Close connection
}

void InsteonOFF()
{
	HTTPClient http2;    //Declare object of class HTTPClient

	http2.begin("http://192.168.0.XX:XXXX/3?02621FF5870F13FF=I=3");      //OFF for Insteon controller. Amend to suit your system configuration
	http2.addHeader("Content-Type", "text/plain");  //Specify content-type header

	int http2Code = http2.POST("Message from ESP8266");   //Send the request
	String payload2 = http2.getString();                  //Get the response payload

	Serial.println(http2Code);   //Print HTTP return code
	Serial.println(payload2);    //Print request response payload

	http2.end();  //Close connection;
}

void setup()
{
	Serial.begin(115200);
	delay(10);

	// reading counter
	counter = 0;
	//
	Serial.println(WiFi.localIP());
	// Encoder setup
	pinMode(SwitchPin, INPUT_PULLUP); // switch is not powered by the + on the Encoder breakout
	pinMode(ClockPin, INPUT);
	pinMode(DataPin, INPUT);
	// Prepare LED Pin
	pinMode(ledPin, OUTPUT);
	digitalWrite(16, LOW);
	attachInterrupt(digitalPinToInterrupt(SwitchPin), Switch, FALLING);
	attachInterrupt(digitalPinToInterrupt(ClockPin), Encode, FALLING);
	// Connect to WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(SSID);
	WiFi.begin(SSID, PASSWORD);
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

	// Start the server
	server.begin();
	Serial.println("Server started");

	// Print the IP address
	Serial.println(WiFi.localIP());

	// initialize the lcd
	lcd.begin(16, 2);
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.print("Room Temp:");
	lcd.setCursor(0, 1); // go to next line
	lcd.print("Setpoint:");

	dht.setup(11, DHTesp::DHT11);;
	delay(10);
}

void loop()
{
	if(EncoderChange || SwitchCtr)
	{
		EncoderChange = 0;
		Serial.print("EncodeCTR: ");
		Serial.print(EncodeCTR);
		Serial.println();
		SwitchCtr = 0;
	}

	// get temperature readings
	int temp1 = dht.getTemperature();
	humidity = dht.getHumidity();
	//delay(5000);

	// Calculate stable temp readings
	int value = temp1;
	readings[counter] = value;
	counter++;
	if(counter >= numReadings) counter = 0;

	//running average. This gives the readings more stability
	float total = 0;
	for(int i = 0; i < numReadings; i++) total += readings[i];
	float temperature = total / numReadings;
	//

	setpoint = EncodeCTR;

	// print average values
	Serial.print("\tAVG:\t");
	Serial.println(temperature, 0);  // 0 decimals
	delay(1000); // this delay is key to slow the readings! Without it, you get multiple readings per second 

  // print values to lcd 
	lcd.setCursor(11, 0);
	lcd.print(temperature, 0); // read average instead of temperature for stability
	lcd.print(" C ");
	lcd.setCursor(11, 1);
	lcd.print(setpoint);
	lcd.print(" C ");

	// Temperature tests start here to decide when to turn the heater on/off
	if(temperature <= setpoint - 1)
	{ // condition true if measured temp below xx degrees under setpoint temp
		if(insteonstatus != 1)
		{  // if status is not ON the turn on the device, if not do nothing
			InsteonON();
			digitalWrite(ledPin, HIGH); // turn on the LED - for some reason, LOW turns it on on this script
			delay(500);
			insteonstatus = 1;
		}
	}
	if(temperature >= setpoint + 1)
	{  // condition true if measured temp below xx degrees over setpoint temp
		if(insteonstatus != 0)
		{ // if status is not OFF the turn off the device, if not do nothing
			InsteonOFF();
			digitalWrite(ledPin, LOW); // turn OFF the LED
			delay(500);
			insteonstatus = 0;
		}
	}
	// delay(500);

   // IoT loop Check if a client has connected

	WiFiClient client = server.available();
	if(!client)
	{
		return;
	}

	// Wait until the client sends some data
	while(!client.available())
	{
		delay(1);
	}

	// Read the first line of the request
	String req = client.readStringUntil('\r');
	client.flush();

	// Match the request

	if(req.indexOf("") != -10)
	{  //checks if you're on the main page

		if(req.indexOf("/+") != -1)
		{ //checks if you clicked plus
			EncodeCTR = EncodeCTR + 1;
			Serial.println("You clicked +");
			Serial.println(EncodeCTR);
		}
		if(req.indexOf("/-") != -1)
		{ //checks if you clicked minus
			EncodeCTR = EncodeCTR - 1;
			Serial.println("You clicked -");
			Serial.println(EncodeCTR);
		}
		if(req.indexOf("/ON") != -1)
		{ //checks if you clicked minus
			EncodeCTR = temperature + 2;
			Serial.println("You clicked Boiler On");
			Serial.println(EncodeCTR);
		}
		if(req.indexOf("/OFF") != -1)
		{ //checks if you clicked minus
			EncodeCTR = 19; // resent the temp to 19
			Serial.println("You clicked Boiler Off");
			Serial.println(EncodeCTR);
		}
	}

	else
	{
		Serial.println("invalid request");
		client.stop();
		return;
	}
	// Publish buttons on internet browser via Web Server. Simply type IP address of device and port on URL bar- e.g. 192.168.0.2:443
	  // Prepare the response
	String s = "HTTP/1.1 200 OK\r\n";
	s += "Content-Type: text/html\r\n\r\n";
	s += "<!DOCTYPE HTML>\r\n<html>\r\n";
	s += "<p>Setpoint Temperature <a href='/+'\"><button>+</button></a>&nbsp;<a href='/-'\"><button>-</button></a></p>";
	s += "<p>Boiler Status <a href='/ON'\"><button>ON</button></a>&nbsp;<a href='/OFF'\"><button>OFF</button></a></p>";
	client.flush();
	// Send the response to the client
	client.print(s);
	delay(1);

	client.println();
	client.print("Room Temperature = ");
	client.println(temperature);
	client.println();
	client.print("Setpoint = ");
	client.println(setpoint);
	delay(1);
}
