/* Author: Jan Šulák
 * Description: MQTT client that fetches weather data from OpenWeather API and sends it to the specified MQTT topics.
 * Date: 9.December 2024
 */

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>
#include <mqtt/async_client.h>
#include <map>
#include <fstream>
#include <csignal>

using namespace std;

bool running = true;

const string MQTT_BROKER = ""; // Set the IP address of the MQTT broker
const string REQUEST_TOPIC = "requests";
string mood = "Neutral";

map<string, string> cityMood = {
    {"Brno", "Neutral"},
    {"Prague", "Neutral"},
    {"Ostrava", "Neutral"},
    {"Plzen", "Neutral"},
    {"Liberec", "Neutral"},
    {"Olomouc", "Neutral"},
    {"Vienna", "Neutral"},
    {"Berlin", "Neutral"},
    {"Paris", "Neutral"},
    {"London", "Neutral"}};

const string API_KEY = ""; // Set your OpenWeather API key
const string DATA_FILE = "data.txt";

// Function to handle HTTP response
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *out)
{
    size_t totalSize = size * nmemb;
    out->append((char *)contents, totalSize);
    return totalSize;
}

// Function to fetch weather data from OpenWeather API
string fetchWeatherData(const string &city)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        cerr << "Failed to initialize cURL" << endl;
        return "";
    }

    string response;
    string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + API_KEY + "&units=metric";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        cerr << "cURL error: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl); // Ensure cleanup in all cases
    return response;
}

// Function to parse JSON response
void parseWeatherData(const string &jsonResponse, int &temperature, int &humidity)
{
    size_t tempPos = jsonResponse.find("\"temp\":");
    if (tempPos != string::npos)
    {
        istringstream tempStream(jsonResponse.substr(tempPos + 7));
        tempStream >> temperature;
    }
    else
    {
        cerr << "Temperature data not found in JSON response" << endl;
    }

    size_t humidityPos = jsonResponse.find("\"humidity\":");
    if (humidityPos != string::npos)
    {
        istringstream humidityStream(jsonResponse.substr(humidityPos + 11));
        humidityStream >> humidity;
    }
    else
    {
        cerr << "Humidity data not found in JSON response" << endl;
    }
}

// Function to load city moods from file data.txt
void loadCityMood()
{
    ifstream file(DATA_FILE);
    if (file.is_open())
    {
        string city, mood;
        while (file >> city >> mood)
        {
            cityMood[city] = mood;
        }
        file.close();
    }
    else // create file if it does not exist and initialize with cityMood data
    {
        ofstream file(DATA_FILE);
        if (file.is_open())
        {
            for (const auto &entry : cityMood)
            {
                file << entry.first << " " << entry.second << endl;
            }
            file.close();
        }
        else
        {
            cerr << "Failed to create data file: " << DATA_FILE << endl;
        }
    }
}

// Function to save city moods to file
void saveCityMood()
{
    ofstream file(DATA_FILE);
    if (file.is_open())
    {
        for (const auto &entry : cityMood)
        {
            file << entry.first << " " << entry.second << endl;
        }
        file.close();
    }
    else
    {
        cerr << "Failed to open data file: " << DATA_FILE << endl;
    }
}

// Callback for handling incoming messages
class Callback : public virtual mqtt::callback
{
private:
    mqtt::async_client &client; // Reference to the MQTT client

public:
    Callback(mqtt::async_client &mqttClient) : client(mqttClient) {}

    void message_arrived(mqtt::const_message_ptr msg) override
    {
        string payload = msg->get_payload();
        cout << "[" << msg->get_topic() << "]: " << payload << endl;

        if (!payload.empty())
        {
            string city = payload.find(" ") != string::npos ? payload.substr(0, payload.find(" ")) : payload;
            string mood = payload.find(" ") != string::npos ? payload.substr(payload.find(" ") + 1) : cityMood[city];
            cityMood[city] = mood;

            string weatherData = fetchWeatherData(city);
            if (!weatherData.empty())
            {
                int temperature = 0;
                int humidity = 0;
                parseWeatherData(weatherData, temperature, humidity);
                mood = cityMood[city];
                // Construct and send the MQTT message to the city topic
                const string payload =
                    "{ \"temperature\": " + to_string(temperature) +
                    ", \"humidity\": " + to_string(humidity) +
                    ", \"mood\": \"" + mood + "\" }";

                try
                {
                    client.publish(city, payload.c_str(), payload.length(), 1, false);
                    cout << "[" << city << "]: " << payload << endl;
                    saveCityMood(); // Save updated city moods to file
                }
                catch (const mqtt::exception &e)
                {
                    cerr << "MQTT publish error: " << e.what() << endl;
                }
            }
            else
            {
                cerr << "Failed to fetch weather data for city: " << city << endl;
            }
        }
        else
        {
            cerr << "Invalid message format: " << payload << endl;
        }
    }
};

void signalHandler(int signum)
{
    cout << "Interrupt signal (" << signum << ") received. Exiting..." << endl;
    running = false; // Set the flag to false to exit the main loop
}

int main()
{
    signal(SIGINT, signalHandler);
    loadCityMood(); // Load city moods from file

    mqtt::async_client client(MQTT_BROKER, "");
    mqtt::connect_options connOpts;

    Callback callback(client);
    client.set_callback(callback);

    try
    {
        client.connect(connOpts)->wait();
        cout << "Connected to MQTT broker on " << MQTT_BROKER << endl;

        client.subscribe(REQUEST_TOPIC, 1)->wait();

        // Keep the program running to process incoming messages
        cout << "Waiting for messages on topic [" << REQUEST_TOPIC << "]..." << endl;
        while (running)
        {
            this_thread::sleep_for(chrono::milliseconds(100)); // Avoid busy-waiting
        }
        // Graceful cleanup
        if (client.is_connected())
        {
            client.disconnect()->wait();
            cout << "Disconnected from MQTT broker. Exiting..." << endl;
        }
    }
    catch (const mqtt::exception &e)
    {
        cerr << "MQTT error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
