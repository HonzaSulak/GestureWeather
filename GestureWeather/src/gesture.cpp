/* Author: Jan Šulák
 * Project: Display MQTT weather station
 * Date: 9.December 2024
 */

#include "gesture.h"

const std::vector<std::string> emotions = {"Excited", "Happy", "Neutral", "Sad", "Miserable"};
const std::vector<std::string> city = {"Brno", "Prague", "Ostrava", "Plzen", "Liberec", "Olomouc", "Vienna", "Berlin", "Paris", "London"};

void decreaseMood(std::string &mood)
{
    for (int i = 0; i < emotions.size(); ++i)
    {
        if (emotions[i] == mood)
        {
            mood = (i == 0) ? emotions[0] : emotions[i - 1];
            return;
        }
    }
}

void increaseMood(std::string &mood)
{
    for (int i = 0; i < emotions.size(); ++i)
    {
        if (emotions[i] == mood)
        {
            mood = (i == emotions.size() - 1) ? emotions[emotions.size() - 1] : emotions[i + 1];
            return;
        }
    }
}

void upGesture(std::string &message, int &currentState, int &currentCity, Adafruit_SSD1306 &display, PubSubClient &client, std::string &mood, bool &isReceived)
{
    if (currentState == CITY_STATE)
    {
        currentState = START_STATE;
        showStartupScreen(display);
    }
    else if (currentState == DETAIL_STATE)
    {
        currentState = CITY_STATE;
        showCityScreen(display, city[currentCity]);
    }
    else if (currentState == MOOD_STATE)
    {
        currentState = DETAIL_STATE;
        isReceived = false;
        std::string request = city[currentCity] + " " + mood;
        client.subscribe(city[currentCity].c_str());
        client.publish("requests", request.c_str()); // Send the request to the server in format "city mood"
        Serial.print("[requests]: ");
        Serial.println(request.c_str());
        unsigned long startTime = millis();
        while (!isReceived)
        { 
            display.clearDisplay();
            display.display();
            if (millis() - startTime > 3000)
            {
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 10);
            display.println("CONNECTION");
            u_int16_t x = getCenteredPosition(display, "TIMEOUT");
            display.setCursor(x, 30);
            display.println("TIMEOUT");
            display.display();
            return;
            }
            client.loop();
        }
        showDetailScreen(message, display);
    }
}

void downGesture(std::string &message, int &currentState, int &currentCity, Adafruit_SSD1306 &display, PubSubClient &client, std::string &mood, bool &isReceived)
{
    if (currentState == CITY_STATE)
    {
        currentState = DETAIL_STATE;
        isReceived = false;
        client.subscribe(city[currentCity].c_str());
        client.publish("requests", city[currentCity].c_str()); // Send the request to the server in format "city" only
        Serial.print("[requests]: ");
        Serial.println(city[currentCity].c_str());
        unsigned long startTime = millis();
        while (!isReceived)
        { 
            display.clearDisplay();
            display.display();
            if (millis() - startTime > 3000)
            {
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 10);
            display.println("CONNECTION");
            u_int16_t x = getCenteredPosition(display, "TIMEOUT");
            display.setCursor(x, 30);
            display.println("TIMEOUT");
            display.display();
            return;
            }
            client.loop();
        }
        showDetailScreen(message, display);
    }
    else if (currentState == DETAIL_STATE)
    {
        currentState = MOOD_STATE;
        showMoodScreen(mood, display);
    }
}

void leftGesture(int &currentState, int &currentCity, Adafruit_SSD1306 &display, std::string &mood)
{
    if (currentState == CITY_STATE)
    {
        if (currentCity == 0)
        {
            currentCity = 9;
        }
        else
        {
            currentCity--;
        }
        showCityScreen(display, city[currentCity]);
    }
    else if (currentState == MOOD_STATE)
    {
        decreaseMood(mood);
        showMoodScreen(mood, display);
    }
}

void rightGesture(int &currentState, int &currentCity, Adafruit_SSD1306 &display, std::string &mood)
{
    if (currentState == CITY_STATE)
    {
        if (currentCity == 9)
        {
            currentCity = 0;
        }
        else
        {
            currentCity++;
        }
        showCityScreen(display, city[currentCity]);
    }
    else if (currentState == MOOD_STATE)
    {
        increaseMood(mood);
        showMoodScreen(mood, display);
    }
}
