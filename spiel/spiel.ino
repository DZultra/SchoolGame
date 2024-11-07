/*
  Libarys
*/

#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo Servo1;

/*
  Variables
*/

const int ldrPin = A0;
const int randomSeedPin = 2; // A1 macht komische Dinge
const int ledPins[7] = {2, 3, 4, 5, 6, 7, 8}; // 
const int servoPin = 13;
const int buttonPin = 12;

// Folgende Listen werden präsentiert von "Schlechte Verkablung Gmbh" (alle LEDs sind verschoben um 1)
const int led8[] = { 13, 14, 15, 16, 17, 18 };
const int led7[] = { 35, 36, 37, 38, 39, 40};
const int led6[] = { 57, 58, 59, 60, 61, 62 };
const int led5[] = { 79, 80, 81, 82, 83, 84, 85 };
const int led4[] = { 104, 105, 106, 107, 108, 109 };
const int led3[] = { 128, 129, 130, 131, 132, 133 };
const int led2[] = { 154, 155, 156, 157, 158, 159 };

const int laenge2 = 6;
const int laenge3 = 6;
const int laenge4 = 6;
const int laenge5 = 7;
const int laenge6 = 6;
const int laenge7 = 6;
const int laenge8 = 6;

int position;
int position1;
int ldrValue;
int ldrthreshold = 1000;
int score = 0;
int leuchtendeLED;
int activeLED;
int currentLED;
int winscore = 10;
int new_position;

bool richtung; // true -> Im Uhrzeigersinn | false -> gegen Uhrzeigersinn
bool LEDactive = false;
bool gameActive = false;
bool buttonPressed = false;
bool lastpressgood;

/*
  Setup
*/


void setup() { // Setup
  Serial.begin(9600);
  randomSeed(analogRead(randomSeedPin));
  gameActive = false;
  LEDactive = false;

  Button_Setup();
  LED_Setup();
  Servo_Setup();
  LCD_Setup();

  debug("Setup Complete");
  delay(1000);
}

/*
  Game Logic
*/

void loop() { // Loop
  checkCoinInput();
  if (gameActive == false) {  // Wenn Game nicht active ist
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Enter Coin");
    lcd.setCursor(0, 1);
    lcd.print("to Start!");
    delay(10);
  } else {
    Spiel();
  }
}

void Spiel() { // Spiel Logik
  richtung = true;
  for (int pos = 0; pos <= 180; pos += 1) { 
    if (LEDactive == false) {  // Wenn keine LED an ist, dann
      randomLEDon();           // mache zufällige LED an
    } 
    if (ButtonPressedAtRightTime() == false) {
      game_over();
      return;
    }
    if(score == winscore) { game_win(); return; }
    Servo1.write(pos);
    delay(70);
  }
  richtung = false;
  for (int pos1 = 180; pos1 >= -10; pos1 -= 1) {
    if (LEDactive == false) {  // Wenn keine LED an ist, dann
      randomLEDon();           // mache zufällige LED an
    } 
    Servo1.write(pos1);
    delay(70);
    if (ButtonPressedAtRightTime() == false) {
      game_over();
      return;
    }
    if(score == winscore) { game_win(); return; }
  }
}

bool ButtonPressedAtRightTime() { // Wurde der Knopf richtig gedrückt?
  if (checkButton() == true) { // Knopf wurde gedrückt
    //lcd.clear(); lcd.setCursor(0, 0); lcd.print("Button Pressed"); delay(1000);
    return checkWinkelIfButtonPressed(leuchtendeLED);
  }
  else { // Knopf nicht gedrückt
    return checkWinkelIfNotButtonPressed(leuchtendeLED);
  }
}

/*
  Button Pressed
*/

bool checkWinkelIfButtonPressed(int leuchtendeLED) { // Spiel-Logik wenn Knopf gedrückt
  if (leuchtendeLED == 2) {
    return checkWinkelInListeIfButtonPressed(2, led2, laenge2);
  } 
  else if (leuchtendeLED == 3) {
    return checkWinkelInListeIfButtonPressed(3, led3, laenge3);
  } 
  else if (leuchtendeLED == 4) {
    return checkWinkelInListeIfButtonPressed(4, led4, laenge4);
  }
  else if (leuchtendeLED == 5) {
    return checkWinkelInListeIfButtonPressed(5, led5, laenge5);
  }
  else if (leuchtendeLED == 6) {
    return checkWinkelInListeIfButtonPressed(6, led6, laenge6);
  }
  else if (leuchtendeLED == 7) {
    return checkWinkelInListeIfButtonPressed(7, led7, laenge7);
  }
  else if (leuchtendeLED == 8) {
    return checkWinkelInListeIfButtonPressed(8, led8, laenge8);
  }
  return false;
}

bool checkWinkelInListeIfButtonPressed(int leuchtendeLED, int* ledListe, int laenge) { // Spiel-Logik wenn Knopf gedrückt
  //Knopf gedrückt
  position = currentWinkel();
  //lcd.clear(); lcd.setCursor(0, 0); lcd.print("Position"); delay(1000); // Debug
  //lcd.clear(); lcd.setCursor(0, 0); lcd.print(position); delay(1000); // Debug
  for (int i = 0; i < laenge; i++) { if (ledListe[i] == position) { // Ist die aktuelle Position über der leuchtenden LED?
      // Ja
      //lcd.clear(); lcd.setCursor(0, 0); lcd.print("Debug1 PosGood"); delay(1000); // Debug
      score++;
      lcd.clear(); lcd.setCursor(0, 0); lcd.print("Score:");
      lcd.setCursor(0, 1); lcd.print(score);
      digitalWrite(leuchtendeLED, LOW);
      LEDactive = false;
      if (richtung == true) {
        new_position = position + 1;
      } else {
        new_position = position - 1;
      }
      for (int i = 0; i < laenge; i++) { if (ledListe[i] == new_position) { // Ist die neue Position über der leuchtenden LED?
        lastpressgood = true;
        break;
      }}
      return true; // Knopf wurde gedrückt wenn Position bei leuchtender LED
    }
  }
  // Nein
  //lcd.clear(); lcd.setCursor(0, 0); lcd.print("Debug1 PosBad"); delay(2000); // Debug
  return false;
}

/*
  Button not Pressed
*/

bool checkWinkelIfNotButtonPressed(int leuchtendeLED) { // Spiel-Logik wenn Knopf nicht gedrückt
  if (leuchtendeLED == 2) {
    return checkWinkelInListeIfNotButtonPressed(2, led2, laenge2);
  } 
  else if (leuchtendeLED == 3) {
    return checkWinkelInListeIfNotButtonPressed(3, led3, laenge3);
  } 
  else if (leuchtendeLED == 4) {
    return checkWinkelInListeIfNotButtonPressed(4, led4, laenge4);
  }
  else if (leuchtendeLED == 5) {
    return checkWinkelInListeIfNotButtonPressed(5, led5, laenge5);
  }
  else if (leuchtendeLED == 6) {
    return checkWinkelInListeIfNotButtonPressed(6, led6, laenge6);
  }
  else if (leuchtendeLED == 7) {
    return checkWinkelInListeIfNotButtonPressed(7, led7, laenge7);
  }
  else if (leuchtendeLED == 8) {
    return checkWinkelInListeIfNotButtonPressed(8, led8, laenge8);
  }
}

bool checkWinkelInListeIfNotButtonPressed(int leuchtendeLED, int* ledListe, int laenge) { // Spiel-Logik wenn Knopf nicht gedrückt
  // Knopf nicht gedrückt
  position1 = currentWinkel();
  if (richtung == true) {
    // Im Uhrzeigersinn
    for (int i = 0; i < laenge; i++) { if (ledListe[i] == position1) { // Ist die aktuelle Position über der leuchtenden LED?
      // Ja
      new_position = position1 + 1;
      for (int i = 0; i < laenge; i++) { if (ledListe[i] == new_position) { // Ist die neue Position über der leuchtenden LED?
        // Ja
        return true; // Knopf kann noch gedrückt werden
      } else {
        lastpressgood == false;
      }}
      // Nein
      return false; // Knopf hätte gedrückt werden müssen
    }}
    // Nein
    return true; // Knopf wurde nicht gedrückt und sollte nicht gedrückt werden
  } else {
    // Gegen den Uhrzeigersinn
    for (int i = 0; i < laenge; i++) { if (ledListe[i] == position1) { // Ist die aktuelle Position über der leuchtenden LED?
      // Ja
      new_position = position1 - 1;
      for (int i = 0; i < laenge; i++) { if (ledListe[i] == new_position) { // Ist die neue Position über der leuchtenden LED?
        // Ja
        return true; // Knopf kann noch gedrückt werden
      } else {
        lastpressgood == false;
      }}
      // Nein
      return false; // Knopf hätte gedrückt werden müssen
    }}
    // Nein
    return true;
  }
}

/*
  Functions
*/

void checkCoinInput() { // Coin Slot Überprüfung
  if (gameActive == true) { return; }
  ldrValue = analogRead(ldrPin);
  debug("LDR-Value: " + ldrValue);
  if (ldrValue < ldrthreshold) {
    gameActive = true;
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Get Ready");
    delay(1000);
    lcd.clear();
    lcd.print("3");
    delay(1000);
    lcd.clear();
    lcd.print("2");
    delay(1000);
    lcd.clear();
    lcd.print("1");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Score: 0");
  } else {
      if (gameActive) {
         return;
      }
      gameActive = false;
  }
}

void game_over() { // Wenn Spiel verloren
  allLEDoff();
  score = 0;
  gameActive = false;
  LEDactive = false;
  lcd.setCursor(0,0); lcd.clear(); lcd.print("Game Over"); // Loser
  delay(4000);
  lcd.clear(); lcd.print("Score:");
  lcd.setCursor(0,1);
  lcd.print(score);
  Servo1.write(0);
  //while (true) {}; // Halt STOP!
}

void game_win() { // Wenn Spiel gewonnen
  allLEDoff();
  lastpressgood = false;
  gameActive = false;
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("You Won!"); // Glückwunsch
  delay(5000);
  score = 0;
}

bool checkButton() { // Button Abfrage
  return digitalRead(buttonPin) == LOW;
}

int currentWinkel() { // Aktueller Winkel des Servos
  int servowinkel = Servo1.read();
  return servowinkel;
}

void randomLEDon() { // Zufällige LED an
  currentLED == leuchtendeLED;
  leuchtendeLED = random(2, 9);  // Zufallszahl von 2 bis 8
  if (currentLED == leuchtendeLED) { // Verhindern, dass die selber LED zweimal hintereinander angeht
    if (leuchtendeLED == 2) {
      leuchtendeLED--; 
    } else {
      leuchtendeLED++;
    }
  }
  digitalWrite(leuchtendeLED, HIGH);
  LEDactive = true;
}

void allLEDoff() { // Alle LEDs ausmachen
  // Keine Lust auf For-Schleife
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
}

void debug(char message) { // Debug Messages methodieren
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message);
}

/*
  Setup
*/
void LCD_Setup() {
  lcd.init();
  lcd.backlight();
}

void LED_Setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void Servo_Setup() {
  Servo1.attach(servoPin);
  Servo1.write(0);
}

void Serial_Setup() {
  Serial.begin(9600);
}

void Button_Setup() {
  pinMode(buttonPin, INPUT_PULLUP);
}