# Ugra-eni_projekt
Automatski sustav za hranjenje ljubimaca

Opis sustava:

Glavni dio cijelog sustava je Arduino Uno R3 mikroupravljač s CH340 USB-serijskim pretvaračem koji u realnom vremenu upravlja procesom hranjenja kroz koordinaciju više komponenti. Korisnik putem dva fizička gumba (UP i SET) konfigurira vremenski interval između obroka u satima, minutama i sekundama. OLED zaslon s I2C komunikacijom u realnom vremenu prikazuje trenutne postavke tijekom konfiguracije te preostalo vrijeme do sljedećeg obroka tijekom rada. 
Kada dođe vrijeme za hranjenje, Arduino aktivira SG90 servo motor koji mehanički oslobađa odgovarajuću količinu hrane. Piezo buzzer pruža audio feedback tijekom interakcije s korisnikom i signalizira trenutak hranjenja. Svi senzori i aktuatori komuniciraju s mikroupravljačem preko digitalnih pinova, dok OLED zaslon koristi I2C protokol za efikasnu komunikaciju. 

Implementacija:
Sav programski kod je pisan u Arduino IDE okruženju koje koristi C/C++ programski jezik. 
Kod je strukturiran u nekoliko funkcionalnih cjelina koje upravljaju različitim aspektima sustava. 

Upotreba OLED zaslona

Definiranje parametara i inicijalizacija zaslona: 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C 
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
 
// Inicijalizacija I2C komunikacije 
Wire.begin(); 
Wire.setClock(100000); 
 
// Inicijalizacija zaslona 
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("SSD1306 nije pronađen!")); 
    for(;;); 
} 

Prikaz informacija na zaslonu: 

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

 

Upotreba servo motora 

Inicijalizacija i kontrola servo motora: 

Servo myServo; 
 
// Priključi servo motor na pin 9 
myServo.attach(9); 
 
// Rotacija servo motora za doziranje hrane 
myServo.write(60);  // Otvori mehanizam (60 stupnjeva) 
delay(2000);        // Čekaj 2 sekunde 
myServo.write(0);   // Zatvori mehanizam (0 stupnjeva) 
delay(1000); 
 
// Odvoji servo motor radi smanjenja potrošnje energije 
myServo.detach(); 

Debouncing gumba 

Debouncing je tehnika koja sprječava višestruko registriranje pritiska gumba uzrokovano mehaničkim vibracijama kontakata. 
Implementacija koristi vremenski filter od 50ms: 

bool btnUpPressed = false; 
unsigned long lastDebounceUp = 0; 
const unsigned long debounceDelay = 50; 
 
void handleSetting() { 
    unsigned long currentTime = millis(); 
    int btnUpState = digitalRead(btnUp); 
     
    if (btnUpState == LOW) { 
        if (!btnUpPressed && (currentTime - lastDebounceUp > debounceDelay)) { 
            btnUpPressed = true; 
            lastDebounceUp = currentTime; 
             
         // Povećaj vrijednost ovisno o trenutnom modu 
            if (mode == 0) { 
                setHour = (setHour + 1) % 24; 
            } 
            else if (mode == 1) { 
                setMin = (setMin + 1) % 60; 
            } 
            else if (mode == 2) { 
                setSec = (setSec + 1) % 60; 
            } 
             
            // Audio feedback 
            digitalWrite(buzzerPin, HIGH); 
            delay(50); 
            digitalWrite(buzzerPin, LOW); 
             
            displaySettingScreen(); 
        } 
    } else { 
        btnUpPressed = false; 
    } 
} 

 

Implementacija glavne logike sustava 

Funkcija doziranja hrane: 

void dispenseFood() { 
    // Prikaz na zaslonu 
    display.clearDisplay(); 
    display.setTextSize(2); 
    display.setCursor(10, 8); 
    display.println(F("FEEDING")); 
    display.display(); 
     
    // Audio signal - početak 
    digitalWrite(buzzerPin, HIGH); 
    delay(300); 
    digitalWrite(buzzerPin, LOW); 
    delay(200); 
     
    // Aktivacija servo motora 
    myServo.attach(9); 
    delay(100); 
     
    // Doziranje hrane 
    myServo.write(60); 
    delay(2000); 
    myServo.write(0); 
    delay(1000); 
     
    myServo.detach(); 
     
    // Audio signal - završetak 
    digitalWrite(buzzerPin, HIGH); 
    delay(200); 
    digitalWrite(buzzerPin, LOW); 
} 

 
Literatura 

[1] Arduino Documentation, 'Arduino Uno R3', dostupno na: https://docs.arduino.cc/hardware/uno-rev3/ 
[2] Adafruit Industries, 'Adafruit GFX Graphics Library', dostupno na: https://github.com/adafruit/Adafruit-GFX-Library 
[3] Adafruit Industries, 'Adafruit SSD1306 OLED Library', dostupno na: https://github.com/adafruit/Adafruit_SSD1306 
[4] Arduino Reference, 'Servo Library', dostupno na: https://www.arduino.cc/reference/en/libraries/servo/ 
[5] Arduino Reference, 'Wire Library (I2C)', dostupno na: https://www.arduino.cc/en/Reference/Wire 
[6] Solomon Systech, 'SSD1306 Datasheet - 128x64 Dot Matrix OLED/PLED Segment/Common Driver with Controller', 2008. 
[7] Arduino Tutorial, 'Debouncing', dostupno na: https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce 