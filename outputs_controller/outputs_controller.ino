//Outputs Code 
struct Components {
  //This will have the score recorded depending on the states of 
  //the LEDs. 
  //Score will be ONLY WRITE in this file, the inputs file will 
  //read this score and display on the LCD 
  int score;
  //This gameStatus will tell us if the player has won or lost 
  //If gameStatus is 0 then game is ongoing, no win or loss 
  //If gameStatus is 1 the game is over 
  //The gameStaus will only be 1 if score is >= 100 
  //This gameStatus will only be 2 if the palyer lost 
  int gameStatus;

  //Help us track player movements 
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
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, // "Happy Birthday to You"
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, // "Happy Birthday to You"
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, // "Happy Birthday dear [Name]"
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4 // "Happy Birthday to You"
};


// Duration of each note (4 = quarter note, 2 = half note, etc.)
int winDurations[] = {
  4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 2
};


// "Twinkle twinle little star"
int melody2[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,  // Twinkle, twinkle, little star
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,  // How I wonder what you are
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,  // Up above the world so high
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,  // Like a diamond in the sky
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,  // Twinkle, twinkle, little star
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4   // How I wonder what you are}
};

int loseDurations[] = {
  4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star (G4 is half note/longer)
  4, 4, 4, 4, 4, 4, 2,  // How I wonder what you are
  4, 4, 4, 4, 4, 4, 2,  // Up above the world so high
  4, 4, 4, 4, 4, 4, 2,  // Like a diamond in the sky
  4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star
  4, 4, 4, 4, 4, 4, 2   // How I wonder what you are
};

int loseMelody[] = {
  NOTE_C4, NOTE_DS4, NOTE_G3,  // Descending minor pattern
  NOTE_F3, NOTE_GS3, NOTE_C4
};

int loseDurations3[] = {
  4, 8, 4,  // First phrase
  4, 8, 2   // Second phrase, ending slightly longer
};

const int greenPin = 7;
const int redPin = 6;
const int yellowPin = 5;
const int buzzerPin = 4;


int greenState = LOW;
int redState = LOW;
int yellowState = LOW;


//record changes in the game 
int gameSts = 0; //0:game not startedm, 1:running, 2:game reset, 3=game over(win or loss)
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
unsigned long ledInterval = 2000 * (randNum);
unsigned long scoreInterval = 500;
unsigned long serialInterval = 50;

int sucRead = 0;

void setup() {
  // put your setup code here, to run once:
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
  if(gameSts == 1){
    randNum = random(0, 10);
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
    
    // Yellow LED pattern
    if (randNum % 3 == 0) {
      digitalWrite(yellowPin, HIGH);
      yellowState = HIGH;
    } else {
      digitalWrite(yellowPin, LOW);
      yellowState = LOW;
    }
  }

}

void loop() {
  // randNum = rand() % 11;
  unsigned long currMillis = millis();
  // put your main code here, to run repeatedly:
  Components data;
  if (Serial.available() >= sizeof(data)) {
    Serial.readBytes((byte*)&data, sizeof(data));
    sucRead = 1;
    // Use your data
    gameSts = data.gameStatus;
    lastBtnState = data.btnState;
    score  = data.score;
  }else{
    sucRead = 0;
  }
  if(gameSts == 1 && ((redState == LOW && greenState == HIGH) || (redState == HIGH && greenState == LOW))){
    if(redState == HIGH && sucRead == 1 && localScore < data.score){
      gameSts = 3;
    }
  }
  if(data.score >= 100 && gameSts == 1){
    //paly winning tune
    playWinTune();
  }
  else if(gameSts == 3 and data.score != 100){
    playLoseTune();
  }
  else if(gameSts == 1){
    if(currMillis - lastLedChange >= ledInterval){
      updateLed();
      lastLedChange = currMillis;
    }
    if(lastBtnState == HIGH && greenState == HIGH){
      if(currMillis - lastScoreUpdate >= scoreInterval){
        if(score < 100){
          score++;
          lastScoreUpdate = currMillis;
        }
      }
    } else {
      lastScoreUpdate = currMillis;
    }

    if (lastBtnState == HIGH && redState == HIGH) {
      gameSts = 2;  // Game over - loss
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      playLoseTune();
    }

    if (score >= 100) {
      gameSts = 3;  // Win
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      playWinTune();
    }
  }

  if(currMillis - lastSerialSend >= serialInterval){
    Components updateData;
    updateData.gameStatus = gameSts;
    updateData.score = score;
    Serial.write((byte*)&updateData, sizeof(updateData));
    lastSerialSend = currMillis;
  }

  
}
