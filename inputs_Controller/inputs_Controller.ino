//----------------------------Header--------------------------------------------
//Name: Hibatul Muqeet, Yash Rathore
//Project: Traffic Light: A Fun Interactive Reaction Game 
//Abstract: This project presents a digital simulation of the classic kindergarten game,
// Red Light Green Light, implemented using multiple Arduino microcontrollers. 
// The system consists of a network of player-controlled push buttons and a centralized light controller,
//  all coordinated through a master controller that manages game logic and scoring. 
// Inter-device communication is handled via serial communication between the Arduino units,
//  enabling real-time responsiveness to player actions. Game progress and scores are 
// displayed to all participants through an integrated LCD display. 
// The result is an adaptive, fully responsive digital recreation of Red Light Green Light that faithfully 
// replicates the original game's mechanics while demonstrating robust, expert-level 
// embedded systems design and game management.


//Inputs code 
#include <LiquidCrystal.h>

struct Components {
  //This will have the score recorded depending on the states of 
  //the LEDs. 
  //Score will be ONLY READ in this file, the outputs file will 
  //change this score and we will display this value on the LCD 
  int score;
  //This gameStatus will tell us if the player has won or lost 
  //If gameStatus is 0 then game is ongoing, no win or loss 
  //If gameStatus is 1 the game is over 
  //The gameStaus will only be 1 if score is >= 100 
  //This gameStatus will only be 2 if the palyer lost 
  //The gameStatus is READ only 
  int gameStatus;
  int btnState;
};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int btnOnePin;
const int btnTwoPin;

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
unsigned long lastSerialRecieve = 0;
unsigned long lastSerialSend = 0;
unsigned long serialInterval  = 50;
unsigned long nextGameInterval = 3000;
unsigned long lastGameOver = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(btnOnePin, INPUT_PULLUP);
  pinMode(btnTwoPin, INPUT_PULLUP);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("  Press START  ");
}

void updateDisplay(){
  lcd.print("                ");

  if(gameState == 0){
    lcd.setCursor(0, 0);
    lcd.print("  Press START  ");
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
  }else if (gameState == 2){
    lcd.setCursor(0, 0);
    lcd.print("   GAME OVER   ");
    lcd.setCursor(0, 1);
    lcd.print("  PLAY AGAIN!  ");
    
    if(millis() - lastGameOver >= nextGameInterval){
      gameState = 0;  // play again state
      localScore = 0;
      lastGameOver = millis();
    }
    
  } else if (gameState == 3){
    if(localScore < 100){
      lcd.setCursor(0, 0);
      lcd.print("   GAME OVER   ");
      lcd.setCursor(0, 1);
      lcd.print("  YOU LOST!    ");
      if(millis() - lastGameOver >= nextGameInterval){
        gameState = 0;  // play again state
        localScore = 0;
        lastGameOver = millis();
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("    YOU WIN!   ");
      lcd.setCursor(0, 1);
      lcd.print("  CONGRATS!    ");
      if(millis() - lastGameOver >= nextGameInterval){
        gameState = 0;  // play again state
        localScore = 0;
        lastGameOver = millis();
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currMillis = millis();
  
  //Reading and handling Inputs
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
        } 
        else if(gameState == 1){
          gameState = 2;
        }
        else if(gameState == 2){
          gameState = 0;
        }
      }
    }
    
  }
  btnOneState = btnOneRead;

  if(btnTwoRead != lastBtnTwoState){
    btnTwoLastDebounceTime = millis();
  }
  if((currMillis - btnTwoLastDebounceTime) > debounceDelay){
    if(btnTwoRead != btnTwoState){
      btnTwoState = btnTwoRead;
    }
    
  }
  btnTwoState = btnTwoRead;

  if(currMillis - prevMillis >= 1000){

    if(currMillis - lastSerialSend >= serialInterval){
        //Serial Communication
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
    
    if(Serial.available() >= sizeof(Components)){
      Components updateData;
      Serial.readBytes((byte*)&updateData, sizeof(updateData));

      gameState = updateData.gameStatus;
      prevScore = localScore;
      localScore = updateData.score;
    }
    
    if(localScore != prevScore){
      updateDisplay();
      prevScore = localScore;
    }
  }
  prevMillis = currMillis;
}
