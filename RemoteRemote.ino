#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>  // For working with JSON

#define TX_PIN 18            // GPIO pin connected to the APF03 transmitter
#define SIGNAL_INTERVAL 10000 // Interval between signals in milliseconds (10 seconds)
#define BIT_DURATION 107      // Duration of each bit in microseconds
#define REPEAT_COUNT 7        // Number of times to repeat the signal

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Binary signals to transmit
const char light_power_signal[] = "1110000000001111111110001111111110001111111110001111111110011111111100011111111100011100000000011100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011100000000011100000000011111111100011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011111111100011100000000011110";
const char fan_power_signal[] = "11100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011111111100011111111100011111111100011111111100111111111100011111111100011100000000111000000000111111111000111000000000111000000000111000000000111000000000111000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000011110";
const char fan_natural_signal[] = "1110000000001111111111000111111111000111111111001111111111001111111110001111111111001111000000000111000000000111111111000111111111100011111111100011111111100011111111100011111111100011100000000011100000000011111111100011100000000011100000000011111111100011100000000011100000000011111111100011100000000011100000000011100000000011100000000011111111100011100000000011100000000011111111100011110";
const char fan_reverse_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110001111111111000111000000000111000000000111111111000111111111000111111111000111111111000111111111000111111111000111000000000111111111000111111111000111000000000111100000000011111111100011100000000011100000000011100000000011100000000011100000000011100000000011111111100011100000000011100000000011100000000011111111100011110";
const char fan_level1_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110011111111100011100000000011100000000011111111110011111111100011111111100011111111100011111111110001111111110001110000000001110000000001110000000001111111110001110000000001111000000000111000000000111000000000111111111000111000000000111000000000111000000000111000000000111000000000111000000000111100000000011100000000011110";
const char fan_level2_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001110000000001110000000001111111110001110000000001110000000001110000000001111111110001110000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000011110";
const char fan_level3_signal[] = "11100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011100000000011111111100011100000000011100000000011100000000011111111100011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011110";
const char fan_level4_signal[] = "11100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011100000000011111111100011100000000011100000000011111111100011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011110";
const char fan_level5_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001100000000011111111100011111111100011111111100011111111100011111111100011111111100011000000000111000000000111000000000111111111000111000000000111000000000111111111000111000000000111111111000111000000000111000000000111000000000111000000000111000000000111000000000111000000000111100000000011110";
const char fan_level6_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001100000000001111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011100000000011111111100011100000000011100000000011111111100011111111100011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011100000000011110";
const char light_brightness_up_signal[] = "1110000000011111111110001111111110001111111110001111111110001111111110001111111110001110000000001110000000001111111110001111111110001111111111000111111111000111111111000111111111100011100000000011100000000011100000000011100000000011111111100011111111100011100000000011100000000011100000000011110000000001110000000001110000000001111111110001111111110001110000000001111111110011111111100011110";
const char light_brightness_down_signal[] = "111000000000111111111000111111111100011111111100111111111100011111111100111111111100111100000000011100000000011111111100011111111100111111111000111111111000111111111000111111111000111000000000111000000000111000000000111000000000111111111000111111111000111000000000111000000000111000000000111000000000111000000000111000000000111111111000111111111000111000000000111111111100111100000000011110";
const char light_colour_yellow_signal[] = "1110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001110000000001111111110001111111110001111111110001111111110001111111110001111111110001110000000001110000000001110000000001110000000001111111110001110000000001110000000001110000000001110000000001110000000001110000000001110000000001110000000001111111110001110000000001110000000001111111110001110";
const char light_colour_grey_signal[] = "11100000000011111111110001111111110001111111110001111111111001111111110001111111110001110000000001111000000000111111111000111111111000111111111000111111111000111111111000111111111000111000000000111000000000111000000000111000000000111111111000111000000000111100000000011100000000011100000000011100000000011100000000011100000000011111111100011111111100011100000000011111111100011100000000011110";
const char light_colour_blue_signal[] = "11100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011111111100011111111100011111111100011111111100011111111100011111111100011100000000011100000000011100000000011100000000011111111100011100000000011100000000011100000000011110000000011100000000011100000000011100000000011111111100011111111100011100000000011111111100011111111100011110";

// Create an AsyncWebServer instance on port 80
AsyncWebServer server(80);

// Struct to manage current and previous states
struct State {
  bool currentFanPower;
  bool previousFanPower;
  bool currentReverse;
  bool previousReverse;
  String currentFanSpeed;
  String previousFanSpeed;
  int currentBrightness;
  int previousBrightness;
  String currentLightColour;
  String previousLightColour;
  bool currentLightPower;
  bool previousLightPower;
};

State state;

// Save state to file
void saveToFile() {
  File file = SPIFFS.open("/values.json", "w");  // Open file for writing
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Create a JSON object
  StaticJsonDocument<512> doc;
  doc["currentFanPower"] = state.currentFanPower;
  doc["previousFanPower"] = state.previousFanPower;
  doc["currentReverse"] = state.currentReverse;
  doc["previousReverse"] = state.previousReverse;
  doc["currentFanSpeed"] = state.currentFanSpeed;
  doc["previousFanSpeed"] = state.previousFanSpeed;
  doc["currentBrightness"] = state.currentBrightness;
  doc["previousBrightness"] = state.previousBrightness;
  doc["currentLightColour"] = state.currentLightColour;
  doc["previousLightColour"] = state.previousLightColour;
  doc["currentLightPower"] = state.currentLightPower;
  doc["previousLightPower"] = state.previousLightPower;

  // Serialize the JSON object and write it to the file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to file");
  } else {
    Serial.println("Data saved successfully");
  }

  file.close();  // Always close the file when done
}

// Load state from file
void loadFromFile() {
  File file = SPIFFS.open("/values.json", "r");  // Open file for reading
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Create a JSON object
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);  // Parse the JSON

  if (error) {
    Serial.println("Failed to read file");
    file.close();
    return;
  }

  // Load state from JSON
  state.currentFanPower = doc["currentFanPower"];
  state.previousFanPower = doc["previousFanPower"];
  state.currentReverse = doc["currentReverse"];
  state.previousReverse = doc["previousReverse"];
  state.currentFanSpeed = doc["currentFanSpeed"].as<String>();
  state.previousFanSpeed = doc["previousFanSpeed"].as<String>();
  state.currentBrightness = doc["currentBrightness"];
  state.previousBrightness = doc["previousBrightness"];
  state.currentLightColour = doc["currentLightColour"].as<String>();
  state.previousLightColour = doc["previousLightColour"].as<String>();
  state.currentLightPower = doc["currentLightPower"];
  state.previousLightPower = doc["previousLightPower"];

  file.close();
}

// Send a signal
void sendSignal(const char* signal) {
  for (int repeat = 0; repeat < REPEAT_COUNT; repeat++) { // Repeat the signal REPEAT_COUNT times
    int count = 1; // Counter for the current segment length

    for (size_t i = 1; i <= strlen(signal); i++) {
      // Check if the current bit is the same as the previous bit
      if (i < strlen(signal) && signal[i] == signal[i - 1]) {
        count++; // Increment the segment length
      } else {
        // Send the current segment
        digitalWrite(TX_PIN, signal[i - 1] == '1' ? HIGH : LOW);
        delayMicroseconds(BIT_DURATION * count); // Transmit the segment for the appropriate duration

        count = 1; // Reset the segment counter
      }
    }
    delay(5); // Small delay (e.g., 5 milliseconds) between repeated signals
  }

  digitalWrite(TX_PIN, LOW); // Ensure the pin is LOW after the transmission
}

// Initialize remote settings
void initRemote() {
  state.currentFanPower = false;
  state.previousFanPower = false;
  state.currentReverse = false;
  state.previousReverse = false;
  state.currentFanSpeed = "natural";
  state.previousFanSpeed = "natural";
  state.currentBrightness = 0;
  state.previousBrightness = 0;
  state.currentLightColour = "grey";
  state.previousLightColour = "grey";
  state.currentLightPower = false;
  state.previousLightPower = false;

  saveToFile(); // Save initial state to file
}

// Handle button presses
void handleButtonPress(AsyncWebServerRequest *request) {
  String stateParam = request->getParam("state")->value();
  String type = request->url().substring(1, request->url().indexOf("Button"));

  if (type == "lightPower") {
    Serial.println("Light power button was pressed!");
    sendSignal(light_power_signal);
    state.previousLightPower = state.currentLightPower;
    state.currentLightPower = (stateParam == "on");
    saveToFile();
  } else if (type == "fanPower") {
    Serial.println("Fan power button was pressed!");
    sendSignal(fan_power_signal);
    state.previousFanPower = state.currentFanPower;
    state.currentFanPower = (stateParam == "on");
    saveToFile();
  } else if (type == "fanReverse") {
    Serial.println("Fan reverse button was pressed!");
    sendSignal(fan_reverse_signal);
    state.previousReverse = state.currentReverse;
    state.currentReverse = (stateParam == "on");
    saveToFile();
  }

  request->send(200, "text/plain", type + " button press received");
}

// Handle colour dropdown change
void handleColourDropdownChange(AsyncWebServerRequest *request) {
  if (request->hasParam("colour")) {
    String value = request->getParam("colour")->value();  // Get selected colour value
    Serial.printf("Colour dropdown changed to: %s\n", value.c_str());

    // Update state
    state.previousLightColour = state.currentLightColour;
    state.currentLightColour = value;

    // Send the signal based on the selected color
    if (value == "Grey") {
      sendSignal(light_colour_grey_signal);
    } else if (value == "Yellow") {
      sendSignal(light_colour_yellow_signal);
    } else if (value == "Blue") {
      sendSignal(light_colour_blue_signal);
    }

    saveToFile();
  }
  request->send(200, "text/plain", "Colour dropdown change received");
}

// Handle fan speed dropdown change
void handleSpeedDropdownChange(AsyncWebServerRequest *request) {
  if (request->hasParam("level")) {
    String value = request->getParam("level")->value();  // Get selected fan speed value
    Serial.printf("Fan speed changed to: %s\n", value.c_str());

    // Update state
    state.previousFanSpeed = state.currentFanSpeed;
    state.currentFanSpeed = value;

    // Send the signal based on the selected fan speed
    if (value == "Natural") {
      sendSignal(fan_natural_signal);
    } else if (value == "1") {
      sendSignal(fan_level1_signal);
    } else if (value == "2") {
      sendSignal(fan_level2_signal);
    } else if (value == "3") {
      sendSignal(fan_level3_signal);
    } else if (value == "4") {
      sendSignal(fan_level4_signal);
    } else if (value == "5") {
      sendSignal(fan_level5_signal);
    } else if (value == "6") {
      sendSignal(fan_level6_signal);
    }

    saveToFile();
  }
  request->send(200, "text/plain", "Fan speed dropdown change received");
}

// Handle slider change
void handleSliderChange(AsyncWebServerRequest *request) {
  if (request->hasParam("value")) {
    String value = request->getParam("value")->value();
    Serial.printf("Slider changed to: %s\n", value.c_str());

    int previousBrightness = state.currentBrightness;
    int currentBrightness = value.toInt();

    while (currentBrightness != previousBrightness) {
      if (currentBrightness > previousBrightness) {
        sendSignal(light_brightness_up_signal);
        previousBrightness++;
      } else {
        sendSignal(light_brightness_down_signal);
        previousBrightness--;
      }
      delay(100); // Add a small delay to avoid overwhelming the transmitter
    }

    // Update state
    state.previousBrightness = state.currentBrightness;
    state.currentBrightness = currentBrightness;

    saveToFile();
  }
  request->send(200, "text/plain", "Slider change received");
}

// Handle timer button press
void handleTimerButtonPress(AsyncWebServerRequest *request) {
  if (request->hasParam("state")) {
    String stateParam = request->getParam("state")->value();
    if (stateParam == "start") {
      Serial.println("Timer started!");
      if (!state.currentFanPower) {
        sendSignal(fan_power_signal);
        state.previousFanPower = state.currentFanPower;
        state.currentFanPower = true;
        saveToFile();
      }
      request->send(200, "text/plain", "Timer started");
    } else if (stateParam == "stop") {
      Serial.println("Timer stopped!");
      request->send(200, "text/plain", "Timer stopped");
    } else if (stateParam == "reset") {
      Serial.println("Timer reset!");
      sendSignal(fan_power_signal);
      state.previousFanPower = state.currentFanPower;
      state.currentFanPower = false;
      saveToFile();
      request->send(200, "text/plain", "Timer reset");
    } else {
      request->send(400, "text/plain", "Invalid state");
    }
  } else {
    request->send(400, "text/plain", "Missing state parameter");
  }
}

// Add this for state retrieval endpoint (optional but useful)
void handleGetState(AsyncWebServerRequest *request) {
  StaticJsonDocument<512> doc;
  
  doc["fanPower"] = state.currentFanPower;
  doc["reverse"] = state.currentReverse;
  doc["fanSpeed"] = state.currentFanSpeed;
  doc["brightness"] = state.currentBrightness;
  doc["lightColour"] = state.currentLightColour;
  doc["lightPower"] = state.currentLightPower;

  String response;
  serializeJson(doc, response);
  
  request->send(200, "application/json", response);
}

// Add this at the start of setup()
void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {  // The `true` argument will format SPIFFS if it's not already formatted
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Setup transmitter stuff
  pinMode(TX_PIN, OUTPUT);  // Set TX_PIN as output
  digitalWrite(TX_PIN, LOW); // Start with the transmitter OFF

  // Initialize remote state
  initRemote();
  
  // Load saved state
  loadFromFile();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/lightPowerButton", HTTP_GET, handleButtonPress);
  server.on("/fanPowerButton", HTTP_GET, handleButtonPress);
  server.on("/fanReverseButton", HTTP_GET, handleButtonPress);
  server.on("/lightColour", HTTP_GET, handleColourDropdownChange);
  server.on("/fanSpeedLevel", HTTP_GET, handleSpeedDropdownChange);
  server.on("/lightBrightness", HTTP_GET, handleSliderChange);
  server.on("/timerButton", HTTP_GET, handleTimerButtonPress);
  server.on("/getState", HTTP_GET, handleGetState);

  // Serve the webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Remote²</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
      /* General Styles */
      body {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100vh;
        margin: 0;
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        background-color: #f8f9fa;
        color: #333;
      }

      /* Title Styling */
      h1 {
        margin-bottom: 20px;
        font-size: 2.5rem;
        font-weight: 700;
        background: linear-gradient(45deg, #6D5ACF, #5a4aac);
        -webkit-background-clip: text;
        background-clip: text;
        color: transparent;
        text-align: center;
      }

      /* Button Container */
      .button-container {
        display: flex;
        gap: 10px;
        margin-bottom: 20px;
      }

      /* Buttons */
      button {
        font-size: 1rem;
        padding: 12px 24px;
        border: none;
        border-radius: 12px;
        background-color: #6D5ACF;
        color: white;
        cursor: pointer;
        box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        transition: background-color 0.3s ease, transform 0.2s ease;
      }

      button.toggled {
        background-color: #4CAF50; /* Green for "on" state */
      }

      button.square {
        width: 45px; /* 25% smaller */
        height: 45px; /* 25% smaller */
        padding: 0;
        display: flex;
        align-items: center;
        justify-content: center;
      }

      button:hover {
        background-color: #5a4aac;
        transform: translateY(-2px);
      }

      button:active {
        transform: translateY(0);
      }

      /* Labels */
      label {
        font-size: 1rem;
        color: #555;
        margin-top: 10px;
        font-weight: 500;
      }

      /* Dropdowns */
      select {
        font-size: 1rem;
        padding: 10px;
        border: 2px solid #6D5ACF;
        border-radius: 12px;
        background-color: white;
        color: #333;
        cursor: pointer;
        appearance: none;
        -webkit-appearance: none;
        -moz-appearance: none;
        background-image: url('data:image/svg+xml;utf8,<svg fill="%236D5ACF" height="24" viewBox="0 0 24 24" width="24" xmlns="http://www.w3.org/2000/svg"><path d="M7 10l5 5 5-5z"/></svg>');
        background-repeat: no-repeat;
        background-position: right 10px center;
        background-size: 12px;
        transition: border-color 0.3s ease;
        width: 150px; /* 25% less wide */
      }

      select:hover {
        border-color: #5a4aac;
      }

      /* Slider Container */
      .slider-container {
        position: relative;
        width: 200px;
        margin-top: 10px;
      }

      /* Filled Track */
      .slider-filled {
        position: absolute;
        top: 50%;
        left: 0;
        height: 8px;
        background: linear-gradient(to right, #6D5ACF, #A8A4CE);
        border-radius: 8px;
        z-index: 1;
        transform: translateY(-50%);
      }

      /* Slider Input */
      input[type="range"] {
        position: relative;
        width: 100%;
        margin: 0;
        z-index: 2;
        background: transparent;
        -webkit-appearance: none;
        appearance: none;
      }

      input[type="range"]::-webkit-slider-runnable-track {
        background: transparent;
        height: 8px;
      }

      input[type="range"]::-moz-range-track {
        background: transparent;
        height: 8px;
      }

      input[type="range"]::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 20px;
        height: 20px;
        background: white;
        border: 2px solid #6D5ACF;
        border-radius: 50%;
        cursor: pointer;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        transition: transform 0.2s ease;
      }

      input[type="range"]::-moz-range-thumb {
        width: 20px;
        height: 20px;
        background: white;
        border: 2px solid #6D5ACF;
        border-radius: 50%;
        cursor: pointer;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }

      input[type="range"]:hover::-webkit-slider-thumb {
        transform: scale(1.1);
      }

      /* Timer Container */
      .timer-container {
        display: flex;
        align-items: center;
        position: relative;
        margin-top: 20px;
      }

      /* Timer Input and Display */
      #timer-input, #timer-display {
        font-size: 1.6rem; /* Reduced by 20% (from 2rem) */
        text-align: right;
        padding: 10px;
        border: 2px solid #6D5ACF;
        border-radius: 12px;
        width: 80px; /* Reduced by 20% (from 100px) */
        background-color: white;
        color: #333;
        position: relative;
        z-index: 2;
        direction: ltr;
        transition: border-color 0.3s ease;
      }

      #timer-display {
        display: none;
      }

      /* Placeholder Background */
      .placeholder-background {
        position: absolute;
        top: 52%;
        left: 8px; /* Moved 6px to the right (from 2px) */
        transform: translateY(-50%);
        font-size: 1.28rem; /* Reduced by 20% (from 1.6rem) */
        color: #999;
        pointer-events: none;
        z-index: 3;
        text-align: left;
        width: 100%;
        box-sizing: border-box;
        direction: ltr;
      }

      #start-button {
        font-size: 1.5rem;
        padding: 10px;
        margin-left: 10px;
        border: 2px solid #6D5ACF;
        border-radius: 12px;
        background-color: #6D5ACF;
        color: white;
        cursor: pointer;
        display: flex;
        align-items: center;
        justify-content: center;
        width: 45px; /* 25% smaller */
        height: 45px; /* 25% smaller */
        transition: background-color 0.3s ease, transform 0.2s ease;
      }

      #start-button:hover {
        background-color: #5a4aac;
        transform: translateY(-2px);
      }

      #start-button:active {
        transform: translateY(0);
      }

      /* Responsive Adjustments */
      @media (max-width: 600px) {
        h1 {
          font-size: 2rem;
        }

        .button-container {
          gap: 8px;
        }

        button {
          font-size: 1rem;
          padding: 10px 20px;
        }

        label, select, input[type="range"] {
          font-size: 1rem;
        }

        input[type="range"] {
          width: 80%;
        }

        .timer-container {
          flex-direction: row;
          align-items: center;
        }

        #timer-input, #timer-display {
          font-size: 1.8rem;
          width: 90px;
        }

        .placeholder-background {
          font-size: 1.4rem;
        }

        #start-button {
          font-size: 1.4rem;
          width: 45px;
          height: 45px;
        }
      }
    </style>
    <script>
    function sendButton(button) {
      const state = button.classList.contains('toggled') ? 'off' : 'on';
      let type = "";

      if (button.textContent === "Light") {
        type = "lightPower";
      } else if (button.textContent === "Fan") {
        type = "fanPower";
      } else if (button.textContent === "↺") {
        type = "fanReverse";
      }

      // Send the fetch request with the type and state
      fetch(`/${type}Button?state=${state}`)
        .then(response => response.text())
        .then(() => {
          button.classList.toggle('toggled');
        })
        .catch(error => console.error('Error:', error));
    }

    function sendColourDropdown() {
      const value = document.getElementById('colour-dropdown').value;
      fetch(`/lightColour?colour=${value}`)
        .then(response => response.text())
        .catch(error => console.error('Error:', error));
    }

    function sendFanSpeedDropdown() {
      const value = document.getElementById('fanSpeed').value;
      fetch(`/fanSpeedLevel?level=${value}`)
        .then(response => response.text())
        .catch(error => console.error('Error:', error));
    }

    function sendSlider() {
      const value = document.getElementById('slider').value;
      fetch(`/lightBrightness?value=${value}`)
        .then(response => response.text())
        .catch(error => console.error('Error:', error));
    }

    function sendTimerButton(state) {
      fetch(`/timerButton?state=${state}`)
        .then(response => response.text())
        .catch(error => console.error('Error:', error));
    }

    document.addEventListener('DOMContentLoaded', function() {
      const timerInput = document.getElementById('timer-input');
      const placeholder = document.getElementById('placeholder');
      const timerDisplay = document.getElementById('timer-display');
      const timerButton = document.getElementById('start-button');
      let countdownInterval = null;
      let previousEntry = '01:00'; // Default previous entry

      // Update button state based on input and timer state
      function updateButtonState() {
        if (countdownInterval) {
          timerButton.innerHTML = '✕';
          timerButton.style.backgroundColor = '#f44336';
          sendTimerButton('stop');
        } else if (timerInput.value.trim() === '') {
          timerButton.innerHTML = '↺';
          timerButton.style.backgroundColor = '#6D5ACF';
          sendTimerButton('reset');
        } else {
          timerButton.innerHTML = '▶';
          timerButton.style.backgroundColor = '#6D5ACF';
          sendTimerButton('start');
        }
      }

      function updatePlaceholder(value) {
        const digitsEntered = value.replace(/\D/g, '').length;
        switch (digitsEntered) {
          case 0:
            placeholder.textContent = 'HH:MM';
            break;
          case 1:
            placeholder.textContent = 'HH:M';
            break;
          case 2:
            placeholder.textContent = 'HH:';
            break;
          case 3:
            placeholder.textContent = 'H';
            break;
          case 4:
            placeholder.textContent = '';
            break;
          default:
            placeholder.textContent = '';
        }
      }

      timerInput.addEventListener('input', function(e) {
        let input = e.target;
        let value = input.value.replace(/\D/g, '');

        if (value.length > 4) {
          value = value.slice(0, 4);
        }

        if (value.length > 2) {
          value = value.slice(0, 2) + ':' + value.slice(2, 4);
        }

        input.value = value;
        updatePlaceholder(value);
        updateButtonState();
      });

      timerButton.addEventListener('click', function() {
        if (countdownInterval) {
          stopTimer();
          resetTimer();
        } else if (timerInput.value.trim() === '') {
          timerInput.value = previousEntry;
          updatePlaceholder(previousEntry);
          updateButtonState();
        } else {
          previousEntry = timerInput.value;
          startTimer();
        }
      });

      function startTimer() {
        const value = timerInput.value.replace(/\D/g, '');
        if (value.length === 4) {
          const minutes = parseInt(value.slice(0, 2), 10);
          const seconds = parseInt(value.slice(2, 4), 10);
          const totalTime = minutes * 60 + seconds;

          if (totalTime > 0) {
            timerInput.style.display = 'none';
            placeholder.style.display = 'none';
            timerDisplay.style.display = 'block';
            timerDisplay.textContent = timerInput.value;
            startCountdown(totalTime);
            updateButtonState();

            // Ensure the fan is on
            const fanButton = document.querySelector('button:contains("Fan")');
            if (!fanButton.classList.contains('toggled')) {
              sendButton(fanButton);
            }
          } else {
            alert('Please enter a valid time.');
          }
        } else {
          alert('Please enter a valid time in the format HH:MM.');
        }
      }

      function startCountdown(totalTime) {
        let remainingTime = totalTime;

        countdownInterval = setInterval(() => {
          if (remainingTime <= 0) {
            clearInterval(countdownInterval);
            countdownInterval = null;
            timerDisplay.textContent = '00:00';
            alert('Timer finished!');
            resetTimer();

            // Turn off the fan
            const fanButton = document.querySelector('button:contains("Fan")');
            if (fanButton.classList.contains('toggled')) {
              sendButton(fanButton);
            }
            return;
          }

          const minutes = Math.floor(remainingTime / 60);
          const seconds = remainingTime % 60;
          timerDisplay.textContent = `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
          remainingTime--;
        }, 1000);
      }

      function stopTimer() {
        clearInterval(countdownInterval);
        countdownInterval = null;
      }

      function resetTimer() {
        stopTimer();
        timerInput.style.display = 'block';
        placeholder.style.display = 'block';
        timerDisplay.style.display = 'none';
        timerInput.value = '';
        placeholder.textContent = 'HH:MM';
        updateButtonState();
      }

      updateButtonState();
    });
    )rawliteral");
  });


  // Start server
  server.begin();
}

void loop() {
  // No need to do anything here since we're using async server
}