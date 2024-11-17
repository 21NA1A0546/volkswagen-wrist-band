// Include necessary libraries
#include <Wire.h>
#include <Adafruit_SSD1306.h>      // For OLED Display
#include <PulseSensorPlayground.h> // For Pulse Sensor

// Define OLED address
#define SSD1306_I2C_ADDRESS 0x3C

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pulse Sensor Settings
#define PULSE_INPUT_PIN A0
PulseSensorPlayground pulseSensor;

// Alarm and Car Control Pins
#define BUZZER_PIN 7
#define CAR_STOP_PIN 8

// Variables to hold readings
int heartRate;
float systolicBP = 120.0; // Placeholder value
float diastolicBP = 80.0; // Placeholder value

// Normal thresholds
const int MAX_HEART_RATE = 100;      // Example threshold for heart rate
const float MAX_SYSTOLIC_BP = 130.0; // Example threshold for systolic BP
const float MAX_DIASTOLIC_BP = 85.0; // Example threshold for diastolic BP

// Timing variables for debouncing
unsigned long lastCriticalAlert = 0;
const unsigned long criticalDebounceDelay = 5000; // 5 seconds

void setup()
{
    // Initialize Serial Communication
    Serial.begin(9600);

    // Initialize OLED display
    if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C))
    {
        Serial.println("OLED initialization failed!");
        while (true)
            ;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    // Initialize Pulse Sensor
    if (!pulseSensor.analogInput(PULSE_INPUT_PIN) || !pulseSensor.begin())
    {
        Serial.println("Pulse sensor initialization failed!");
        while (true)
            ;
    }

    // Initialize buzzer and car stop pins
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(CAR_STOP_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(CAR_STOP_PIN, HIGH); // Assume HIGH = car running, LOW = car stopped

    display.println("Initializing...");
    display.display();
    delay(2000);
}

void loop()
{
    // Read Heart Rate
    heartRate = pulseSensor.getBeatsPerMinute();
    bool isBeatDetected = pulseSensor.sawNewBeat();

    // Simulate BP readings (Replace with BP sensor code)
    systolicBP = 120.0 + random(-5, 5); // Example fluctuation
    diastolicBP = 80.0 + random(-3, 3);

    // Check if any reading is above the normal range
    bool isCritical = isCriticalCondition();

    if (isCritical && (millis() - lastCriticalAlert > criticalDebounceDelay))
    {
        lastCriticalAlert = millis();
        activateCriticalAlert();
    }
    else if (!isCritical)
    {
        deactivateCriticalAlert();
    }

    // Display readings
    updateDisplay(isBeatDetected);

    // Print readings to Serial Monitor
    logReadings();

    delay(1000);
}

// Function to check critical health conditions
bool isCriticalCondition()
{
    return (heartRate > MAX_HEART_RATE || systolicBP > MAX_SYSTOLIC_BP || diastolicBP > MAX_DIASTOLIC_BP);
}

// Function to activate critical alert
void activateCriticalAlert()
{
    digitalWrite(BUZZER_PIN, HIGH);  // Turn on the buzzer
    digitalWrite(CAR_STOP_PIN, LOW); // Stop the car
    Serial.println("CRITICAL ALERT ACTIVATED!");
}

// Function to deactivate critical alert
void deactivateCriticalAlert()
{
    digitalWrite(BUZZER_PIN, LOW);    // Turn off the buzzer
    digitalWrite(CAR_STOP_PIN, HIGH); // Keep the car running
}

// Function to update the OLED display
void updateDisplay(bool isBeatDetected)
{
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("Health Monitor");
    display.println("------------------");

    if (isBeatDetected)
    {
        display.print("Heart Rate: ");
        display.print(heartRate);
        display.println(" BPM");
    }
    else
    {
        display.println("Heart Rate: Measuring...");
    }

    display.print("BP: ");
    display.print(systolicBP);
    display.print("/");
    display.print(diastolicBP);
    display.println(" mmHg");

    if (isCriticalCondition())
    {
        display.println("ALERT! Critical levels");
    }

    display.display();
}

// Function to log readings to the Serial Monitor
void logReadings()
{
    Serial.print("Heart Rate: ");
    Serial.print(heartRate);
    Serial.print(" BPM | BP: ");
    Serial.print(systolicBP);
    Serial.print("/");
    Serial.print(diastolicBP);
    Serial.println(" mmHg");
}