//----------------------------Header--------------------------------------------
//Name: Hibatul Muqeet, Yash Rathore
//Project: Traffic Light: A Fun Interactive Reaction Game 

#include <LiquidCrystal.h>

struct Components {
  int score;
  int gameStatus;
  int btnState;
};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int btnOnePin = 7;
const int btnTwoPin = 9;

int gameState = 0;
int localScore = 0;
int prevScore = 0;

int btnOnePrevState = LOW;
int btnOneState = LOW;
int lastBtnOneState = LOW;
unsigned long btnOneLastDebounceTime = 0;

unsigned long btnTwoLastDebounceTime = 0;
int lastBtnTwoState = LOW;
int btnTwoState = LOW;

int debounceDelay = 50;
int mvDelay = 5;

unsigned long prevMillis = 0;
unsigned long lastSerialReceive = 0;
unsigned long lastSerialSend = 0;
unsigned long serialInterval = 50;  // Send every 50ms (20 times per second)
unsigned long nextGameInterval = 3000;
unsigned long lastGameOver = 0;

void setup() {
  pinMode(btnOnePin, INPUT_PULLUP);
  pinMode(btnTwoPin, INPUT_PULLUP);
  Serial.begin(9600);

  lcd.begin(16, 2);
  updateDisplay();
}

void updateDisplay(){
  lcd.clear();
  
  if(gameState == 0){
    lcd.setCursor(0, 0);
    lcd.print("  Press START  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  } else if (gameState == 1){
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.print(localScore);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    if (localScore < 100) {
      lcd.print("Keep going!    ");
    } else {
      lcd.print("Almost there!  ");
    }
  } else if (gameState == 2){
    lcd.setCursor(0, 0);
    lcd.print("   GAME OVER   ");
    lcd.setCursor(0, 1);
    lcd.print("  PLAY AGAIN!  ");
    
    if(millis() - lastGameOver >= nextGameInterval){
      gameState = 0;
      localScore = 0;
      lastGameOver = millis();
      updateDisplay();
    }
    
  } else if (gameState == 3){
    if(localScore < 100){
      lcd.setCursor(0, 0);
      lcd.print("   GAME OVER   ");
      lcd.setCursor(0, 1);
      lcd.print("  YOU LOST!    ");
      if(millis() - lastGameOver >= nextGameInterval){
        gameState = 0;
        localScore = 0;
        lastGameOver = millis();
        updateDisplay();
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("    YOU WIN!   ");
      lcd.setCursor(0, 1);
      lcd.print("  CONGRATS!    ");
      if(millis() - lastGameOver >= nextGameInterval){
        gameState = 0;
        localScore = 0;
        lastGameOver = millis();
        updateDisplay();
      }
    }
  }
}

void loop() {
  unsigned long currMillis = millis();
  
  // Reading and handling Inputs
  int btnOneRead = digitalRead(btnOnePin);
  int btnTwoRead = digitalRead(btnTwoPin);

  if(btnOneRead != btnOnePrevState){
    btnOneLastDebounceTime = millis();
  }
  if((currMillis - btnOneLastDebounceTime) > debounceDelay){
    if(btnOneRead != btnOneState){
      btnOneState = btnOneRead;
      if(btnOneState == LOW) {
        if(gameState == 0){
          gameState = 1;
          updateDisplay();
          
        } 
        else if(gameState == 1){
          gameState = 2;
          updateDisplay();
        }
        else if(gameState == 2){
          gameState = 0;
          updateDisplay();
        }
      }
    }
    
  }
  btnOnePrevState = btnOneRead;

  if(btnTwoRead != lastBtnTwoState){
    btnTwoLastDebounceTime = millis();
  }
  if((currMillis - btnTwoLastDebounceTime) > debounceDelay){
    if(btnTwoRead != btnTwoState){
      btnTwoState = btnTwoRead;
    }
    
  }
  lastBtnTwoState = btnTwoRead;

  // REMOVED the 1-second delay - now sends continuously
  // Serial Communication - Send every 50ms
  if(currMillis - lastSerialSend >= serialInterval){
    Components data;
    data.gameStatus = gameState;
    data.score = localScore;
    if(btnTwoState == LOW){
      data.btnState = 1;
    } else {
      data.btnState = 0;
    }

    Serial.write((byte*)&data, sizeof(data));
    lastSerialSend = currMillis;
  }
  
  // Receive data from output Arduino
  if(Serial.available() >= sizeof(Components)){
    Components updateData;
    Serial.readBytes((byte*)&updateData, sizeof(updateData));

    gameState = updateData.gameStatus;
    prevScore = localScore;
    localScore = updateData.score;
    
    if(localScore != prevScore){
      updateDisplay();
      Serial.println(localScore);
      prevScore = localScore;
    }
  }
}