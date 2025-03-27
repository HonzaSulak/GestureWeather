# Author: Jan Šulák
# Description: MQTT client that fetches weather data from OpenWeather API and sends it to the specified MQTT topics.
# Date: 9.December 2024

CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude -std=c++17 -I/usr/local/include
LDFLAGS = -lcurl -lpaho-mqttpp3 -lpaho-mqtt3as -L/usr/local/lib
DEBUG_FLAGS = -g

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

EXEC = weather_mqtt

VALGRIND_OPTS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

all: $(EXEC)

run: $(EXEC)
	./$(EXEC)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(EXEC)

$(EXEC): clean $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXEC)

valgrind: debug
	valgrind $(VALGRIND_OPTS) ./$(EXEC)

.PHONY: clean all valgrind debug