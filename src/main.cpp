#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// network credentials
const char *ssid = "DoubleD";
const char *password = "DoubleD3141";

bool ledOn = false;

// server port
ESP8266WebServer server(80);

// variable for storing http request
String header;

// asign the built-in led pin as output
const int outputLED = LED_BUILTIN;

// millis returns time elasped since device turned on
unsigned long current_time = millis();

// define timeout time (milli)
unsigned long timeout = 2000;

void handleRoot()
{
  digitalWrite(outputLED, HIGH);
  server.send(200, "text/plain", "hello niqqa\r\n");
  digitalWrite(outputLED, LOW);
}

void handleLED()
{
  if (server.method() != HTTP_POST)
  {
    server.send(405, "text/plain", "Method Not Allowed");
  }
  digitalWrite(outputLED, HIGH);
  String message = "POST form was:\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(200, "text/plain", message);
  digitalWrite(outputLED, LOW);
}

void handleLEDGet()
{
  server.send(200, "text/plain", ledOn ? "ON" : "OFF");
}

void handleLEDPost()
{
  String data = server.arg("plain");
  if (data == "ON")
  {
    digitalWrite(outputLED, LOW);
    ledOn = true;
  }
  else if (data == "OFF")
  {
    digitalWrite(outputLED, HIGH);
    ledOn = false;
  }
  else if (data == "TOGGLE")
  {
    digitalWrite(outputLED, ledOn);
    ledOn = !ledOn;
  }
  else
  {
    server.send(400, "text/plain", "request needs to be ON, OFF or TOGGLE!");
    return;
  }
  server.send(201, "text/plain", ledOn ? "ON" : "OFF");
}

void handleLEDBlink()
{
  server.send(200, "text/plain", "BLINKY TIME");
  String data = server.arg("plain");
  int repetitionCount = data.toInt();
  for (int i = 0; i < repetitionCount; i++)
  {
    digitalWrite(outputLED, LOW);
    delay(100);
    digitalWrite(outputLED, HIGH);
    delay(100);
  }
}

void setup()
{
  // init UART port
  Serial.begin(115200);

  pinMode(outputLED, OUTPUT);
  digitalWrite(outputLED, HIGH); // output is inverted 
  ledOn = false;

  // connect to wifi
  Serial.print("connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".\n");
  }
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  // launch server
  server.on("/", handleRoot);
  server.on("/led/state", HTTP_GET, handleLEDGet);
  server.on("/led/state", HTTP_POST, handleLEDPost);
  server.on("/led/blink", HTTP_POST, handleLEDBlink);
  server.begin();
  Serial.println("Server has started!");
}

void loop()
{
  server.handleClient();
}