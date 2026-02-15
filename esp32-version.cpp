#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Pin definitions
const int trigPin = 25;
const int echoPin = 26;
const int servoPin = 27;
const int buzzerPin = 14;
const int btnLeft = 32;   
const int btnRight = 33;  

// TFT pins
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

Servo myServo;
int angle = 90;
int step = 1; 
const int alarmDistance = 20;

// Buzzer PWM channel
const int buzzerChannel = 15;

// Radar display parameters
const int centerX = 160;
const int centerY = 200;
const int maxRadius = 120;
const int numCircles = 4;
int distanceHistory[181]; // Store distances for each angle

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  
  myServo.attach(servoPin);
  
  // Setup buzzer
  ledcAttachChannel(buzzerPin, 1500, 8, buzzerChannel);
  
  Serial.begin(9600);
  
  // Initialize TFT
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  
  // Initialize distance history
  for (int i = 0; i < 181; i++) {
    distanceHistory[i] = 400;
  }
  
  drawRadarBase();
}

void loop() {
  // Manual Override
  if (digitalRead(btnLeft) == LOW) {
    if (angle < 180) step = 1; 
  } 
  else if (digitalRead(btnRight) == LOW) {
    if (angle > 0) step = -1;
  }

  // Auto-Update Angle
  angle += step;

  // Limits Check
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
  
  // Store distance in history
  distanceHistory[angle] = distance;
  
  // Buzzer Logic
  if (distance > 0 && distance < alarmDistance) {
    ledcWriteTone(buzzerPin, 1500);
  } else {
    ledcWrite(buzzerPin, 0);
  }

  // Update radar display
  updateRadarDisplay(angle, distance);

  // Send data to Serial (for debugging)
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
  if (duration == 0) return 400;
  return duration * 0.034 / 2;
}

void drawRadarBase() {
  tft.fillScreen(ILI9341_BLACK);
  
  // Draw concentric circles
  for (int i = 1; i <= numCircles; i++) {
    int radius = (maxRadius * i) / numCircles;
    tft.drawCircle(centerX, centerY, radius, 0x0320); // Dark green
  }
  
  // Draw angle lines (every 30 degrees) - 0° is LEFT, 180° is RIGHT
  for (int angle = 0; angle <= 180; angle += 30) {
    float rad = angle * PI / 180.0;
    int x = centerX - maxRadius * cos(rad);
    int y = centerY - maxRadius * sin(rad);
    tft.drawLine(centerX, centerY, x, y, 0x0320); // Dark green
  }
  
  // Draw labels
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.print("RADAR SYSTEM");
  
  // Distance markers
  for (int i = 1; i <= numCircles; i++) {
    int dist = (100 * i) / numCircles;
    tft.setCursor(centerX + 2, centerY - (maxRadius * i) / numCircles - 4);
    tft.print(dist);
    tft.print("cm");
  }
  
  // Angle markers (0° is LEFT, 90° is UP, 180° is RIGHT)
  tft.setCursor(centerX - 15, centerY - maxRadius - 10);
  tft.print("90");
  tft.setCursor(centerX - maxRadius - 20, centerY - 5);
  tft.print("0");
  tft.setCursor(centerX + maxRadius + 5, centerY - 5);
  tft.print("180");
}

void updateRadarDisplay(int currentAngle, int distance) {
  // Clear old sweep line (draw it in very dark green to create fade effect)
  float oldRad = (currentAngle - 2) * PI / 180.0;
  int oldX = centerX - maxRadius * cos(oldRad);
  int oldY = centerY - maxRadius * sin(oldRad);
  tft.drawLine(centerX, centerY, oldX, oldY, ILI9341_BLACK);
  
  // Redraw grid line if needed
  if (currentAngle % 30 == 0) {
    float rad = currentAngle * PI / 180.0;
    int x = centerX - maxRadius * cos(rad);
    int y = centerY - maxRadius * sin(rad);
    tft.drawLine(centerX, centerY, x, y, 0x0320);
  }
  
  // Draw object detection point (red dot)
  if (distance < 100) {
    float rad = currentAngle * PI / 180.0;
    int objRadius = map(distance, 0, 100, 0, maxRadius);
    int objX = centerX - objRadius * cos(rad);
    int objY = centerY - objRadius * sin(rad);
    tft.fillCircle(objX, objY, 2, ILI9341_RED);
  }
  
  // Draw scan line (bright green)
  float rad = currentAngle * PI / 180.0;
  int x = centerX - maxRadius * cos(rad);
  int y = centerY - maxRadius * sin(rad);
  tft.drawLine(centerX, centerY, x, y, ILI9341_GREEN);
  
  // Draw fade effect (darker lines behind the sweep)
  for (int i = 1; i <= 20; i++) {
    int fadeAngle = currentAngle - i;
    if (fadeAngle >= 0 && fadeAngle <= 180) {
      float fadeRad = fadeAngle * PI / 180.0;
      int fadeX = centerX - maxRadius * cos(fadeRad);
      int fadeY = centerY - maxRadius * sin(fadeRad);
      uint16_t fadeColor = tft.color565(0, max(0, 255 - i * 12), 0);
      tft.drawLine(centerX, centerY, fadeX, fadeY, fadeColor);
      
      // Redraw object point if in fade zone
      if (distanceHistory[fadeAngle] < 100) {
        int objRadius = map(distanceHistory[fadeAngle], 0, 100, 0, maxRadius);
        int objX = centerX - objRadius * cos(fadeRad);
        int objY = centerY - objRadius * sin(fadeRad);
        uint16_t objColor = tft.color565(255 - i * 10, 0, 0);
        tft.fillCircle(objX, objY, 2, objColor);
      }
    }
  }
  
  // Display distance and angle info
  tft.fillRect(0, 300, 320, 20, ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.setCursor(10, 305);
  tft.print("Angle: ");
  tft.print(currentAngle);
  tft.print("  Dist: ");
  tft.print(distance);
  tft.print("cm");
}






// #include <ESP32Servo.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_ILI9341.h>
// #include <SPI.h>

// // Pin definitions
// const int trigPin = 25;
// const int echoPin = 26;
// const int servoPin = 27;
// const int buzzerPin = 14;
// const int btnLeft = 32;   
// const int btnRight = 33;  

// // TFT pins
// #define TFT_CS 5
// #define TFT_DC 2
// #define TFT_RST 4

// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Servo myServo;
// int angle = 90;
// int step = 1; 
// const int alarmDistance = 20;

// // Buzzer PWM channel
// const int buzzerChannel = 15;

// // Radar display parameters
// const int centerX = 160;
// const int centerY = 200;
// const int maxRadius = 120;
// const int numCircles = 4;
// int distanceHistory[181]; // Store distances for each angle

// void setup() {
//   pinMode(trigPin, OUTPUT);
//   pinMode(echoPin, INPUT);
//   pinMode(btnLeft, INPUT_PULLUP);
//   pinMode(btnRight, INPUT_PULLUP);
  
//   myServo.attach(servoPin);
  
//   // Setup buzzer
//   ledcAttachChannel(buzzerPin, 1500, 8, buzzerChannel);
  
//   Serial.begin(9600);
  
//   // Initialize TFT
//   tft.begin();
//   tft.setRotation(0);
//   tft.fillScreen(ILI9341_BLACK);
  
//   // Initialize distance history
//   for (int i = 0; i < 181; i++) {
//     distanceHistory[i] = 400;
//   }
  
//   drawRadarBase();
// }

// void loop() {
//   // Manual Override
//   if (digitalRead(btnLeft) == LOW) {
//     if (angle < 180) step = 1; 
//   } 
//   else if (digitalRead(btnRight) == LOW) {
//     if (angle > 0) step = -1;
//   }

//   // Auto-Update Angle
//   angle += step;

//   // Limits Check
//   if (angle >= 180) {
//     angle = 180;
//     step = -1; 
//   } else if (angle <= 0) {
//     angle = 0;
//     step = 1;  
//   }

//   myServo.write(angle);
  
//   // Calculate distance
//   int distance = getDistance();
  
//   // Store distance in history
//   distanceHistory[angle] = distance;
  
//   // Buzzer Logic
//   if (distance > 0 && distance < alarmDistance) {
//     ledcWriteTone(buzzerPin, 1500);
//   } else {
//     ledcWrite(buzzerPin, 0);
//   }

//   // Update radar display
//   updateRadarDisplay(angle, distance);

//   // Send data to Serial (for debugging)
//   Serial.print(angle);
//   Serial.print(",");
//   Serial.print(distance);
//   Serial.println("."); 

//   delay(30); 
// }

// int getDistance() {
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);
  
//   long duration = pulseIn(echoPin, HIGH, 25000); 
//   if (duration == 0) return 400;
//   return duration * 0.034 / 2;
// }

// void drawRadarBase() {
//   tft.fillScreen(ILI9341_BLACK);
  
//   // Draw concentric circles
//   for (int i = 1; i <= numCircles; i++) {
//     int radius = (maxRadius * i) / numCircles;
//     tft.drawCircle(centerX, centerY, radius, 0x0320); // Dark green
//   }
  
//   // Draw angle lines (every 30 degrees)
//   for (int angle = 0; angle <= 180; angle += 30) {
//     float rad = angle * PI / 180.0;
//     int x = centerX + maxRadius * cos(rad - PI/2);
//     int y = centerY + maxRadius * sin(rad - PI/2);
//     tft.drawLine(centerX, centerY, x, y, 0x0320); // Dark green
//   }
  
//   // Draw labels
//   tft.setTextColor(ILI9341_GREEN);
//   tft.setTextSize(1);
//   tft.setCursor(5, 5);
//   tft.print("RADAR SYSTEM");
  
//   // Distance markers
//   for (int i = 1; i <= numCircles; i++) {
//     int dist = (100 * i) / numCircles;
//     tft.setCursor(centerX + 2, centerY - (maxRadius * i) / numCircles - 4);
//     tft.print(dist);
//     tft.print("cm");
//   }
  
//   // Angle markers
//   tft.setCursor(centerX - 15, centerY + maxRadius + 5);
//   tft.print("90");
//   tft.setCursor(10, centerY - 5);
//   tft.print("0");
//   tft.setCursor(300, centerY - 5);
//   tft.print("180");
// }

// void updateRadarDisplay(int currentAngle, int distance) {
//   // Clear old sweep line (draw it in very dark green to create fade effect)
//   float oldRad = (currentAngle - 2) * PI / 180.0;
//   int oldX = centerX + maxRadius * cos(oldRad - PI/2);
//   int oldY = centerY + maxRadius * sin(oldRad - PI/2);
//   tft.drawLine(centerX, centerY, oldX, oldY, ILI9341_BLACK);
  
//   // Redraw grid line if needed
//   if (currentAngle % 30 == 0) {
//     float rad = currentAngle * PI / 180.0;
//     int x = centerX + maxRadius * cos(rad - PI/2);
//     int y = centerY + maxRadius * sin(rad - PI/2);
//     tft.drawLine(centerX, centerY, x, y, 0x0320);
//   }
  
//   // Draw object detection point (red dot)
//   if (distance < 100) {
//     float rad = currentAngle * PI / 180.0;
//     int objRadius = map(distance, 0, 100, 0, maxRadius);
//     int objX = centerX + objRadius * cos(rad - PI/2);
//     int objY = centerY + objRadius * sin(rad - PI/2);
//     tft.fillCircle(objX, objY, 2, ILI9341_RED);
//   }
  
//   // Draw scan line (bright green)
//   float rad = currentAngle * PI / 180.0;
//   int x = centerX + maxRadius * cos(rad - PI/2);
//   int y = centerY + maxRadius * sin(rad - PI/2);
//   tft.drawLine(centerX, centerY, x, y, ILI9341_GREEN);
  
//   // Draw fade effect (darker lines behind the sweep)
//   for (int i = 1; i <= 20; i++) {
//     int fadeAngle = currentAngle - i;
//     if (fadeAngle >= 0 && fadeAngle <= 180) {
//       float fadeRad = fadeAngle * PI / 180.0;
//       int fadeX = centerX + maxRadius * cos(fadeRad - PI/2);
//       int fadeY = centerY + maxRadius * sin(fadeRad - PI/2);
//       uint16_t fadeColor = tft.color565(0, max(0, 255 - i * 12), 0);
//       tft.drawLine(centerX, centerY, fadeX, fadeY, fadeColor);
      
//       // Redraw object point if in fade zone
//       if (distanceHistory[fadeAngle] < 100) {
//         int objRadius = map(distanceHistory[fadeAngle], 0, 100, 0, maxRadius);
//         int objX = centerX + objRadius * cos(fadeRad - PI/2);
//         int objY = centerY + objRadius * sin(fadeRad - PI/2);
//         uint16_t objColor = tft.color565(255 - i * 10, 0, 0);
//         tft.fillCircle(objX, objY, 2, objColor);
//       }
//     }
//   }
  
//   // Display distance and angle info
//   tft.fillRect(0, 300, 320, 20, ILI9341_BLACK);
//   tft.setTextColor(ILI9341_GREEN);
//   tft.setTextSize(1);
//   tft.setCursor(10, 305);
//   tft.print("Angle: ");
//   tft.print(currentAngle);
//   tft.print("  Dist: ");
//   tft.print(distance);
//   tft.print("cm");
// }