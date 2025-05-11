#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Parking slot sensors
#define trigPinA 12
#define echoPinA 14
#define trigPinB 27
#define echoPinB 26

// Entrance detection
#define trigPinC 32
#define echoPinC 33

// Exit detection
#define trigPinD 25
#define echoPinD 34

// Servos
#define servoEntrance 13
#define servoExit     15

// LCD: I2C addr 0x27, 16 chars, 2 lines
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo gateServoEntrance;
Servo gateServoExit;

const int OCCUPIED_THRESHOLD = 15;
const int GATE_TRIGGER_DISTANCE = 15;

bool isOccupiedA = false, prevStateA = false;
bool isOccupiedB = false, prevStateB = false;

unsigned long lastChangeTimeA = 0;
unsigned long lastChangeTimeB = 0;

void setup() {
  Serial.begin(115200);
  
  // Ultrasonic pin modes
  pinMode(trigPinA, OUTPUT); pinMode(echoPinA, INPUT);
  pinMode(trigPinB, OUTPUT); pinMode(echoPinB, INPUT);
  pinMode(trigPinC, OUTPUT); pinMode(echoPinC, INPUT);
  pinMode(trigPinD, OUTPUT); pinMode(echoPinD, INPUT);

  // Attach servos
  gateServoEntrance.attach(servoEntrance);
  gateServoExit.attach(servoExit);
  gateServoEntrance.write(0);  // Closed
  gateServoExit.write(0);      // Closed

  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Parking System");
}

void loop() {
  // Distance readings
  int distanceA = getDistance(trigPinA, echoPinA);
  int distanceB = getDistance(trigPinB, echoPinB);
  int gateEntranceDist = getDistance(trigPinC, echoPinC);
  int gateExitDist     = getDistance(trigPinD, echoPinD);

  // Slot states
  isOccupiedA = (distanceA < OCCUPIED_THRESHOLD);
  isOccupiedB = (distanceB < OCCUPIED_THRESHOLD);

  bool carAtEntrance = (gateEntranceDist < GATE_TRIGGER_DISTANCE);
  bool carAtExit     = (gateExitDist < GATE_TRIGGER_DISTANCE);

  int availableSlots = (!isOccupiedA) + (!isOccupiedB);

  // Display slot status on Serial
  Serial.println("-----------------------------");
  displaySlotStatus("A", isOccupiedA, lastChangeTimeA, prevStateA);
  displaySlotStatus("B", isOccupiedB, lastChangeTimeB, prevStateB);

  Serial.print("Available Slots: "); Serial.println(availableSlots);
  Serial.print("Car at Entrance: "); Serial.println(carAtEntrance ? "YES" : "NO");
  Serial.print("Car at Exit    : "); Serial.println(carAtExit ? "YES" : "NO");

  // Gate logic
  if (availableSlots > 0 && carAtEntrance) {
    gateServoEntrance.write(90);  // Open entrance
    Serial.println("Entrance Gate: OPEN");
  } else {
    gateServoEntrance.write(0);
    Serial.println("Entrance Gate: CLOSED");
  }

  if (carAtExit) {
    gateServoExit.write(90);  // Open exit
    Serial.println("Exit Gate: OPEN");
  } else {
    gateServoExit.write(0);
    Serial.println("Exit Gate: CLOSED");
  }

  // Update LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(isOccupiedA ? "OCC" : "AVL");  // OCC = Occupied, AVL = Available
  lcd.print(" B:");
  lcd.print(isOccupiedB ? "OCC" : "AVL");

  lcd.setCursor(0, 1);
  if (availableSlots == 0) {
    lcd.print("Status: FULL");
  } else {
    lcd.print("Available: ");
    lcd.print(availableSlots);
  }

  delay(2000);
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  int distance = duration * 0.034 / 2;
  return distance;
}

void displaySlotStatus(String slot, bool state, unsigned long &lastTime, bool &prevState) {
  if (state != prevState) {
    lastTime = millis();
    prevState = state;
    Serial.print("Slot ");
    Serial.print(slot);
    Serial.print(" changed to: ");
    Serial.println(state ? "OCCUPIED" : "VACANT");
  }

  unsigned long duration = (millis() - lastTime) / 1000;
  Serial.print("Slot ");
  Serial.print(slot);
  Serial.print(": ");
  Serial.print(state ? "OCCUPIED" : "VACANT");
  Serial.print(" for ");
  Serial.print(duration);
  Serial.println(" sec");
}
