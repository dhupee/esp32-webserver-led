#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

/*
  create this header file on the 'src' folder, inside add this text:
  #define SECRET_SSID "YourOwnSSID"
  #define SECRET_PASS "YourOwnPassword"
*/
#include "secrets.h"

// Uncomment mode that you want to use
#define STA_MODE
// #define AP_MODE 

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

#ifdef AP_MODE
/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

WebServer server(80); // default port

uint8_t led1Pin = 2; // LED1 pin
bool led1State = LOW;

uint8_t led2Pin = 4; // LED2 pin
bool led2State = LOW;

int blinkAmount = 5; // Number of times to blink the LEDs
int blinkDelay = 500; // Delay between each blink (in milliseconds)

// Web UI
String sendHTML(uint8_t led1Stat, uint8_t led2Stat)
{
    String html = "<!DOCTYPE html> <html>\n";
    html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    html += "<title>LED Control</title>\n";
    html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    html += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    html += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    html += ".button-on {background-color: #3498db;}\n";
    html += ".button-on:active {background-color: #2980b9;}\n";
    html += ".button-off {background-color: #34495e;}\n";
    html += ".button-off:active {background-color: #2c3e50;}\n";
    html += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
    html += "</style>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "<h1>ESP32 Web Server</h1>\n";

    if (led1Stat)
        html += "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
    else
        html += "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

    if (led2Stat)
        html += "<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
    else
        html += "<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";

    html += "<h3>Blink LEDs</h3>\n";
    html += "<form action=\"/blink\" method=\"GET\">\n";
    html += "<label for=\"amount\">Blink Amount:</label>\n";
    html += "<input type=\"number\" id=\"amount\" name=\"amount\" min=\"1\" max=\"10\" value=\"" + String(blinkAmount) + "\">\n";
    html += "<br><br>\n";
    html += "<label for=\"delay\">Blink Delay (ms):</label>\n";
    html += "<input type=\"number\" id=\"delay\" name=\"delay\" min=\"100\" max=\"2000\" step=\"100\" value=\"" + String(blinkDelay) + "\">\n";
    html += "<br><br>\n";
    html += "<input type=\"submit\" value=\"Blink\">\n";
    html += "</form>\n";

    html += "</body>\n";
    html += "</html>\n";
    return html;
}

// Initial connection
void handleRoot()
{
    led1State = LOW;
    led2State = LOW;
    Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleLED1On()
{
    led1State = HIGH;
    Serial.println("GPIO4 Status: ON");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleLED1Off()
{
    led1State = LOW;
    Serial.println("GPIO4 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleLED2On()
{
    led2State = HIGH;
    Serial.println("GPIO5 Status: ON");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleLED2Off()
{
    led2State = LOW;
    Serial.println("GPIO5 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleBlink()
{
    String blinkAmountStr = server.arg("amount");
    String blinkDelayStr = server.arg("delay");

    blinkAmount = blinkAmountStr.toInt();
    blinkDelay = blinkDelayStr.toInt();

    Serial.print("Blink Amount: ");
    Serial.println(blinkAmount);
    Serial.print("Blink Delay: ");
    Serial.println(blinkDelay);

    for (int i = 0; i < blinkAmount; i++) {
        led1State = !led1State;
        led2State = !led2State;
        digitalWrite(led1Pin, led1State);
        digitalWrite(led2Pin, led2State);
        delay(blinkDelay);
    }

    server.send(200, "text/html", sendHTML(led1State, led2State));
}

void handleNotFound()
{
    server.send(404, "text/plain", "Not found");
}

void setup()
{
    Serial.begin(115200);
    delay(100);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);

#ifdef AP_MODE
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println("Access Point Created");
    delay(100);
#endif

#ifdef STA_MODE
    WiFi.begin(ssid, password); // Connect to your local Wi-Fi network
    while (WiFi.status() != WL_CONNECTED) { // Check if Wi-Fi is connected to the network
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
#endif

    server.on("/", handleRoot);
    server.on("/led1on", handleLED1On);
    server.on("/led1off", handleLED1Off);
    server.on("/led2on", handleLED2On);
    server.on("/led2off", handleLED2Off);
    server.on("/blink", handleBlink);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
    digitalWrite(led1Pin, led1State);
    digitalWrite(led2Pin, led2State);
}
