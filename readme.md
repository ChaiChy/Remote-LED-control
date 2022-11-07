# LED beebop

For the final part of this project the built-in LED of the WeMos D1 R1 is controlled using the aforementioned alexa voice command.

## table of contents 
1. [WeMos D1 R1 Board](#wemos-d1-r1)
2. [Prerequisities](#prerequisities)
3. [Execution](#execution)

## WeMos D1 R1 
The WeMos boards are widely considered to be the cheaper alternatives to the popular Arduino microcontrollers. Like Arduino, the WeMos boards can too be programmed using the Arduino framework, making it easy to work with. The board we are using for this particular project is a WeMos D1 R1. It uses the ESP8266 WiFi microchip and can be used for hosting webserver, therefore it is able to receive http requests, making it the perfect chip for our project.

![image](/images/WeMos-d1-mini-Pin-out.png)

## Prerequisities
This part of the project is written in C++ with the Arduino framework. We use the Vscode extension _platformIO_ for ease of compiling and flashing. 
- [platformio](https://platformio.org/) 

## Execution
The source code is contained within the folder _src_ in file _main.cpp_. The code is split into 3 parts for ease of explaination:

1. [Hosting webserver](#hosting-webserver)
2. [Handler functions](#handler-functions)

### Hosting webserver 
For the board to receive http requests it must first be connected to the internet. This is done simply by calling _WiFi.begin_:
```
// network credentials
const char *ssid = "enter your wifi name here";
const char *password = "enter your password here";

WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
    delay(500);
    Serial.print(".\n");
}
Serial.println("WiFi connected!");
Serial.print("IP Address: ");
Serial.print(WiFi.localIP());
```
Note that we are using the UART to monitor if the board's progression as well as printing the IP Address. The set up UART simply use the function
```
Serial.begin(Baud rate) //115200 is the common baud rate used for 
                        //most uart interface. Can be set in 
                        //platformio.ini
```

After WiFi connection is established, simply copy the IP address and paste it in your Internet browser, you should be able to see your webserver running.

To start the server use the function _server.begin()_.

### Handler functions
The handler functions are called to process specific HTTP POST or GET requests. They are assigned using the _server.on_ function:

```
server.on("/", handleRoot);
server.on("/led/state", HTTP_GET, handleLEDGet);
server.on("/led/state", HTTP_POST, handleLEDPost);
server.on("/led/blink", HTTP_POST, handleLEDBlink);
```

With the exception of [handleRoot](#handleroot), [handleLEDGet](#handleledget), [handleLEDPost](#handleledpost) and [handleLEDBlink](#handleledblink) are triggered by HTTP requests.

#### handleRoot()
```
void handleRoot()
{
  server.send(200, "text/plain", "hello m8\r\n");
}
```
handleRoot is called whenever the webserver is in its root page, ie the _home page_. It simply prints "hello m8" on the webpage.

#### handleLEDGet()
```
void handleLEDGet()
{
  server.send(200, "text/plain", ledOn ? "ON" : "OFF");
}
```
handleLEDGet is called in response to a HTTP GET request to "/led/state". It returns the the current state of the built-in LED in plain text "ON" or "OFF".

#### handleLEDPOST()
```
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
```
handleLEDPost in called in response to a HTTP POST request to "/led/state". It first reads the argument specified in the post request. If the argument is "ON", the built-in LED on the is switched on using digitalWrite() and vice versa. If the argument is "TOGGLE", the output of the led is inverted. Finally the server sends the HTTP response code 201 to indicate that request has been acknowledge and the current output of the led is stated.

#### handleLEDBlink
```
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
```
handleLEDBlink is called in response to a HTTP POST request to "/led/blink". It reads the argument of the HTTP POST request (in integer). The built-in LED will blink in an amount of times corresponding to the argument.