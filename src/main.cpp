// #include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncTCP.h"
#include "Ticker.h"

#define led 2

unsigned long myChannelNumber = 1668515;
const char *myWriteAPIKey = "AWWM4LJMFYKWAUWC";
const char *host = "api.thingspeak.com";

// open serve on port 80
WiFiClient client;
AsyncWebServer server(80);

// define function global
void ledBlink(int pin);
void connectWifi(String wifiName, String password);
void initServer();
void initRouter();
void setupPinState();
void sendDataToThinkspeak();

Ticker scheduleBlinkLed(std::bind(ledBlink, led), 500);
Ticker scheduleRandomSeed(sendDataToThinkspeak, 5000);

void setup()
{
  // put your setup code here, to run once:
  setupPinState();
  Serial.begin(115200);
  Serial.println();
  connectWifi("duy123", "44448888");
  initServer();
  ThingSpeak.begin(client);
  scheduleBlinkLed.start();
  scheduleRandomSeed.start();
}

void loop()
{
  // put your main code here, to run repeatedly:
  scheduleBlinkLed.update();
  scheduleRandomSeed.update();
}

// define function global

void setupPinState()
{
  pinMode(led, OUTPUT);
}

void ledBlink(int pin)
{
  digitalWrite(pin, !digitalRead(pin));
}

void connectWifi(String wifiName, String password)
{
  WiFi.begin(wifiName, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
}

void initServer()
{
  // install router server
  initRouter();
  server.begin();
}

void initRouter()
{
  // define request by method GET
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Received request!");
    request->send(200, "text/plain", String("Welcome Project ESP8266 Miru!")); });
}

void sendDataToThinkspeak()
{
  int num = random(10, 50);
  // Serial.print("Sending Data = ");
  // Serial.println(num);
  if (WiFi.status() == WL_CONNECTED)
  {
    // send data to Thinkspeak
    ThingSpeak.setField(1, num);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
}
