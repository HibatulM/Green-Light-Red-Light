
struct Components {
  int score;
  int gameStatus;
  int btnState;
};

// Buzzer frequencies
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

// "Happy Birthday" melody (win)
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

// Lose melody
int loseMelody[] = {
  NOTE_C4, NOTE_DS4, NOTE_G3,
  NOTE_F3, NOTE_GS3, NOTE_C4
};

int loseDurations[] = {
  4, 8, 4,
  4, 8, 2
};

// Pin definitions
const int greenPin = 7;
const int redPin = 6;
const int yellowPin = 5;
const int buzzerPin = 4;

// LED states
int greenState = LOW;
int redState = LOW;
int yellowState = LOW;

// Game variables
int gameSts = 0;      // 0:idle, 1:active, 2:loss, 3:win
int score = 0;
int localScore = 0;
int lastBtnState = LOW;
int randNum = 2;

// Timing variables (non‑blocking)
unsigned long lastLedChange = 0;
unsigned long lastScoreUpdate = 0;
unsigned long lastSerialSend = 0;
unsigned long ledInterval = 2000;     // LED pattern changes every 2s
unsigned long scoreInterval = 500;    // Score increment interval (500ms)
unsigned long serialInterval = 50;    // Serial send interval

// Serial success flag
int sucRead = 0;

// ==================== NON‑BLOCKING MELODY PLAYER ====================
int* currentMelodyNotes;
int* currentMelodyDurations;
int melodyLength;
int melodyIndex;
unsigned long melodyNoteStartedAt;
int melodyWaitDuration;
bool melodyPlaying = false;

void startWinMelody() {
  // Turn off all LEDs
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  
  melodyPlaying = true;
  currentMelodyNotes = winMelody;
  currentMelodyDurations = winDurations;
  melodyLength = 25;
  melodyIndex = 0;
  melodyNoteStartedAt = 0;
  melodyWaitDuration = 0;
}

void startLoseMelody() {
  // Turn off all LEDs
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  
  melodyPlaying = true;
  currentMelodyNotes = loseMelody;
  currentMelodyDurations = loseDurations;
  melodyLength = 6;
  melodyIndex = 0;
  melodyNoteStartedAt = 0;
  melodyWaitDuration = 0;
}

void updateMelody() {
  if (!melodyPlaying) return;
  
  // Finished all notes?
  if (melodyIndex >= melodyLength) {
    melodyPlaying = false;
    noTone(buzzerPin);
    return;
  }
  
  // Start a new note (or first note)
  if (melodyNoteStartedAt == 0) {
    int note = currentMelodyNotes[melodyIndex];
    int durationDiv = currentMelodyDurations[melodyIndex];
    int notePlayMs = 1000 / durationDiv;
    int totalWaitMs = notePlayMs * 1.3;   // note + 30% gap
    
    tone(buzzerPin, note, notePlayMs);
    melodyNoteStartedAt = millis();
    melodyWaitDuration = totalWaitMs;
  } 
  else {
    // Wait for current note (including gap) to finish
    if (millis() - melodyNoteStartedAt >= melodyWaitDuration) {
      melodyIndex++;
      melodyNoteStartedAt = 0;   // will trigger next note in next call
    }
  }
}
// ====================================================================

void updateLed() {
  // Generate new random pattern
  randNum = random(0, 10);
  
  // Red/Green (mutually exclusive)
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
  
  // Yellow LED
  if (randNum % 3 == 0) {
    digitalWrite(yellowPin, HIGH);
    yellowState = HIGH;
  } else {
    digitalWrite(yellowPin, LOW);
    yellowState = LOW;
  }
}

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

void loop() {
  unsigned long currMillis = millis();
  
  // 1) Non‑blocking melody playback (must run often)
  updateMelody();
  
  // 2) Receive data from the other Arduino
  Components data;
  if (Serial.available() >= sizeof(data)) {
    Serial.readBytes((byte*)&data, sizeof(data));
    sucRead = 1;
    int oldGameSts = gameSts;
    
    gameSts = data.gameStatus;
    lastBtnState = data.btnState;
    score = data.score;
    localScore = data.score;
    
    // Immediate LED update when game starts
    if (oldGameSts == 0 && gameSts == 1) {
      score = 0;
      localScore = 0;
      lastLedChange = millis();
      updateLed();
    }
  } else {
    sucRead = 0;
  }
  
  // 3) Game logic – only when game is active AND no melody is playing
  if (gameSts == 1 && !melodyPlaying) {
    
    // ----- Cheating detection (original logic) -----
    if (redState == HIGH && sucRead == 1 && localScore < data.score && lastBtnState == HIGH) {
      gameSts = 2;                // Loss
      startLoseMelody();
    }
    
    // ----- Move on red = instant loss -----
    else if (lastBtnState == HIGH && redState == HIGH) {
      gameSts = 2;
      startLoseMelody();
    }
    
    // ----- Score increment on green + button press -----
    else if (lastBtnState == HIGH && greenState == HIGH) {
      if (currMillis - lastScoreUpdate >= scoreInterval) {
        if (score < 100) {
          score++;
          lastScoreUpdate = currMillis;
        }
      }
    } 
    else {
      // Reset score timer when not scoring
      lastScoreUpdate = currMillis;
    }
    
    // ----- Win condition -----
    if (score >= 100) {
      gameSts = 3;               // Win
      startWinMelody();
    }
    
    // ----- LED pattern update (every 2 seconds) -----
    if (currMillis - lastLedChange >= ledInterval) {
      updateLed();
      lastLedChange = currMillis;
    }
  }
  
  // 4) Send current state back to the other Arduino
  if (currMillis - lastSerialSend >= serialInterval) {
    Components updateData;
    updateData.gameStatus = gameSts;
    updateData.score = score;
    updateData.btnState = lastBtnState;
    Serial.write((byte*)&updateData, sizeof(updateData));
    lastSerialSend = currMillis;
  }
}