/* Author: Jan Šulák
 * Project: Display MQTT weather station
 * Date: 9.December 2024
 */

#ifndef GESTURE_H
#define GESTURE_H

#include "screen.h"

void decreaseMood(std::string &mood);
void increaseMood(std::string &mood);

void upGesture(std::string &message, int &currentState, int &currentCity, Adafruit_SSD1306 &display, PubSubClient &client, std::string &mood, bool &isReceived);
void downGesture(std::string &message, int &currentState, int &currentCity, Adafruit_SSD1306 &display, PubSubClient &client, std::string &mood, bool &isReceived);
void leftGesture(int &currentState, int &currentCity, Adafruit_SSD1306 &display, std::string &mood);
void rightGesture(int &currentState, int &currentCity, Adafruit_SSD1306 &display, std::string &mood);

#endif // GESTURE_H