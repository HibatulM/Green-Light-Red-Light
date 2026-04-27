//Outputs Code 
struct Components {
  int score;
  int gameStatus;
  int btnState;
};

//buzzer Frequencies
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_DS4 311
#define NOTE_G3 196
#define NOTE_F3 175
#define NOTE_GS3 208
#define NOTE_AS4 466
#define NOTE_C5 523

// "Happy Birthday" melody sequence
int winMelody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

int winDurations[] = {
  4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 2
};

int loseMelody[] = {
  NOTE_C4, NOTE_DS4, NOTE_G3,
  NOTE_F3, NOTE_GS3, NOTE_C4
};

int loseDurations[] = {
  4, 8, 4,
  4, 8, 2
};

const int greenPin = 7;
const int redPin = 6;
const int yellowPin = 5;
const int buzzerPin = 4;

int greenState = LOW;
int redState = LOW;
int yellowState = LOW;

int gameSts = 0;
int randNum = 2;
int score = 0;
int localScore = 0;
int lastBtnState = LOW;

unsigned long lastLedChange = 0;
unsigned long lastScoreUpdate = 0;
unsigned long lastSerialSend = 0;
unsigned long lastDebounce = 0;

unsigned long prevMillis = 0;
unsigned long interval = 1000;
unsigned long ledInterval = 2000;  // Changed: LED changes every 2 seconds
unsigned long scoreInterval = 500;
unsigned long serialInterval = 50;

int sucRead = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  randomSeed(analogRead(0));

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, LOW);
}

void playLoseTune(){
  for (int thisNote = 0; thisNote < 6; thisNote++) {
    int noteDuration = 1000 / loseDurations[thisNote];
    tone(buzzerPin, loseMelody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.3;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void playWinTune(){
  for (int thisNote = 0; thisNote < 25; thisNote++) {
    int noteDuration = 1000 / winDurations[thisNote];
    tone(buzzerPin, winMelody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void updateLed(){
  // Generate new random pattern
  randNum = random(0, 10);
  
  // Set Red/Green LEDs (mutually exclusive)
  if (randNum % 2 == 0) {
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    greenState = HIGH;
    redState = LOW;
  } else {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    redState = HIGH;
    greenState = LOW;
  }
  
  // Set Yellow LED
  if (randNum % 3 == 0) {
    digitalWrite(yellowPin, HIGH);
    yellowState = HIGH;
  } else {
    digitalWrite(yellowPin, LOW);
    yellowState = LOW;
  }
}

void loop() {
  unsigned long currMillis = millis();
  
  Components data;
  if (Serial.available() >= sizeof(data)) {
    Serial.readBytes((byte*)&data, sizeof(data));
    sucRead = 1;
    int oldGameSts = gameSts;

    gameSts = data.gameStatus;
    lastBtnState = data.btnState;
    score = data.score;
    localScore = data.score;
    updateLed();

    if (oldGameSts == 0 && gameSts == 1) {
      score = 0;
      localScore = 0;
      lastLedChange = millis();
      updateLed();   // turn LED on immediately when START is clicked
    }
  } else {
    sucRead = 0;
  }
  
  // Check for cheating (moving on red)
  if(gameSts == 1 && redState == HIGH && sucRead == 1 && localScore < data.score && lastBtnState == HIGH){
    gameSts = 2;  // Game over - loss
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    playLoseTune();
  }
  
  if(data.score >= 100 && gameSts == 1){
    playWinTune();
    gameSts = 3;
  }
  else if(gameSts == 2 && data.score != 100){
    playLoseTune();
    gameSts = 3;
  }
  
  else if(gameSts == 1){
    // FIXED: Only update LED when enough time has passed (every 2 seconds)
    if(currMillis - lastLedChange >= ledInterval){
      updateLed();
      lastLedChange = currMillis;
    }
    
    // Score increment logic - only when button is pressed AND light is green
    if(lastBtnState == HIGH && greenState == HIGH){
      if(currMillis - lastScoreUpdate >= scoreInterval){
        if(score < 100){
          score++;
          lastScoreUpdate = currMillis;
        }
      }
    } else if(lastBtnState == HIGH && redState == HIGH) {
      // Moving on red - instant loss
      gameSts = 2;
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      playLoseTune();
    } else {
      // Reset score update timer when not actively scoring
      lastScoreUpdate = currMillis;
    }

    if (score >= 100) {
      gameSts = 3;  // Win
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      playWinTune();
    }
  }

  // Send data back to input Arduino
  if(currMillis - lastSerialSend >= serialInterval){
    Components updateData;
    updateData.gameStatus = gameSts;
    updateData.score = score;
    updateData.btnState = lastBtnState;
    Serial.write((byte*)&updateData, sizeof(updateData));
    lastSerialSend = currMillis;
  }
}