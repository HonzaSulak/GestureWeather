/* Author: Jan Šulák
 * Project: Display MQTT weather station
 * Date: 9.December 2024
 */

#include <SparkFun_APDS9960.h>
#include <WiFi.h>

#include "gesture.h"

// Sensor pins
#define APDS9960_INT 26
#define APDS9960_SDA 21
#define APDS9960_SCL 22

// Display pins
#define SPI_MOSI 23
#define SPI_CLK 18
#define SPI_DC 27
#define SPI_CS 5
#define SPI_RESET 17

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         SPI_MOSI, SPI_CLK, SPI_DC, SPI_RESET, SPI_CS);
SparkFun_APDS9960 apds = SparkFun_APDS9960();
volatile int isr_flag = 0;

// WiFi and MQTT
const char *WIFI_SSID = ""; // Set WiFi name
const char *WIFI_PASS = ""; // Set WiFi password 
const char *MQTT_BROKER = ""; // Set MQTT broker

// States
int currentState = START_STATE;
int currentCity = 0;
bool isReceived = false;

WiFiClient espClient;
PubSubClient client(espClient);
std::string message;
std::string mood = "Neutral";

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("[");
  Serial.print(topic);
  Serial.print("]: ");
  message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  isReceived = true;
  Serial.print(message.c_str());
  Serial.println();
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("user"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, return code=");
      Serial.print(client.state());
      Serial.println(" reattempting in 5 seconds");
      delay(5000);
    }
  }
}

void IRAM_ATTR interruptRoutine()
{
  isr_flag = 1;
}

void handleGesture()
{
  if (apds.isGestureAvailable())
  {
    int gesture = apds.readGesture();
    if (currentState == START_STATE && (gesture == DIR_UP || gesture == DIR_DOWN || gesture == DIR_LEFT || gesture == DIR_RIGHT))
    { // Move from the start screen to the default city screen
      currentState = CITY_STATE;
      currentCity = 0;
      showCityScreen(display, "Brno");
      return;
    }
    switch (gesture)
    {
    case DIR_UP:
      upGesture(message, currentState, currentCity, display, client, mood, isReceived);
      Serial.println("UP");
      break;
    case DIR_DOWN:
      downGesture(message, currentState, currentCity, display, client, mood, isReceived);
      Serial.println("DOWN");
      break;
    case DIR_LEFT:
      leftGesture(currentState, currentCity, display, mood);
      Serial.println("LEFT");
      break;
    case DIR_RIGHT:
      rightGesture(currentState, currentCity, display, mood);
      Serial.println("RIGHT");
      break;
    default: // If the gesture is not a swipe, ignore it
      Serial.println("UNKNOWN");
      break;
    }
  }
}

void setup()
{
  Wire.begin(APDS9960_SDA, APDS9960_SCL);

  pinMode(APDS9960_INT, INPUT_PULLUP);
  Serial.begin(115200);

  // Initialize gesture sensor
  if (apds.init())
  {
    Serial.println(F("APDS-9960 initialization complete"));
  }
  else
  {
    Serial.println(F("Gesture sensor initialization failed!"));
    while (1)
      ; // Halt execution
  }

  if (apds.enableGestureSensor(true))
  {
    Serial.println(F("Gesture sensor running"));
  }
  else
  {
    Serial.println(F("Failed to enable gesture sensor!"));
    while (1)
      ; // Halt execution
  }

  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

  // Initialize SPI display
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 initialization failed!"));
    while (1)
      ; // Halt execution
  }

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("connected");

  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);

  showStartupScreen(display);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (isr_flag == 1)
  { // If the gesture is detected, handle it
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    handleGesture();
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }
}