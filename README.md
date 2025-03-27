# GestureWeather Project

## Overview

The GestureWeather project is a weather station system that combines gesture-based interaction with MQTT communication to display weather data and mood information for various European cities. The project is divided into two main components:

1. **API Component**: Fetches weather data from the OpenWeather API and communicates with an MQTT broker.
2. **GestureWeather Component**: Runs on an ESP32 microcontroller, allowing users to interact with the system using gestures and view weather and mood data on an OLED display.

---

## Features

### API Component
- **Weather Data Fetching**: Retrieves weather data (temperature and humidity) for predefined cities using the OpenWeather API.
- **City Mood Management**: Maintains a mood state for each city, which can be updated dynamically.
- **MQTT Communication**: Acts as an MQTT client, subscribing to a request topic and publishing weather and mood data to city-specific topics.
- **Data Persistence**: Saves and loads city mood data to/from a local file (`data.txt`).

### GestureWeather Component
- **Gesture-Based Interaction**: Uses the APDS-9960 gesture sensor to detect swipe gestures (up, down, left, right) for navigation and interaction.
- **OLED Display**: Displays weather data, mood information, and navigation screens on an Adafruit SSD1306 OLED display.
- **WiFi and MQTT Integration**: Connects to a WiFi network and communicates with the MQTT broker to send and receive data.
- **State Management**: Implements multiple states for user interaction:
  - **Startup State**: Displays a welcome screen.
  - **City State**: Allows navigation between cities.
  - **Detail State**: Shows detailed weather and mood information for the selected city.
  - **Mood State**: Enables mood adjustment for the selected city.

---

## Components and Code Structure

### API Component
- **Source Code**: Located in the `API/src/` directory.
  - [`main.cpp`](API/src/main.cpp): Implements the MQTT client, weather data fetching, and city mood management.
- **Build System**: Uses a `Makefile` for compilation and execution.

### GestureWeather Component
- **Source Code**: Located in the `GestureWeather/src/` directory.
  - [`main.cpp`](GestureWeather/src/main.cpp): Initializes hardware, manages gestures, and handles MQTT communication.
  - [`gesture.cpp`](GestureWeather/src/gesture.cpp): Implements gesture-based navigation and mood adjustment.
  - [`screen.cpp`](GestureWeather/src/screen.cpp): Handles OLED display rendering for various screens.
- **Headers**: Located in the `GestureWeather/include/` directory.
  - [`gesture.h`](GestureWeather/include/gesture.h): Declares gesture-related functions.
  - [`screen.h`](GestureWeather/include/screen.h): Declares display-related functions.
- **Configuration**: Managed via `platformio.ini` for the ESP32 development environment.

---

## How It Works

1. **API Component**:
   - Subscribes to the `requests` MQTT topic.
   - Fetches weather data from the OpenWeather API when a request is received.
   - Publishes the weather data and mood information to city-specific MQTT topics.

2. **GestureWeather Component**:
   - Detects user gestures using the APDS-9960 sensor.
   - Displays relevant information on the OLED screen based on the current state.
   - Sends requests to the `requests` MQTT topic and listens for responses from the API component.

---

## Prerequisites

### API Component
- **Dependencies**:
  - `libcurl` for HTTP requests.
  - `paho-mqttpp3` for MQTT communication.
- **Setup**:
  - Set the `MQTT_BROKER` and `API_KEY` values in [`main.cpp`](API/src/main.cpp).

### GestureWeather Component
- **Hardware**:
  - ESP32 microcontroller.
  - APDS-9960 gesture sensor.
  - Adafruit SSD1306 OLED display.
- **Dependencies**:
  - Libraries specified in `platformio.ini`.
- **Setup**:
  - Set the `WIFI_SSID`, `WIFI_PASS`, and `MQTT_BROKER` values in [`main.cpp`](GestureWeather/src/main.cpp).

---

## Usage

### API Component
1. Navigate to the `API` directory.
2. Build the project using the `Makefile`:
   ```sh
   make