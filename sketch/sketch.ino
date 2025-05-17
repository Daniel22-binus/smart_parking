#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Firebase_ESP_Client.h>

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

// Animation state
int carX = 0;
int carSpeed = 2;

// conf wifi
#define WIFI_SSID "Yos"
#define WIFI_PASSWORD "12345678"
WiFiClientSecure ssl_client;

// conf firebase
#define API_KEY "AIzaSyB-7wTDQa5SimmdxtpV2D9m8WOiaEQ8L14"
#define DATABASE_URL "https://smart-parking-esp32-1c799-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define AUTH_EMAIL "daniel025@binus.ac.id"
#define AUTH_PASS "Test123"

// Objek Firebase dan Auth
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//firebase path
String path_slot_parking = "/smart-parking/slot-parking";
String path_payment = "/smart-parking/payment";


void initWifi() {
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Menyambung ke Wi-Fi...");
  }
  Serial.println("Tersambung ke Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure SSL client
  ssl_client.setInsecure();
}

void initFirebase() {
  // Setup Firebase config dan API key
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  //login
  auth.user.email = AUTH_EMAIL;
  auth.user.password = AUTH_PASS;

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
  Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot1"), 0);
  Firebase.RTDB.setInt(&fbdo, (path_slot_parking + "/slot2"), 0);
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
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  if (occupied == 0) {
    digitalWrite(LED_GREEN, HIGH);
  } else if (occupied == 1) {
    digitalWrite(LED_YELLOW, HIGH);
  } else {
    digitalWrite(LED_RED, HIGH);
  }
}

void drawCar(int x) {
  // Car body
  display.fillRoundRect(x, 40, 20, 8, 2, SSD1306_WHITE);
  // Wheels
  display.fillCircle(x + 4, 48, 2, SSD1306_BLACK);
  display.fillCircle(x + 16, 48, 2, SSD1306_BLACK);
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

  // Decide animation behavior
  if (occupied < 2) {
    carX += carSpeed;
    if (carX > SCREEN_WIDTH) carX = -25;
  }
  // If full, stop car (traffic jam)

  // Draw display
  display.clearDisplay();

  // Draw status on top
  display.setCursor(0, 0);
  display.print("Status: ");
  display.print(2 - occupied);
  display.print("/2 ");
  if (occupied == 0) display.print("(Empty)");
  else if (occupied == 1) display.print("(1 left)");
  else display.print("(Full)");

  // Draw multiple cars for fun!
  drawCar(carX);
  drawCar(carX - 30);
  drawCar(carX - 60);

  display.display();

  delay(150);
}
