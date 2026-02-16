/*
 * SUIM Node - Universal Sensor Code (COMPLETE VERSION)
 * Works with any combination of sensors (as some may not arrive in time)
 * Auto-detects what's connected and sends appropriate data
 * 
 * Team JEPAS - Davidson Inventors Challenge 2026
 * Last Updated: Feb 15, 2026
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// ============================================
// CONFIGURATION - CHANGE THESE FOR EACH NODE
// ============================================
const char* NODE_ID = "Node2";  // Change to "Node2", "Node3" for other nodes
const char* WIFI_SSID = "YOUR_WIFI_SSID";  // UPDATE THIS  // UPDATE WITH SCHOOL WIFI ON TUESDAY
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";  // UPDATE THIS  // UPDATE WITH SCHOOL WIFI PASSWORD
const char* PI_URL = "http://davidson-hub.local:5000/data";  // Raspberry Pi endpoint

// ============================================
// SENSOR PIN DEFINITIONS
// ============================================
#define MQ135_PIN 34   // Analog pin for MQ-135 (air quality)
#define MQ7_PIN 35     // Analog pin for MQ-7 (carbon monoxide)
#define MQ131_PIN 32   // Analog pin for MQ-131 (ozone)

// I2C sensors use standard pins (GPIO 21 = SDA, GPIO 22 = SCL)
Adafruit_BME680 bme;  // BME680 environmental sensor

// ============================================
// SENSOR AVAILABILITY FLAGS
// ============================================
bool bme680_available = false;
bool mq135_available = false;
bool mq7_available = false;
bool mq131_available = false;
bool mics6814_available = false;  // Should be at school? by Tuesday
bool pms5003_available = false;   // Coming Tuesday (needs breakout board, may not arrive in time - nuclear option, cut and seperat wires manually)

// ============================================
// SETUP - RUNS ONCE ON BOOT
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("SUIM Node Starting...");
  Serial.println("=================================");
  Serial.print("Node ID: ");
  Serial.println(NODE_ID);
  
  // Initialize I2C
  Wire.begin();
  
  // Detect and initialize sensors
  detectSensors();
  
  // Connect to WiFi
  connectToWiFi();
  
  Serial.println("\n=== Setup Complete ===");
  Serial.println("Entering main loop...\n");
}

// ============================================
// SENSOR DETECTION
// ============================================
void detectSensors() {
  Serial.println("\n--- Detecting Sensors ---");
  
  // Try to initialize BME680
  if (bme.begin(0x76) || bme.begin(0x77)) {  // Try both I2C addresses
    bme680_available = true;
    
    // Configure BME680 settings
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320°C for 150ms
    
    Serial.println("✓ BME680 detected (temp, humidity, pressure, VOC)");
  } else {
    Serial.println("✗ BME680 not detected");
  }
  
  // Check MQ-135 (analog sensor - need consistent high readings)
  pinMode(MQ135_PIN, INPUT);
  delay(100);
  int mq135_test1 = analogRead(MQ135_PIN);
  delay(100);
  int mq135_test2 = analogRead(MQ135_PIN);
  
  // Sensor is real if: reading > 1000 AND readings are very similar (< 100 difference)
  if (mq135_test1 > 1000 && abs(mq135_test1 - mq135_test2) < 100) {
    mq135_available = true;
    Serial.print("✓ MQ-135 detected (air quality) - Raw: ");
    Serial.print(mq135_test1);
    Serial.print(", ");
    Serial.println(mq135_test2);
  } else {
    Serial.print("✗ MQ-135 not detected (Raw: ");
    Serial.print(mq135_test1);
    Serial.print(" vs ");
    Serial.print(mq135_test2);
    Serial.println(")");
  }
  
  // Check MQ-7 (analog sensor)
  pinMode(MQ7_PIN, INPUT);
  delay(100);
  int mq7_test1 = analogRead(MQ7_PIN);
  delay(100);
  int mq7_test2 = analogRead(MQ7_PIN);
  
  if (mq7_test1 > 1000 && abs(mq7_test1 - mq7_test2) < 100) {
    mq7_available = true;
    Serial.print("✓ MQ-7 detected (carbon monoxide) - Raw: ");
    Serial.print(mq7_test1);
    Serial.print(", ");
    Serial.println(mq7_test2);
  } else {
    Serial.print("✗ MQ-7 not detected (Raw: ");
    Serial.print(mq7_test1);
    Serial.print(" vs ");
    Serial.print(mq7_test2);
    Serial.println(")");
  }
  
  // Check MQ-131 (analog sensor)
  pinMode(MQ131_PIN, INPUT);
  delay(100);
  int mq131_test1 = analogRead(MQ131_PIN);
  delay(100);
  int mq131_test2 = analogRead(MQ131_PIN);
  
  if (mq131_test1 > 1000 && abs(mq131_test1 - mq131_test2) < 100) {
    mq131_available = true;
    Serial.print("✓ MQ-131 detected (ozone) - Raw: ");
    Serial.print(mq131_test1);
    Serial.print(", ");
    Serial.println(mq131_test2);
  } else {
    Serial.print("✗ MQ-131 not detected (Raw: ");
    Serial.print(mq131_test1);
    Serial.print(" vs ");
    Serial.print(mq131_test2);
    Serial.println(")");
  }
  
  // Check MICS-6814 (I2C sensor - address 0x04)
  Wire.beginTransmission(0x04);
  if (Wire.endTransmission() == 0) {
    mics6814_available = true;
    Serial.println("✓ MICS-6814 detected (NO2, CO, NH3)");
  } else {
    Serial.println("✗ MICS-6814 not detected (expected - arriving Tuesday)");
  }
  
  // PMS5003 detection placeholder
  // Will be implemented when breakout board arrives
  Serial.println("✗ PMS5003 not detected (expected - needs breakout board)");
  
  Serial.println("--- Detection Complete ---\n");
}

// ============================================
// WIFI CONNECTION
// ============================================
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ WiFi Connection Failed");
    Serial.println("Will retry in main loop...");
  }
}

// ============================================
// MAIN LOOP - RUNS CONTINUOUSLY
// ============================================
void loop() {
  // Ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }
  
  // Read all available sensors
  String sensorData = readSensors();
  
  // Send data to Raspberry Pi
  sendDataToPi(sensorData);
  
  // Wait 60 seconds before next reading
  Serial.println("Waiting 60 seconds...\n");
  delay(60000);  // 60 seconds
}

// ============================================
// READ ALL SENSORS
// ============================================
String readSensors() {
  Serial.println("=== Reading Sensors ===");
  
  // Create JSON payload
  String json = "{";
  json += "\"node_id\":\"" + String(NODE_ID) + "\",";
  
  // BME680 readings (Temperature, Humidity, Pressure)
  if (bme680_available) {
    if (bme.performReading()) {
      float temp = bme.temperature;
      float humidity = bme.humidity;
      float pressure = bme.pressure / 100.0;  // Convert Pa to hPa
      
      json += "\"temperature\":" + String(temp, 1) + ",";
      json += "\"humidity\":" + String(humidity, 1) + ",";
      json += "\"pressure\":" + String(pressure, 1) + ",";
      
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" °C");
      
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
      
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" hPa");
    } else {
      json += "\"temperature\":null,";
      json += "\"humidity\":null,";
      json += "\"pressure\":null,";
      Serial.println("BME680 reading failed");
    }
  } else {
    json += "\"temperature\":null,";
    json += "\"humidity\":null,";
    json += "\"pressure\":null,";
  }
  
  // MQ-135 (Air Quality - reports as NO2 for now)
  // NOTE: This is a rough estimate. When MICS-6814 arrives, it will provide accurate NO2
  if (mq135_available) {
    int mq135_raw = analogRead(MQ135_PIN);
    float mq135_voltage = (mq135_raw / 4095.0) * 3.3;
    float no2_estimate = mq135_voltage * 20;  // Rough conversion
    
    json += "\"no2\":" + String(no2_estimate, 1) + ",";
    
    Serial.print("MQ-135 (NO2 estimate): ");
    Serial.print(no2_estimate);
    Serial.print(" µg/m³ (Raw: ");
    Serial.print(mq135_raw);
    Serial.println(")");
  } else {
    json += "\"no2\":null,";
  }
  
  // MQ-7 (Carbon Monoxide)
  if (mq7_available) {
    int mq7_raw = analogRead(MQ7_PIN);
    float mq7_voltage = (mq7_raw / 4095.0) * 3.3;
    float co_estimate = mq7_voltage * 10;  // Rough conversion
    
    json += "\"co\":" + String(co_estimate, 1) + ",";
    
    Serial.print("MQ-7 (CO): ");
    Serial.print(co_estimate);
    Serial.print(" ppm (Raw: ");
    Serial.print(mq7_raw);
    Serial.println(")");
  } else {
    json += "\"co\":null,";
  }
  
  // MQ-131 (Ozone)
  if (mq131_available) {
    int mq131_raw = analogRead(MQ131_PIN);
    float mq131_voltage = (mq131_raw / 4095.0) * 3.3;
    float o3_estimate = mq131_voltage * 50;  // Rough conversion
    
    json += "\"o3\":" + String(o3_estimate, 1) + ",";
    
    Serial.print("MQ-131 (O3): ");
    Serial.print(o3_estimate);
    Serial.print(" ppb (Raw: ");
    Serial.print(mq131_raw);
    Serial.println(")");
  } else {
    json += "\"o3\":null,";
  }
  
  // MICS-6814 (NO2, CO, NH3) - When sensor arrives Tuesday
  if (mics6814_available) {
    // Read oxidizing gas (NO2)
    Wire.beginTransmission(0x04);
    Wire.write(0x01); // Register for oxidizing gas
    Wire.endTransmission();
    Wire.requestFrom(0x04, 2);
    if (Wire.available() >= 2) {
      int no2_raw = Wire.read() << 8 | Wire.read();
      float no2_proper = no2_raw * 0.1;  // Conversion factor
      json += "\"no2\":" + String(no2_proper, 1) + ",";
      
      Serial.print("MICS-6814 (NO2): ");
      Serial.print(no2_proper);
      Serial.println(" µg/m³");
    }
    
    // Read reducing gas (CO)
    Wire.beginTransmission(0x04);
    Wire.write(0x02); // Register for reducing gas
    Wire.endTransmission();
    Wire.requestFrom(0x04, 2);
    if (Wire.available() >= 2) {
      int co_raw = Wire.read() << 8 | Wire.read();
      float co_proper = co_raw * 0.05;
      json += "\"co\":" + String(co_proper, 1) + ",";
      
      Serial.print("MICS-6814 (CO): ");
      Serial.print(co_proper);
      Serial.println(" ppm");
    }
    
    // Read ammonia
    Wire.beginTransmission(0x04);
    Wire.write(0x03); // Register for ammonia
    Wire.endTransmission();
    Wire.requestFrom(0x04, 2);
    if (Wire.available() >= 2) {
      int nh3_raw = Wire.read() << 8 | Wire.read();
      float nh3_proper = nh3_raw * 0.1;
      json += "\"nh3\":" + String(nh3_proper, 1) + ",";
      
      Serial.print("MICS-6814 (NH3): ");
      Serial.print(nh3_proper);
      Serial.println(" ppm");
    }
  }
  
  // Placeholder for sensors not yet connected
  if (!mics6814_available) {
    json += "\"nh3\":null,";
  }
  json += "\"pm25\":null,";
  json += "\"pm10\":null,";
  json += "\"so2\":null";
  
  json += "}";
  
  Serial.println("======================\n");
  
  return json;
}

// ============================================
// SEND DATA TO RASPBERRY PI
// ============================================
void sendDataToPi(String jsonData) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send data - WiFi not connected");
    return;
  }
  
  HTTPClient http;
  http.begin(PI_URL);
  http.addHeader("Content-Type", "application/json");
  
  Serial.println("Sending to Pi:");
  Serial.println(jsonData);
  
  int httpResponseCode = http.POST(jsonData);
  
  if (httpResponseCode > 0) {
    Serial.print("✓ Pi Response Code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.print("Pi Response: ");
    Serial.println(response);
  } else {
    Serial.print("✗ Error sending to Pi: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
