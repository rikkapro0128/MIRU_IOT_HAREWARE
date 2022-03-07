// #include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncTCP.h"
#include "Ticker.h"

#define led 2

unsigned long myChannelNumber = 1668435;
const char *myWriteAPIKey = "NRIB32A567CEC56I";
const char *host = "api.thingspeak.com";

// open serve on port 80
WiFiClient client;
AsyncWebServer server(80);
Ticker scheduleBlinkLed;
Ticker scheduleRandomSeed;

// define function global
void ledBlink(int pin);
void connectWifi(String wifiName, String password);
void initServer();
void initRouter();
void setupPinState();
void addTask();
void sendDataToThinkspeak(int data);

void setup()
{
  // put your setup code here, to run once:
  setupPinState();
  Serial.begin(115200);
  Serial.println();
  connectWifi("duy123", "44448888");
  initServer();
  addTask();
  ThingSpeak.begin(client);
}

void loop()
{
  // put your main code here, to run repeatedly:
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

void addTask()
{
  scheduleBlinkLed.attach_ms(500, std::bind(ledBlink, led));
  scheduleRandomSeed.attach_ms(5000, []()
                               {
    int num = random(10, 50);
    Serial.print("Sending Data = ");
    Serial.println(num);
    if (WiFi.status() == WL_CONNECTED)
    {
      // send data to Thinkspeak
      sendDataToThinkspeak(num);
    } });
}

void sendDataToThinkspeak(int data)
{
  // WiFiClient client;
  // HTTPClient http;
  // if(http.begin(client, String("http://api.thingspeak.com/update.json"))) {
  //   http.addHeader("Content-Type", "application/json");
  //   int httpCode = http.POST(String("{\"api_key\":\"Y9WTFH0OPO3Z5KZS\",\"field1\":\"45\"}"));
  //   // Serial.println(String("http://api.thingspeak.com/update?api_key=" + myWriteAPIKey + "&field1=" + data));
  //   Serial.println("============== Response code: " + String(httpCode));
  //   if (httpCode > 0) {
  //     Serial.println(http.getString());
  //   }
  //   http.end();  //Close connection
  // }else {
  //   Serial.printf("[HTTP] Unable to connect\n");
  // }
  ThingSpeak.setField(1, data);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
