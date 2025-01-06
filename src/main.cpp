#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const char *mqtt_server = MQTT_SERVER;

const char *mqtt_user = MQTT_USER;
const int mqtt_port = MQTT_PORT;
const char *mqtt_password = MQTT_PASSWORD;
const char *mqtt_topic = "home/emergency_button/state";
const char *mqtt_online_topic = "home/emergency_button/online";

const int buttonPin = 5;
const int ledPin = 8; // Onboard LED pin for ESP32-C3 DevKitC V2
int lastButtonState = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800);

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32C3_Client", mqtt_user, mqtt_password))
    {
      Serial.println("Connected!");
      client.publish(mqtt_online_topic, "true");
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  strip.begin(); // Initialize the NeoPixel library
  strip.show();  // Initialize all pixels to 'off'
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  // Remove the following two lines if you do want to send the current state on boot
  lastButtonState = digitalRead(buttonPin);
  buttonState = lastButtonState;
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      if (buttonState == HIGH)
      {
        Serial.println("Button Pressed");
        client.publish(mqtt_topic, "1"); // Publish pressed state as single press
        strip.setPixelColor(0, strip.Color(200, 0, 0)); // Red
        strip.show();
      }
      else
      {
        Serial.println("Button Released");
        client.publish(mqtt_topic, "L"); // Publish released state as long press
        strip.setPixelColor(0, strip.Color(0, 200, 0)); // Green
        strip.show();
        delay(5000); // 5 seconds delay
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
      }
    }
  }

  lastButtonState = reading;
}

