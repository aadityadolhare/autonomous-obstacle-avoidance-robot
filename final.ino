#include <Servo.h>
#include <LiquidCrystal.h>

/*
  ============================================================
      AUTONOMOUS OBSTACLE DETECTION & AVOIDANCE ROBOT
  ============================================================

  Controller      : Arduino Uno
  Motor Driver    : L293D
  Distance Sensor : HC-SR04
  Servo Scanner   : Positional Micro Servo
  Display         : 16x2 LCD (4-bit mode)

  Features:
  - Continuous obstacle detection
  - Automatic stop and reverse
  - Servo-assisted left/right scanning
  - Clearer-path selection
  - Both-paths-blocked recovery
  - Averaged ultrasonic readings
  - Invalid sensor reading protection
  - LCD status display
  - Serial Monitor diagnostics

  ============================================================
*/

// ============================================================
// 1. PIN CONFIGURATION
// ============================================================

// L293D Motor Driver
const int MOTOR1_A = 3;
const int MOTOR1_B = 5;
const int MOTOR2_A = 4;
const int MOTOR2_B = 2;

// HC-SR04 Ultrasonic Sensor
const int TRIG_PIN = 13;
const int ECHO_PIN = 12;

// Servo
const int SERVO_PIN = 8;

// 16x2 LCD in 4-bit mode
// RS -> D6, E -> D7, DB4 -> D9, DB5 -> D10,
// DB6 -> D11, DB7 -> A0
LiquidCrystal lcd(6, 7, 9, 10, 11, A0);

// ============================================================
// 2. ROBOT PARAMETERS
// ============================================================

const float OBSTACLE_THRESHOLD = 25.0;      // cm
const float MIN_ESCAPE_DISTANCE = 15.0;     // cm

const int SERVO_CENTER = 90;
const int SERVO_LEFT   = 160;
const int SERVO_RIGHT  = 20;

const unsigned long REVERSE_TIME = 500;      // ms
const unsigned long TURN_TIME    = 600;      // ms

const int SENSOR_SAMPLES = 3;
const float MAX_DISTANCE = 250.0;             // cm

Servo scannerServo;

// ============================================================
// 3. SETUP
// ============================================================

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);
  pinMode(MOTOR2_A, OUTPUT);
  pinMode(MOTOR2_B, OUTPUT);

  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  scannerServo.attach(SERVO_PIN);
  scannerServo.write(SERVO_CENTER);

  // LCD
  lcd.begin(16, 2);
  showLCD("OBSTACLE ROBOT", "SYSTEM READY");

  // Safe initial state
  stopRobot();

  Serial.println();
  Serial.println("================================================");
  Serial.println("     AUTONOMOUS OBSTACLE AVOIDANCE ROBOT");
  Serial.println("================================================");
  Serial.println("System Initialization");
  Serial.println("---------------------");
  Serial.println("Arduino Controller : READY");
  Serial.println("HC-SR04 Sensor     : READY");
  Serial.println("L293D Motor Driver : READY");
  Serial.println("Servo Scanner      : READY");
  Serial.println("LCD Display        : READY");
  Serial.println("---------------------");
  Serial.print("Obstacle Threshold : ");
  Serial.print(OBSTACLE_THRESHOLD);
  Serial.println(" cm");
  Serial.print("Minimum Escape    : ");
  Serial.print(MIN_ESCAPE_DISTANCE);
  Serial.println(" cm");
  Serial.println("---------------------");
  Serial.println("Robot Ready.");
  Serial.println("================================================");
  Serial.println();

  delay(1500);
}

// ============================================================
// 4. MAIN CONTROL LOOP
// ============================================================

void loop() {
  // Keep sensor facing forward
  scannerServo.write(SERVO_CENTER);
  delay(100);

  float frontDistance = readAverageDistance();

  // ----------------------------------------------------------
  // Invalid sensor reading
  // ----------------------------------------------------------
  if (frontDistance < 0) {
    stopRobot();
    showLCD("SENSOR ERROR", "ROBOT STOPPED");

    Serial.println("[SAFETY] Invalid ultrasonic reading");
    Serial.println("[ACTION] STOP - Waiting for valid reading");
    Serial.println();

    delay(500);
    return;
  }

  // ----------------------------------------------------------
  // Clear path
  // ----------------------------------------------------------
  if (frontDistance > OBSTACLE_THRESHOLD) {
    moveForward();
    showLCDDistance(frontDistance, "CLEAR - FORWARD");

    Serial.print("[SENSING] Front Distance: ");
    Serial.print(frontDistance, 1);
    Serial.println(" cm | Status: CLEAR | Action: FORWARD");

    delay(200);
    return;
  }

  // ----------------------------------------------------------
  // Obstacle detected
  // ----------------------------------------------------------
  stopRobot();
  showLCDDistance(frontDistance, "OBSTACLE DET");

  Serial.print("[SENSING] Front Distance: ");
  Serial.print(frontDistance, 1);
  Serial.println(" cm | Status: OBSTACLE DETECTED");
  Serial.println("[ACTION] STOP");

  delay(300);

  // ----------------------------------------------------------
  // Reverse
  // ----------------------------------------------------------
  showLCD("ACTION: REVERSE", "BACKING UP");
  moveBackward();

  Serial.println("[ACTION] REVERSE");
  delay(REVERSE_TIME);

  stopRobot();
  delay(300);

  // ----------------------------------------------------------
  // Scan left and right
  // ----------------------------------------------------------
  Serial.println("[SCAN] Checking alternative paths...");

  showLCD("SCANNING...", "LEFT");
  float leftDistance = scanLeft();
  delay(200);

  showLCD("SCANNING...", "RIGHT");
  float rightDistance = scanRight();
  delay(200);

  scannerServo.write(SERVO_CENTER);
  delay(300);

  // ----------------------------------------------------------
  // Serial scan results
  // ----------------------------------------------------------
  Serial.println("----------------------------------------");

  Serial.print("[SCAN RESULT] LEFT  : ");
  if (leftDistance < 0) {
    Serial.println("INVALID");
  } else {
    Serial.print(leftDistance, 1);
    Serial.println(" cm");
  }

  Serial.print("[SCAN RESULT] RIGHT : ");
  if (rightDistance < 0) {
    Serial.println("INVALID");
  } else {
    Serial.print(rightDistance, 1);
    Serial.println(" cm");
  }

  // ----------------------------------------------------------
  // Both side readings invalid
  // ----------------------------------------------------------
  if (leftDistance < 0 && rightDistance < 0) {
    showLCD("INVALID SCAN", "REVERSE+RESCAN");

    Serial.println("[DECISION] Both side readings invalid");
    Serial.println("[ACTION] REVERSE + RESCAN");

    moveBackward();
    delay(REVERSE_TIME);
    stopRobot();
    delay(300);
    return;
  }

  // Invalid side is treated as unsafe/blocked
  if (leftDistance < 0) {
    leftDistance = 0;
  }

  if (rightDistance < 0) {
    rightDistance = 0;
  }

  // ----------------------------------------------------------
  // Both paths blocked
  // ----------------------------------------------------------
  if (leftDistance < MIN_ESCAPE_DISTANCE &&
      rightDistance < MIN_ESCAPE_DISTANCE) {

    showLCD("BOTH BLOCKED", "REVERSE+RESCAN");

    Serial.println("[DECISION] BOTH PATHS BLOCKED");
    Serial.println("[ACTION] REVERSE + RESCAN");

    moveBackward();
    delay(REVERSE_TIME);
    stopRobot();
    delay(300);
    return;
  }

  // ----------------------------------------------------------
  // Select clearer path
  // ----------------------------------------------------------
  if (leftDistance > rightDistance) {
    showLCD("PATH: LEFT", "TURNING LEFT");

    Serial.println("[DECISION] LEFT path is clearer");
    Serial.println("[ACTION] TURN LEFT");

    turnLeft();
    delay(TURN_TIME);
  }
  else if (rightDistance > leftDistance) {
    showLCD("PATH: RIGHT", "TURNING RIGHT");

    Serial.println("[DECISION] RIGHT path is clearer");
    Serial.println("[ACTION] TURN RIGHT");

    turnRight();
    delay(TURN_TIME);
  }
  else {
    showLCD("PATHS EQUAL", "DEFAULT: RIGHT");

    Serial.println("[DECISION] Both paths are equal");
    Serial.println("[ACTION] DEFAULT -> TURN RIGHT");

    turnRight();
    delay(TURN_TIME);
  }

  // ----------------------------------------------------------
  // Resume forward motion
  // ----------------------------------------------------------
  stopRobot();
  delay(200);

  showLCD("ROBOT MOVING", "FORWARD");

  Serial.println("[ACTION] RESUME FORWARD");
  Serial.println("----------------------------------------");
  Serial.println();

  delay(200);
}

// ============================================================
// 5. SINGLE ULTRASONIC DISTANCE READING
// ============================================================

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  if (distance <= 0 || distance > MAX_DISTANCE) {
    return -1;
  }

  return distance;
}

// ============================================================
// 6. AVERAGED ULTRASONIC READING
// ============================================================

float readAverageDistance() {
  float total = 0;
  int validReadings = 0;

  for (int i = 0; i < SENSOR_SAMPLES; i++) {
    float distance = readDistance();

    if (distance >= 0) {
      total += distance;
      validReadings++;
    }

    delay(25);
  }

  if (validReadings == 0) {
    return -1;
  }

  return total / validReadings;
}

// ============================================================
// 7. SERVO SCANNING
// ============================================================

float scanLeft() {
  Serial.println("[SCAN] Servo -> LEFT");
  scannerServo.write(SERVO_LEFT);
  delay(700);
  return readAverageDistance();
}

float scanRight() {
  Serial.println("[SCAN] Servo -> RIGHT");
  scannerServo.write(SERVO_RIGHT);
  delay(700);
  return readAverageDistance();
}

// ============================================================
// 8. MOTOR CONTROL
// ============================================================

void moveForward() {
  digitalWrite(MOTOR1_A, HIGH);
  digitalWrite(MOTOR1_B, LOW);
  digitalWrite(MOTOR2_A, HIGH);
  digitalWrite(MOTOR2_B, LOW);
}

void moveBackward() {
  digitalWrite(MOTOR1_A, LOW);
  digitalWrite(MOTOR1_B, HIGH);
  digitalWrite(MOTOR2_A, LOW);
  digitalWrite(MOTOR2_B, HIGH);
}

void turnLeft() {
  digitalWrite(MOTOR1_A, LOW);
  digitalWrite(MOTOR1_B, HIGH);
  digitalWrite(MOTOR2_A, HIGH);
  digitalWrite(MOTOR2_B, LOW);
}

void turnRight() {
  digitalWrite(MOTOR1_A, HIGH);
  digitalWrite(MOTOR1_B, LOW);
  digitalWrite(MOTOR2_A, LOW);
  digitalWrite(MOTOR2_B, HIGH);
}

void stopRobot() {
  digitalWrite(MOTOR1_A, LOW);
  digitalWrite(MOTOR1_B, LOW);
  digitalWrite(MOTOR2_A, LOW);
  digitalWrite(MOTOR2_B, LOW);
}

// ============================================================
// 9. LCD DISPLAY HELPERS
// ============================================================

void showLCD(const char* line1, const char* line2) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void showLCDDistance(float distance, const char* status) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("DIST:");
  lcd.print(distance, 1);
  lcd.print("cm");

  lcd.setCursor(0, 1);
  lcd.print(status);
}
