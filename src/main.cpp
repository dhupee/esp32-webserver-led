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

const char* ssid = SECRET_SSID; // Your WiFi network SSID
const char* password = SECRET_PASS; // Your WiFi network password

#ifdef AP_MODE
/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1); // IP address of the access point
IPAddress gateway(192, 168, 1, 1); // Gateway IP address
IPAddress subnet(255, 255, 255, 0); // Subnet mask
#endif

WebServer server(80); // Create a WebServer object listening on port 80 (default HTTP port)

uint8_t led1Pin = 2; // Pin number for LED1
bool led1State = LOW; // Initial state of LED1 (OFF)

uint8_t led2Pin = 4; // Pin number for LED2
bool led2State = LOW; // Initial state of LED2 (OFF)

int blinkAmount = 5; // Number of times to blink the LEDs
int blinkDelay = 500; // Delay between each blink (in milliseconds)

// Function to generate and return the HTML page for controlling LEDs
String sendHTML(uint8_t led1Stat, uint8_t led2Stat)
{
    String html = "<!DOCTYPE html> <html>\n";
    // HTML head
    html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    html += "<title>LED Control</title>\n";
    // CSS styles
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
    // Page title
    html += "<h1>ESP32 Web Server</h1>\n";

    // LED1 control
    if (led1Stat)
        html += "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
    else
        html += "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

    // LED2 control
    if (led2Stat)
        html += "<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
    else
        html += "<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";

    // Blink LEDs form
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

// Handler for the root URL ("/")
void handleRoot()
{
    led1State = LOW; // Set LED1 state to OFF
    led2State = LOW; // Set LED2 state to OFF
    Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State)); // Send the HTML page to the client
}

// Handler for turning LED1 ON
void handleLED1On()
{
    led1State = HIGH; // Set LED1 state to ON
    Serial.println("GPIO4 Status: ON");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

// Handler for turning LED1 OFF
void handleLED1Off()
{
    led1State = LOW; // Set LED1 state to OFF
    Serial.println("GPIO4 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

// Handler for turning LED2 ON
void handleLED2On()
{
    led2State = HIGH; // Set LED2 state to ON
    Serial.println("GPIO5 Status: ON");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

// Handler for turning LED2 OFF
void handleLED2Off()
{
    led2State = LOW; // Set LED2 state to OFF
    Serial.println("GPIO5 Status: OFF");
    server.send(200, "text/html", sendHTML(led1State, led2State));
}

// Handler for blinking the LEDs
void handleBlink()
{
    String blinkAmountStr = server.arg("amount");
    String blinkDelayStr = server.arg("delay");

    blinkAmount = blinkAmountStr.toInt(); // Convert string to integer
    blinkDelay = blinkDelayStr.toInt(); // Convert string to integer

    Serial.print("Blink Amount: ");
    Serial.println(blinkAmount);
    Serial.print("Blink Delay: ");
    Serial.println(blinkDelay);

    for (int i = 0; i < blinkAmount; i++) {
        led1State = !led1State; // Toggle LED1 state
        led2State = !led2State; // Toggle LED2 state
        digitalWrite(led1Pin, led1State);
        digitalWrite(led2Pin, led2State);
        delay(blinkDelay);
    }

    server.send(200, "text/html", sendHTML(led1State, led2State));
}

// Handler for handling requests to unknown URLs
void handleNotFound()
{
    server.send(404, "text/html, ", "Not found <a href='/'>home</a>");
}

void setup()
{
    Serial.begin(115200);
    delay(100);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);

#ifdef AP_MODE
    WiFi.softAP(ssid, password); // Start the Access Point with the specified SSID and password
    WiFi.softAPConfig(local_ip, gateway, subnet); // Set the IP address, gateway, and subnet mask for the Access Point
    Serial.println("Access Point Created");
    delay(100);
#endif

#ifdef STA_MODE
    WiFi.begin(ssid, password); // Connect to your local Wi-Fi network
    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi connection
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP()); // Print the assigned local IP address
#endif

    // Set up request handlers
    server.on("/", handleRoot); // ("/")
    server.on("/led1on", handleLED1On); // ("/led1on")
    server.on("/led1off", handleLED1Off);
    server.on("/led2on", handleLED2On);
    server.on("/led2off", handleLED2Off);
    server.on("/blink", handleBlink); // ("/blink?amount=10&delay=100")

    server.onNotFound(handleNotFound);

    server.begin(); // Start the HTTP server
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient(); // Handle incoming client requests
    digitalWrite(led1Pin, led1State); // Update LED1 state
    digitalWrite(led2Pin, led2State); // Update LED2 state
}
