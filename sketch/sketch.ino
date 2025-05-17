#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// FirebaseFS.h
#define ENABLE_RTDB
#undef ENABLE_FIRESTORE
#undef ENABLE_STORAGE
#undef ENABLE_MESSAGING
#undef ENABLE_FCM


// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Ultrasonic Pins
#define TRIG_A 4
#define ECHO_A 14
#define TRIG_B 27
#define ECHO_B 26

// LED Pins
#define LED_RED    25
#define LED_YELLOW 33
#define LED_GREEN  32

WiFiClientSecure ssl_client;

// Objek Firebase dan Auth
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//firebase path
String path_slot_parking = "/smart-parking/slot-parking";

void initWifi() {
  // Connect to Wi-Fi
  WiFi.begin("Yos", "12345678");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Menyambung ke Wi-Fi...");
  }
  Serial.println("Tersambung ke Wi-Fi!");
}

void initFirebase() {
  // Setup Firebase config dan API key
  config.api_key = "AIzaSyB-7wTDQa5SimmdxtpV2D9m8WOiaEQ8L14";
  config.database_url = "https://smart-parking-esp32-1c799-default-rtdb.asia-southeast1.firebasedatabase.app/";

  //login
  auth.user.email = "daniel025@binus.ac.id";
  auth.user.password = "Test123";

  // Inisialisasi Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  initWifi();
  initFirebase();
}

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void updateTrafficLight(int occupied) {
  digitalWrite(LED_RED,    occupied == 2);
  digitalWrite(LED_YELLOW, occupied == 1);
  digitalWrite(LED_GREEN,  occupied == 0);
}


void loop() {
  long distA = readDistanceCM(TRIG_A, ECHO_A);
  long distB = readDistanceCM(TRIG_B, ECHO_B);

  int occupied = 0;
  if (distA > 0 && distA < 15) {
    Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot1"), 1);
    occupied++;
  } else {
    Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot1"), 0);
  }

  if (distB > 0 && distB < 15) {
    Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot2"), 1);
    occupied++;
  } else {
    Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot2"), 0);
  }

  updateTrafficLight(occupied);
  // Draw display
  display.clearDisplay();

  // Draw status on top
  display.setCursor(0, 0);
  display.print(F("Status: "));
  display.print(2 - occupied);
  display.print(F("/2 "));
  if (occupied == 0) display.print(F("(Empty)"));
  else if (occupied == 1) display.print(F("(1 left)"));
  else display.print(F("(Full)"));

  display.display();


  delay(500);
}
