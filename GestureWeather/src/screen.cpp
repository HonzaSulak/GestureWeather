/* Author: Jan Šulák
 * Project: Display MQTT weather station
 * Date: 9.December 2024
 */

#include "screen.h"

void parseMessage(std::string &message, std::string &temperature, std::string &humidity, std::string &mood)
{
    std::string temp = "";
    std::string hum = "";
    std::string md = "";
    int i = 0;
    while (message[i] != ':')
    {
        i++;
    }
    i += 2;
    while (message[i] != ',')
    {
        temp += message[i];
        i++;
    }
    i += 2;
    while (message[i] != ':')
    {
        i++;
    }
    i += 2;
    while (message[i] != ',')
    {
        hum += message[i];
        i++;
    }
    i += 2;
    while (message[i] != ':')
    {
        i++;
    }
    i += 3;
    while (message[i] != '"')
    {
        md += message[i];
        i++;
    }
    temperature = temp;
    humidity = hum;
    mood = md;
}

int16_t getCenteredPosition(Adafruit_SSD1306 &display, std::string item)
{ // Calculate the position to center the text on the screen
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(item.c_str(), 0, 0, &x1, &y1, &w, &h);
    return (128 - w) / 2;
}

void showStartupScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setRotation(2);

    // Title
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.println(F("Weather"));

    display.setCursor(25, 25);
    display.println(F("& Mood"));

    // Subtitle
    display.setTextSize(1);
    display.setCursor(10, 50);
    display.println(F("10 European cities"));

    display.display();
}

void showCityScreen(Adafruit_SSD1306 &display, std::string city)
{
    display.clearDisplay();
    display.setRotation(2);

    display.setTextSize(2);
    display.setCursor(0, 25);
    display.print("<");
    int16_t x = getCenteredPosition(display, city);
    display.setCursor(x, 25);
    display.println(city.c_str());
    display.setCursor(115, 25);
    display.println(">");

    display.display();
}

void showDetailScreen(std::string &message, Adafruit_SSD1306 &display)
{
    std::string temperature;
    std::string humidity;
    std::string mood;
    parseMessage(message, temperature, humidity, mood);
    if (std::stoi(temperature) > 999)
    {
        temperature = "999";
    }
    if (std::stoi(temperature) < -99)
    {
        temperature = "-99";
    }
    if (std::stoi(humidity) > 99)
    {
        humidity = "99";
    }
    if (std::stoi(humidity) < 0)
    {
        humidity = "0";
    }

    display.clearDisplay();
    display.setRotation(2);

    // Temperature
    display.setTextSize(3);
    display.setCursor(0, 10);
    display.print(temperature.c_str());
    display.setTextSize(2);
    display.print((char)247); // Degree symbol
    display.println("C");

    // Humidity
    display.setCursor(80, 10);
    display.setTextSize(3);
    display.print(humidity.c_str());
    display.setTextSize(2);
    display.println("%");

    int16_t x = getCenteredPosition(display, mood);
    display.setCursor(x, 45);
    display.println(mood.c_str());

    display.display();
}

void showMoodScreen(std::string &mood, Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setRotation(2);

    // Title
    display.setTextSize(2);
    display.setCursor(0, 25);
    display.print("<");
    int16_t x = getCenteredPosition(display, mood);
    display.setCursor(x, 25);
    display.println(mood.c_str());
    display.setCursor(115, 25);
    display.println(">");

    display.display();
}
