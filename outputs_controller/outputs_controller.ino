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

}

//buzzer Frequencies
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_AS4 466
#define NOTE_C5 523

// "Happy Birthday" melody sequence
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, // "Happy Birthday to You"
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, // "Happy Birthday to You"
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, // "Happy Birthday dear [Name]"
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4 // "Happy Birthday to You"
};


// Duration of each note (4 = quarter note, 2 = half note, etc.)
int noteDurations[] = {
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

int noteDurations2[] = {
  4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star (G4 is half note/longer)
  4, 4, 4, 4, 4, 4, 2,  // How I wonder what you are
  4, 4, 4, 4, 4, 4, 2,  // Up above the world so high
  4, 4, 4, 4, 4, 4, 2,  // Like a diamond in the sky
  4, 4, 4, 4, 4, 4, 2,  // Twinkle, twinkle, little star
  4, 4, 4, 4, 4, 4, 2   // How I wonder what you are
};


const int greenPin;
const int redPin;
const int yellowPin;
const int buzzerPin;


int greenState = LOW;
int redState = LOW;
int yellowState = LOW;

int gameSts = 0;
int randNum = 0;

unsigned long prevMillis = 0;
unsigned long interval = 1000;

int localScore = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
}

void loop() {
  randNum = rand() % 11;
  unsigned long currMillis = millis();
  // put your main code here, to run repeatedly:
  Components data;
  if(curMillis - previousMillis >= interval){
      if (Serial.available() >= sizeof(data)) {
      Serial.readBytes((byte*)&data, sizeof(data));

      // Use your data
      gameSts = data.gameStatus;
      // int score  = data.score;
    }
    if(gameSts == 1 && ((redState == LOW && greenState == HIGH) || (redState == HIGH && greenState == LOW))){
      if(redState == HIGH && localScore < data.score){
        gameSts = 3;
      }
    }
    if(data.score >= 100 && gameSts == 1){
      //paly winning tune
      for (int thisNote = 0; thisNote < 25; thisNote++) {
        // Calculate the duration of each note in milliseconds
        // A quarter note is 1000/4 = 250ms
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(buzzerPin, melody[thisNote], noteDuration);

        // Add a small pause between notes to make them distinct
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);

        // Stop the tone before moving to the next note
        noTone(buzzerPin);
    } 
    }
    prevMillis = currMillis;
  }

  if(curMillis - prevMillis >= interval+(2000*randNum)){
    if(gameSts == 1){
      if(randNum % 2 == 0){
        digitalWrite(greenPin, HIGH);
        greenState = HIGH;
        digitalWrite(redPin, LOW);
        redState = LOW;
        localScore = data.score;
      }else{
        digitalWrite(redPin, HIGH);
        redState = HIGH;
        digitalWrite(greenPin, LOW);
        greenState = LOW;
        localScore = data.score;
      }
      if(randNum % 3 == 0){
        digitalWrite(yellowPin, HIGH);
        yellowState = HIGH;
      }else{
        digitalWrite(yellowPin, LOW);
        yellowState = LOW;
      }
      if(randNum % 4 == 0){
        //play the twinkle twinkle little star tune from buzzer
        for (int thisNote = 0; thisNote < 42; thisNote++) {  // 42 notes total
          // Calculate note duration: 1 second divided by note type (e.g., quarter note = 1000/4 = 250ms)
          int noteDuration = 1000 / noteDurations[thisNote];
          
          // Play the note on the buzzer
          tone(buzzerPin, melody[thisNote], noteDuration);
          
          // Add a small pause between notes to make them sound distinct
          // A 30% pause (multiply by 1.3) is a good starting point
          int pauseBetweenNotes = noteDuration * 1.3;
          delay(pauseBetweenNotes);
          
          // Stop the tone before moving to the next note
          noTone(buzzerPin);
        }
      }
    }
    prevMillis = currMillis;
  }

  
}
