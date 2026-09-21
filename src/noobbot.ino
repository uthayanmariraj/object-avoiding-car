#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

const int ENA_PIN = 5;
const int IN1_PIN = 18;
const int IN2_PIN = 19;

const int ENB_PIN = 21;
const int IN3_PIN = 22;
const int IN4_PIN = 23;

const int PWM_FREQ = 2000;
const int PWM_RESOLUTION = 8;
const int ENA_CHANNEL = 0;
const int ENB_CHANNEL = 1;

const int LEFT_SLOT = 34;
const int RIGHT_SLOT = 35;

volatile long leftCount = 0;
volatile long rightCount = 0;

constexpr int MOTOR_SPEED = 185;
constexpr float PULSES_PER_CM = 2.0;
constexpr float PULSES_PER_DEG = 0.295;

constexpr float SPECIAL_MOVE_DISTANCE_CM = 60.0;
constexpr float FIRST_SEGMENT_CM = 25.0;
constexpr float SECOND_SEGMENT_CM = 30.0;
constexpr float SPECIAL_TURN_DEG = 20.0;

void IRAM_ATTR leftISR() { leftCount++; }
void IRAM_ATTR rightISR() { rightCount++; }

void setMotorLeft(int speed, bool forward) {
  digitalWrite(IN1_PIN, forward ? HIGH : LOW);
  digitalWrite(IN2_PIN, forward ? LOW : HIGH);
  ledcWrite(ENA_CHANNEL, speed);
}

void setMotorRight(int speed, bool forward) {
  digitalWrite(IN3_PIN, forward ? HIGH : LOW);
  digitalWrite(IN4_PIN, forward ? LOW : HIGH);
  ledcWrite(ENB_CHANNEL, speed);
}

void stopMotors() {
  ledcWrite(ENA_CHANNEL, 0);
  ledcWrite(ENB_CHANNEL, 0);
}

void resetEncoders() {
  leftCount = 0;
  rightCount = 0;
}

void moveCM(float cm, int speed) {
  int target = cm * PULSES_PER_CM;
  resetEncoders();

  setMotorLeft(speed, true);
  setMotorRight(speed, true);

  while (leftCount < target || rightCount < target) {
    if (leftCount >= target) ledcWrite(ENA_CHANNEL, 0);
    if (rightCount >= target) ledcWrite(ENB_CHANNEL, 0);
  }
  stopMotors();
}

void turnDeg(float deg, bool leftTurn) {
  int target = deg * PULSES_PER_DEG;
  resetEncoders();

  if (leftTurn) {
    setMotorLeft(MOTOR_SPEED, true);
    setMotorRight(MOTOR_SPEED, false);
  } else {
    setMotorLeft(MOTOR_SPEED, false);
    setMotorRight(MOTOR_SPEED, true);
  }

  while (leftCount < target || rightCount < target) {
    if (leftCount >= target) ledcWrite(ENA_CHANNEL, 0);
    if (rightCount >= target) ledcWrite(ENB_CHANNEL, 0);
  }

  stopMotors();
}

bool wasConnected = false;
String incoming = "";

void setup() {
  Serial.begin(115200);
  SerialBT.begin("noobbot");

  pinMode(IN1_PIN, OUTPUT); pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT); pinMode(IN4_PIN, OUTPUT);

  pinMode(LEFT_SLOT, INPUT_PULLUP);
  pinMode(RIGHT_SLOT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LEFT_SLOT), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_SLOT), rightISR, RISING);

  ledcSetup(ENA_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(ENB_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA_PIN, ENA_CHANNEL);
  ledcAttachPin(ENB_PIN, ENB_CHANNEL);

  Serial.println("Waiting for BT client...");
}

void loop() {
  if (SerialBT.hasClient() && !wasConnected) {
    Serial.println("Client connected");
    SerialBT.println("REQ");
    wasConnected = true;
  }

  if (!SerialBT.hasClient() && wasConnected) {
    stopMotors();
    Serial.println("Client disconnected");
    wasConnected = false;
  }

  if (SerialBT.available()) {
    incoming = SerialBT.readStringUntil('\n');
    incoming.trim();
    Serial.println("RX: " + incoming);

    if (incoming.startsWith("CMD:MOVE:")) {
      float dist = incoming.substring(9).toFloat();

      if (dist == SPECIAL_MOVE_DISTANCE_CM) {
        moveCM(FIRST_SEGMENT_CM, MOTOR_SPEED);
        turnDeg(SPECIAL_TURN_DEG, true);
        moveCM(SECOND_SEGMENT_CM, MOTOR_SPEED);
        turnDeg(SPECIAL_TURN_DEG, true);
      } else {
        moveCM(dist, MOTOR_SPEED);
        turnDeg(SPECIAL_TURN_DEG, true);
      }
    } else if (incoming.startsWith("CMD:TURN:")) {
      char direction = incoming.charAt(9);
      float deg = incoming.substring(incoming.lastIndexOf(':') + 1).toFloat();
      turnDeg(deg, direction == 'L');
    }

    SerialBT.println("REQ");
  }
}
