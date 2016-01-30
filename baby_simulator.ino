#include <LiquidCrystal.h>
#include <Servo.h>

// implement servo
Servo myServo; 

// implement lcd
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  

//number of each round in the game
int roundOfGame = 1;
// number identifier of action user needs to complete each round
int actionOfRound = 0;

// set up button for two actions: singing lullaby and giving milk
int buttonPin = 13;
int buttonState = 0;
int lastButtonState = 0;


// implement piezo
int speakerPin = 8;

// create "Twinkle, Twinkle, Little Star" lullaby
int length = 15; 
char notes[] = "ccggaagffeeddc "; 
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;


int needToWin = 3; // specify number of rounds needed to win
int strikes = 3; // specify number of rounds allowed to lose

// keep count of number of times an action is performed in each round
int count = 0; 

// used to limit amount of time the user has to act in each round
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 10000;

// implement the photoresister
const int photPin = A0;
int photSensorValue = 0;
int light = 0;
int blindsShut = 0;

// light to indicate a successful round
int ledPin = 7;

void setup() {
  myServo.attach(9);
  myServo.write(0);
  lcd.begin(16, 2);
  lcd.print("Baby Simulator");
  lcd.setCursor(0, 1);
  lcd.print("Game!");
  pinMode(speakerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  
  for (int positionCounter = 0; positionCounter < 42; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(150);
    }
  
  // keep track of light in room before game
  light = analogRead(photPin);
  Serial.println(photSensorValue);
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Instructions:");
  delay(2000);
  lcd.clear();
 
  lcd.print("Press twice ");
  lcd.setCursor(0, 1);
  lcd.print("for milk.");
  lcd.setCursor(0, 0);
  delay(2000);
  lcd.print("Press once ");
  lcd.setCursor(0, 1);
  lcd.print("for sleep.");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready, set, go!");
  delay(2000);
}                                                                                                                

void loop() {

  beginRound(roundOfGame);

  actionOfRound = random(3);
  digitalWrite(ledPin, LOW);
  
  count = 0;
  
  switch(actionOfRound) {
    
    // ===================uses the switch to sing a lullaby through the piezo=====================
    case 0:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The baby needs");
    lcd.setCursor(0, 1);
    lcd.print("to sleep."); 
    delay(500);
    
    currentMillis = millis();
    previousMillis = currentMillis - 5000;
    
    // user has 5000 ms to respond
    while (currentMillis - previousMillis <= interval) { 
      buttonState = digitalRead(buttonPin);
      if(buttonState != lastButtonState) {
        if (buttonState == HIGH) {
          // keep track of how many times the switch is pressed
          count++;     
         Serial.print(count);     
        }
      }
      lastButtonState = buttonState;  
      currentMillis = millis();
    }
    
    // if switch is pressed just once, perform action.
    if(count == 1) {
      digitalWrite(ledPin, HIGH);
      singLullaby();
      needToWin--;   // update
    } 
    else strikes--; // update
    
    break;
    
    //==============uses the switch to feed milk through the servo======================================= 
    case 1:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The baby needs");
    lcd.setCursor(0, 1);
    lcd.print("milk.");
    delay(500);
    
    currentMillis = millis();
    previousMillis = currentMillis - 5000;
    
    while (currentMillis - previousMillis <= interval) {
      buttonState = digitalRead(buttonPin);
      if(buttonState != lastButtonState) {
        if (buttonState == HIGH) {
        count++;      
        Serial.print(count);
        }
      }
      lastButtonState = buttonState;
      currentMillis = millis();
    }
    
    if(count == 2) {
      digitalWrite(ledPin, HIGH);
      giveMilk();
      needToWin--;   
    } 
    else strikes--;
    
    break;
    //=================use the photoresister to detect light/blinds shut=====================================
    case 2:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The baby needs");
    lcd.setCursor(0, 1);
    lcd.print("the blinds shut.");
    delay(1000);
    
    currentMillis = millis();
    
    previousMillis = currentMillis - 5000;
    while (currentMillis - previousMillis <= interval) {
      photSensorValue = analogRead(photPin);
      // check if value of photoresister is less than normal value of resister/1.5
      if (photSensorValue < (light / 1.5)) blindsShut = 1;
      currentMillis = millis();
    }
    
    if(blindsShut == 1) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      needToWin--;   
    } 
    else strikes--;
    break;
  }
  
  if(isLoser()) {
    endGame("lost");   
  }
  else if (isWinner()) {
    endGame("won");
  } 
  roundOfGame++;
}

// show this on the display at the beginning of each round
void beginRound(int roundOfGame) {
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Round ");
  lcd.print(roundOfGame);
  delay(2000);
  lcd.clear();
  lcd.print(needToWin);
  lcd.print(" more");
  lcd.setCursor(0, 1);
  lcd.print("to win!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(strikes);
  lcd.print(" strikes left.");
  delay(2000);
}

void singLullaby() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }

    // pause between notes
    delay(tempo / 2); 
  }
  delay(2000);
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void giveMilk() {
  for(int i = 0; i < 3; i++) {
    myServo.write(90);
    delay(500);
    myServo.write(0);
    delay(500);
  }
  delay(2000);
}

// show this on display after the game is won/lost
void endGame(String loseOrWin) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You have ");
    lcd.print(loseOrWin);
    lcd.print("!");
    delay(2500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please press ");
    lcd.setCursor(0, 1);
    lcd.print("restart.");
    delay(400000);
}

// determine if game is lost
boolean isLoser() {
  return(strikes == 0);
}

// determine if game is won
boolean isWinner() {
  return (needToWin == 0);
}
