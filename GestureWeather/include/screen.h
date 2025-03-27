/* Author: Jan Šulák
 * Project: Display MQTT weather station
 * Date: 9.December 2024
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <string>
#include <vector>

enum states
{
  START_STATE,
  CITY_STATE,
  DETAIL_STATE,
  MOOD_STATE
};

void parseMessage(std::string &message, std::string &temperature, std::string &humidity, std::string &mood);
int16_t getCenteredPosition(Adafruit_SSD1306 &display, std::string item);

void showStartupScreen(Adafruit_SSD1306 &display);
void showCityScreen(Adafruit_SSD1306 &display, std::string city);
void showDetailScreen(std::string &message, Adafruit_SSD1306 &display);
void showMoodScreen(std::string &mood, Adafruit_SSD1306 &display);

#endif // SCREEN_H