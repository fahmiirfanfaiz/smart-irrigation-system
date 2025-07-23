#include <ESP32Servo.h>

// Pin Definitions
const int SOIL_SENSOR_PIN = 32;   // Potentiometer (Soil Moisture)
const int RAIN_SENSOR_PIN = 25;   // LDR (Rain Detection)

const int LED_IDLE = 17;          // Green LED
const int LED_WATERING = 18;      // Yellow LED
const int LED_ERROR = 19;         // Red LED

const int SERVO_PIN = 26;         // Servo Motor

// Thresholds (Adjust based on Wokwi simulation values)
const int MOISTURE_THRESHOLD = 1500;   // Soil moisture threshold
const int RAIN_THRESHOLD = 1500;       // LDR threshold: lower value = rain

Servo pumpServo;

void setup() {
  Serial.begin(115200);

  // Initialize LEDs
  pinMode(LED_IDLE, OUTPUT);
  pinMode(LED_WATERING, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  // Attach Servo
  pumpServo.attach(SERVO_PIN);
  pumpServo.write(0); // Start with valve closed

  Serial.println("Smart Irrigation System with Rain Sensor Initialized!");
}

void loop() {
  int soilValue = analogRead(SOIL_SENSOR_PIN);
  int rainValue = analogRead(RAIN_SENSOR_PIN);

  Serial.print("Soil Moisture: ");
  Serial.print(soilValue);
  Serial.print(" | Rain Sensor: ");
  Serial.println(rainValue);

  // Error Handling
  if (soilValue < 50 || soilValue > 4095 || rainValue < 0 || rainValue > 4095) {
    Serial.println("ERROR: Sensor reading out of range!");
    setLEDs(false, false, true); // Red LED on
    pumpServo.write(0);          // Close valve
    delay(1000);
    return;
  }

  // Logic
  if (soilValue > MOISTURE_THRESHOLD) {
    // Soil is wet -> No watering
    Serial.println("Soil is wet. System in idle mode.");
    setLEDs(true, false, false);
    pumpServo.write(0);
  }
  else {
    // Soil is dry -> Check rain sensor
    if (rainValue < RAIN_THRESHOLD) {
      // It's raining -> Skip watering
      Serial.println("Soil is dry but it's raining. Skipping watering.");
      setLEDs(true, false, false);
      pumpServo.write(0);
    }
    else {
      // No rain -> Water the plants
      Serial.println("Soil is dry and no rain detected. Watering plants...");
      setLEDs(false, true, false);
      pumpServo.write(90);
    }
  }

  delay(1000);
}

// Helper function to control LEDs
void setLEDs(bool idle, bool watering, bool error) {
  digitalWrite(LED_IDLE, idle);
  digitalWrite(LED_WATERING, watering);
  digitalWrite(LED_ERROR, error);
}
