#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MyLD2410.h"
#include <WiFi.h>
#include <PubSubClient.h>

// ===== WiFi & MQTT Config =====
const char* ssid = "HUAWEI Y6s";
const char* password = "veenz@123";
const char* mqtt_server = "192.168.43.235"; 

WiFiClient espClient;
PubSubClient client(espClient);


// Pin Definitions
#define RELAY_PIN 25
#define LDR_PIN 34
#define BUZZER_PIN 26
#define DARK_THRESHOLD 500

// OLED Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LD2410B Settings
#define sensorSerial Serial1
#define RX_PIN 16
#define TX_PIN 17
#define LD2410_BAUD_RATE 256000

// Global Objects
RTC_DS3231 rtc;
MyLD2410 sensor(sensorSerial);

// Study Logic
bool humanDetected = false;
bool studying = false;
bool lampOn = false;
bool buzzerAlerted = false;

bool lastHumanState = false;

unsigned long lastCheck = 0;
unsigned long sessionStart = 0;
unsigned long lastBuzzTime = 0;
const int checkInterval = 1000; // every 1 sec

// ===== WiFi & MQTT Setup =====
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


// ===== Setup =====
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  sensorSerial.begin(LD2410_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(1000);

  if (!sensor.begin()) {
    Serial.println("LD2410B sensor failed");
    while (true);
  }

  sensor.enhancedMode();
  sensor.setNoOneWindow(1);
  sensor.setMaxMovingGate(2);
  sensor.setMaxStationaryGate(2);
  sensor.setGateParameters(2, 35, 45);

  if (!rtc.begin()) {
    rtc.adjust(DateTime(2025, 5, 16, 00, 43, 0));  // YYYY, MM, DD, HH, MM, SS
    Serial.println("RTC failed");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 25);
  display.print("Hello!");
  display.display();
  delay(5000); // Show welcome for 5 sec

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastCheck < checkInterval) return;
  lastCheck = millis();

  DateTime now = rtc.now();
  int ldr = analogRead(LDR_PIN);
  bool isDark = ldr < DARK_THRESHOLD;

  // Human Detection
  humanDetected = false;
  if (sensor.check() == MyLD2410::Response::DATA) {
    int distance = sensor.detectedDistance();
    int moving = sensor.movingTargetSignal();
    int stationary = sensor.stationaryTargetSignal();
    if (sensor.presenceDetected() && distance < 100 &&
       (sensor.movingTargetDetected() && moving > 35 ||
        sensor.stationaryTargetDetected() && stationary > 45)) {
      humanDetected = true;
    }
  }

  // MQTT Publishing (only on state change)
  if (humanDetected != lastHumanState) {
    if (humanDetected) {
      client.publish("study/enter", "Human entered the study table.");
      Serial.println("MQTT: study/enter published");
    } else {
      client.publish("study/left", "Human left the study table.");
      Serial.println("MQTT: study/left published");
    }
    lastHumanState = humanDetected;
  }

  // Study Timer Logic
  if (humanDetected) {
    if (!studying) {
      sessionStart = millis();
      lastBuzzTime = sessionStart;
      studying = true;
    }

    float minutesStudied = (millis() - sessionStart) / 60000.0;
    if (millis() - lastBuzzTime >= 120000) {
      buzz();
      displayMessage("⏱" + String(minutesStudied, 2) + " min\nTake a break!");
      lastBuzzTime = millis();
    }
  } else if (studying) {
    float minutesStudied = (millis() - sessionStart) / 60000.0;
    displayMessage("You studied\n" + String(minutesStudied, 2) + " mins");
    studying = false;
    delay(5000);  // Show message before clearing
  }

  // Lamp control
  if (isDark && humanDetected) {
    if (!lampOn) {
      lampOn = true;
      digitalWrite(RELAY_PIN, HIGH);
    }
  } else {
    if (lampOn) {
      lampOn = false;
      digitalWrite(RELAY_PIN, LOW);
    }
  }

  // OLED Live Status
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("⏰ ");
  printTime(now);
  display.setCursor(0, 15);
  display.print(isDark ? "Night Time" : "Day Time");
  if (humanDetected) {
    display.setCursor(0, 30);
    display.print("Human Detected");
    
    float mins = (millis() - sessionStart) / 60000.0;
    display.setCursor(0, 45);
    display.print("Time: ");
    display.print(mins, 2);
    display.print(" min");

    display.setCursor(0, 58);  // Added line for LDR value
    display.print("LDR: ");
    display.print(ldr);
  }
  display.display();

}

void buzz() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void printTime(DateTime now) {
  if (now.hour() < 10) display.print('0');
  display.print(now.hour());
  display.print(':');
  if (now.minute() < 10) display.print('0');
  display.print(now.minute());
}

void displayMessage(String msg) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(msg);
  display.display();
}

