#include <Servo.h>

const int trigPin = 7;
const int echoPin = 6;
const int servoPin = 9;
const int buzzerPin = 8;
const int btnLeft = 2;   
const int btnRight = 3;  

Servo myServo;
int angle = 90;
int step = 1; 
const int alarmDistance = 20; // Try changing this to 100 to test easier

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  
  myServo.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  // Manual Override: Force direction based on buttons
  if (digitalRead(btnLeft) == LOW) {
    if (angle < 180) step = 1; 
  } 
  else if (digitalRead(btnRight) == LOW) {
    if (angle > 0) step = -1;
  }

  // Auto-Update Angle
  angle += step;

  // Limits Check: Reverse direction automatically at ends
  if (angle >= 180) {
    angle = 180;
    step = -1; 
  } else if (angle <= 0) {
    angle = 0;
    step = 1;  
  }

  myServo.write(angle);
  
  // Calculate distance
  int distance = getDistance();
  
  // Buzzer Logic
  if (distance > 0 && distance < alarmDistance) {
    tone(buzzerPin, 1500); // 1500Hz frequency is very audible
  } else {
    noTone(buzzerPin);
  }

  // Send data to the Radar UI format
  Serial.print(angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.println("."); 

  delay(30); 
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 25000); 
  if (duration == 0) return 400; // No object detected
  return duration * 0.034 / 2;
}