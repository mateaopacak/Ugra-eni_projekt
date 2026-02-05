
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;


const int btnUp = 2;
const int btnSet = 3;
const int buzzerPin = 8;


int setHour = 0;
int setMin = 0;
int setSec = 0;
unsigned long intervalSeconds = 0;
unsigned long lastDispenseTime = 0;
bool intervalSet = false;
int mode = 0; 


bool btnUpPressed = false;
bool btnSetPressed = false;
unsigned long lastDebounceUp = 0;
unsigned long lastDebounceSet = 0;
const unsigned long debounceDelay = 50;
bool setupComplete = false;

void setup() {
  Serial.begin(9600);
  Serial.println("=== Food Dispenser Starting ===");

 
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnSet, INPUT_PULLUP);

  
  delay(500);

  Wire.begin();
  Wire.setClock(100000);
  delay(100);

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 nije pronađen!"));
    for(;;);
  }
  Serial.println("OLED 128x32 initialized");
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(100);

  
  myServo.attach(9);
  Serial.println("Resetting servo...");
  myServo.write(0);
  delay(1000);
  myServo.detach();
  Serial.println("Servo ready");

  
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 0);
  display.println(F("Food"));
  display.setCursor(5, 16);
  display.println(F("Ready!"));
  display.display();

  
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);

  delay(2000);

  
  displaySettingScreen();

  Serial.println("=== Setup complete ===");
  Serial.println("Mode 0: Set HOURS");

  delay(3000);

 
  Serial.print("Button UP state: ");
  Serial.println(digitalRead(btnUp));
  Serial.print("Button SET state: ");
  Serial.println(digitalRead(btnSet));

  
  lastDebounceUp = millis();
  lastDebounceSet = millis();

  btnUpPressed = true;
  btnSetPressed = true;

  
  setupComplete = true;

  Serial.println("Ready for input!");
}

void loop() {
  
  if (!intervalSet) {
    handleSetting();
    return;
  }

  
  unsigned long currentSeconds = millis() / 1000;
  unsigned long elapsed = currentSeconds - lastDispenseTime;

  if (elapsed >= intervalSeconds) {
    dispenseFood();
    lastDispenseTime = millis() / 1000;
  }

  unsigned long remaining = intervalSeconds - elapsed;
  
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println(F("Next feeding:"));
  
  display.setTextSize(2);
  display.setCursor(5, 14);
  display.print(formatTime(remaining / 3600));
  display.print(F(":"));
  display.print(formatTime((remaining % 3600) / 60));
  display.print(F(":"));
  display.print(formatTime(remaining % 60));
  
  display.display();

  delay(100);
}

void handleSetting() {
  if (!setupComplete) {
    return;
  }

  unsigned long currentTime = millis();

  int btnUpState = digitalRead(btnUp);
  int btnSetState = digitalRead(btnSet);

  if (btnUpState == LOW) {
    if (!btnUpPressed && (currentTime - lastDebounceUp > debounceDelay)) {
      btnUpPressed = true;
      lastDebounceUp = currentTime;
      
      Serial.println("UP button pressed!");
      
      if (mode == 0) {
        setHour = (setHour + 1) % 24;
        Serial.print("Hour: ");
        Serial.println(setHour);
      }
      else if (mode == 1) {
        setMin = (setMin + 1) % 60;
        Serial.print("Minute: ");
        Serial.println(setMin);
      }
      else if (mode == 2) {
        setSec = (setSec + 1) % 60;
        Serial.print("Second: ");
        Serial.println(setSec);
      }
      
    
      digitalWrite(buzzerPin, HIGH);
      delay(50);
      digitalWrite(buzzerPin, LOW);
      
      
      displaySettingScreen();
    }
  } else {
    if (btnUpPressed) {
      Serial.println("UP button released");
    }
    btnUpPressed = false;
  }

  
  if (btnSetState == LOW) {
    if (!btnSetPressed && (currentTime - lastDebounceSet > debounceDelay)) {
      btnSetPressed = true;
      lastDebounceSet = currentTime;
      
      mode++;
      Serial.print("Mode changed to: ");
      Serial.println(mode);
      
      
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      
      
      if (mode > 2) {
        intervalSeconds = (unsigned long)setHour * 3600UL +
                         (unsigned long)setMin * 60UL +
                         (unsigned long)setSec;
        
        if (intervalSeconds == 0) {
          intervalSeconds = 60;
          Serial.println("Interval was 0, set to 60 seconds");
        }
        
        lastDispenseTime = millis() / 1000;
        intervalSet = true;
        
        Serial.println("=========================");
        Serial.print("TIMER STARTED! Interval: ");
        Serial.print(intervalSeconds);
        Serial.println(" seconds");
        Serial.println("=========================");
        
        
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 0);
        display.println(F("Timer"));
        display.setCursor(10, 16);
        display.println(F("Started!"));
        display.display();
        
      
        digitalWrite(buzzerPin, HIGH);
        delay(150);
        digitalWrite(buzzerPin, LOW);
        delay(100);
        digitalWrite(buzzerPin, HIGH);
        delay(150);
        digitalWrite(buzzerPin, LOW);
        
        delay(2000);
        return;
      }
      
      displaySettingScreen();
    }
  } else {
    btnSetPressed = false;
  }

  delay(50);
}

void displaySettingScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  if (mode == 0) {
    display.println(F(" Set HOURS"));
  }
  else if (mode == 1) {
    display.println(F(" Set MINUTES"));
  }
  else if (mode == 2) {
    display.println(F(" Set SECONDS"));
  }
  
 
  display.setTextSize(2);
  display.setCursor(10, 14);
  display.print(formatTime(setHour));
  display.print(F(":"));
  display.print(formatTime(setMin));
  display.print(F(":"));
  display.print(formatTime(setSec));
  
  display.display();
}


void dispenseFood() {
  Serial.println("=========================");
  Serial.println("=== DISPENSING FOOD ===");
  Serial.println("=========================");

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.println(F("FEEDING"));
  display.display();

  
  digitalWrite(buzzerPin, HIGH);
  delay(300);
  digitalWrite(buzzerPin, LOW);
  delay(200);

  myServo.attach(9);
  delay(100);

  Serial.println("Servo: 0 -> 60");
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 5);
  display.println(F("Dispensing..."));
  display.setTextSize(2);
  display.setCursor(40, 15);
  display.println(F(">>>"));
  display.display();
  
  myServo.write(60);
  delay(2000);

  Serial.println("Servo: 60 -> 0");
  myServo.write(0);
  delay(1000);

  myServo.detach();
  Serial.println("Servo detached");

  
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 8);
  display.println(F("Done!"));
  display.display();
  delay(2000);

  Serial.println("=========================");
  Serial.println("=== COMPLETE ===");
  Serial.println("=========================\n");
}


String formatTime(int val) {
  if (val < 10) return "0" + String(val);
  return String(val);
}