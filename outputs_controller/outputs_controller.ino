//Outputs Code 
struct Components {
  //StartBtn will tell us if the game has started or not 
  //if startBtn is 0 then game has not started
  //if startBtn is 1 then game has started
  //if startBrn is 2 then game has been forcefully reset
  //StartBtn is to be ONLY READ in this file, the input file 
  // will be the one writing to varibale 
  int startBtn;
  // moveBtn will tell us if the player is moving 
  //or staying still 
  //If moveBtn is 0 the player is not moving 
  //If moveBtn is 1 then player is moving 
  //This variable is also to be ONLY READ and only the inputs file
  //will write to this variable
  int moveBtn;
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

const int greenPin;
const int redPin;
const int yellowPin;
const int buzzerPin;

Components output;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  output.gameStatus = 0;
  output.score = 0;
}

void loop() {
  // put your main code here, to run repeatedly:

}
