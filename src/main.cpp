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
const int ledPin = 8;
int lastButtonState = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long ledOffTime = 0;
bool ledOn = false;
bool testCompleted = false;
bool testPressed = false;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800);

void setLedColor(uint8_t r, uint8_t g, uint8_t b, int duration = 0)
{
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
  if (duration > 0)
  {
    ledOffTime = millis() + duration;
    ledOn = true;
  }
  else
  {
    ledOn = false;
  }
}

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    setLedColor(255, 255, 0); // Yellow for Wi-Fi failure
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
  setLedColor(0, 0, 0);
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
      if (!testCompleted)
      {
        // Back to test mode
        testPressed ? setLedColor(0, 50, 255) : setLedColor(0, 0, 255);
      }
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");

      setLedColor(128, 0, 128, 3000); // Purple for MQTT connection failure
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  strip.begin();
  strip.show();
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  lastButtonState = digitalRead(buttonPin);
  buttonState = lastButtonState;

  setLedColor(0, 0, 255); // Blue for test indication
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

      if (!testCompleted)
      {
        if (buttonState == HIGH)
        {
          testPressed = true; // Button is pressed
          Serial.println("Button pressed, test started.");
          setLedColor(0, 255, 255);
        }
        else if (testPressed && buttonState == LOW)
        {
          testCompleted = true; // Button released, test completed
          setLedColor(0, 0, 0); 
          Serial.println("Test completed. Sending enabled.");
        }
      }
      else
      {
        if (buttonState == HIGH)
        {
          Serial.println("Button Pressed");
          if (!client.publish(mqtt_topic, "1")) // Single Press
          {
            setLedColor(128, 0, 128, 3000); // Purple if data publish fails
          }
          else
          {
            setLedColor(200, 0, 0); // Red
          }
        }
        else
        {
          Serial.println("Button Released");
          if (!client.publish(mqtt_topic, "L")) // Long Press
          {
            setLedColor(128, 0, 128, 3000); // Purple if data publish fails
          }
          else
          {
            setLedColor(0, 200, 0, 5000); // Green for 5 seconds
          }
        }
      }
    }
  }

  if (ledOn && millis() > ledOffTime)
  {
    strip.setPixelColor(0, strip.Color(0, 0, 0)); 
    strip.show();
    ledOn = false;
  }

  lastButtonState = reading;
}
