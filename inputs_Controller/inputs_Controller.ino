//Inputs code 
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

};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int btnOnePin;
const int btnTwoPin;

int gameState = 0;
int localScore = 0;

int btnOnePrevState = LOW;
int btnOneState = LOW;
int delay = 50;
unsigned long btnOneLastDebounceTime = 0;

unsigned long btnTwoLastDebounceTime = 0;
int mvDelay = 5;
int btnTwoPrevState = LOW;
int btnTwoState = LOW;


void setup() {
  // put your setup code here, to run once:
  pinMode(btnOnePin, INPUT);
  pinMode(btnTwoPin, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Press START");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Serial Communication
  Components data;
  data.gameStatus = gameState;
  data.score = localScore;

  Serial.write((byte*)&data, sizeof(data));



  //Reading and handling Inputs
  int btnOneRead = digitalRead(btnOnePin);
  int btnTwoRead = digitalRead(btnTwoPin);

  if(btnOneRead != btnOnePrevState){
    btnOneLastDebounceTime = millis();
  }
  if((millis() - btnOneLastDebounceTime) > delay){
    if(btnOneRead != btnOneState){
      btnOneState = btnOneRead;
      if(btnOneState == HIGH) {
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
    btnOnePrevState = btnOneRead;
  }


  if(gameState == 0){
    lcd.setCursor(0, 0);
    lcd.print("PRESS START");
  }
  if (gameState == 1){
    lcd.print("Score: ");
    lcd.setCursor(0, 8);
    lcd.print(localScore);
  }
  if(localScore == 100){
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.print("YOU WON :) !");
  }
  if(gameState == 2){
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.print("NEW GAME...");
  }
  if(gameState == 3){
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.print("PLAY AGAIN");
  }


}
