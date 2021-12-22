#include<LiquidCrystal.h>
#include<EEPROM.h>
#include "LedControl.h" 
#include "notes.h"

const int noOfRows = 8;    // rows on the matrix
const int totalSymbols = 6; // i will use 6 symbols for the game

byte up[noOfRows] = {
  B00011000,
  B00011000,
  B00011000,
  B00011000,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

byte down[noOfRows] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B00011000,
  B00011000,
  B00011000,
  B00011000
};

byte right[noOfRows] = {
  B00010000,
  B00110000,
  B01110000,
  B11111111,
  B11111111,
  B01110000,
  B00110000,
  B00010000
};

byte left[noOfRows] = {
  B00001000,
  B00001100,
  B00001110,
  B11111111,
  B11111111,
  B00001110,
  B00001100,
  B00001000
};

byte buttonRight[8] = { // a circle with a R inside
  B00111100,
  B01000010,
  B10101001,
  B10011001,
  B10101001,
  B10011001,
  B01000010,
  B00111100
};

byte buttonLeft[8] = { // a circle with a L inside
  B00111100,
  B01000010,
  B10111001,
  B10001001,
  B10001001,
  B10001001,
  B01000010,
  B00111100
};

String songsList[] = { // what i will show in the settings menu
  "OFF  ",
  "Song1",
  "Song2"
};

byte symbols[totalSymbols][noOfRows]; // here i store all the symbols to use them faster

const int RS = 13; // values for the LCD
const int enable = 8;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;


const int dinPin = 12; // values for the matrix
const int clockPin = 11;
const int loadPin = 10;
const int rows = 8;
const int cols = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
 
const int difficultyAddress = 0; // addresses i will use to write/read in EEPROM
const int lcdContrastAddress = 1;
const int lcdBrightnessAddress = 2;
const int matrixBrightnessAddress = 3;
const int songAddress = 4;


const int contrastPin = 6;
const int buttonLeftPin = A2;
const int buttonRightPin = A3;
const int lcdBrightnessPin = 9;

const int xPin = A0;
const int yPin = A1;

int xVal = 0;
int yVal = 0;

int c = 0;

bool joyMoved = 0;

int currentMenuOption = 0; // depending on this value i will change what the lcd shown when i am in the main menu
int lastMenuOption = -1;

int currentSettingsOption = 0; // depending on this value i will change what the lcd shown when i am in the settings menu
int lastSettingsOption = -1;

const int minThreshold = 200; // thresholds for the joystick
const int maxThreshold = 600;

const int blinkInterval = 500; // the space to the left of the option currently selected will blink
unsigned long lastBlink = 0;
bool isBlinking = 0; // depending on this value i switch the blinking on and off
unsigned long lastScrollTime = 0; // scroll values for the ABOUT section
const int scrollTime = 500;

int contrastValues[] = {85, 110, 135, 150};
int lcdBrightnessValues[] = {5, 55, 105, 155, 205, 255};

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

int state = 0; // inital state is 0, the welcome to the game message

bool buttonLeftReading = HIGH; // variables for the buttons logic
bool buttonRightReading = HIGH;
bool lastReading = HIGH;
bool buttonIsPressed = false;
bool buttonWasPressed = false;

int difficultyLevel; // i read this value from EEPROM
int lastDifficultyLevel = 0;

int lcdContrastLevel;// i read this value from EEPROM
int lastLcdContrastLevel = 0;

int lcdBrightnessLevel;// i read this value from EEPROM
int lastLcdBrightnessLevel = 0;

int matrixBrightnessLevel;// i read this value from EEPROM
int lastMatrixBrightnessLevel = 0;

int currentSong;// i read this value from EEPROM
int lastSong = -1;

int roundSeconds[] = {20, 15, 10}; // seconds for each round
int waitTime[] = {900, 600, 600}; // time the symbol will be on depending on difficulty
int randNumber = 0; // here i will get a random number for the game
int reverseRandom = 0; // another random value that i will use for the reverse check
bool reverse = false; // if this is true the symbol to be displayed will have a dot in each corner and the value will become the opposite
int currentScore = 0; // score counter
int lastScore = 0; 
int roundNr = 0; // round counter
bool roundStarted = false; // if this is false i prepare the round, else i play the game
bool learnMessage = false;
bool wasShown = false; // variable i use when showing the bomb combination
int noOfSymbols = 0; // here i keep the number of symbols i need each round
int symbolsToWrite[30]; // here i store the generated combination
int currentSymbolIndex = 0; // index for playing the game
int showSymbolIndex = 0; // index for showing the combination
int secondsLeft = 0; // how many seconds are left in the round
const int second = 1000; // value of a second in ms
const int roundCompleteMessageTime = 2000; // the message after a round completes stays on the screen for 2 seconds
unsigned long lastTimeUpdate = 0; // timer for the time left
unsigned long showSymbolTimer = 0; // timer for showing the combination
unsigned long roundCompleteTimer = 0; // timer for round complete message
int symbolTyped = 0; // the typed symbol's value
bool rightPressed = false; // check if right was pressed
bool leftPressed = false; // check is left was pressed

char levelRoundString[20]; // messages during the round
char scoreTimeString[20]; 
char scoreMessage[20];

int gameOverState; // i use 3 states for the game over 
unsigned long gameOverTimer; // timer for the blinking of the game over message
const int GameOverBlinkInterval = 1000; // time message blinks ever second
bool gameOverMessageVisible; // true if the gave over is visible now

int highscore1; // highscore values
int highscore2;
int highscore3;
const int highscore1Address = 5; // addresses of the highscores. the names fo the highscores will be 
const int highscore2Address = 15;// for HS1: 6-14, for H2: 16-24 and for H3: 26-34
const int highscore3Address = 25;

bool onOk = false; // true if the cursor is on the ok in the enter name state

String currentName = "         "; // 9 characters, empty at first
String lastName = ""; // i use this to refresh the lcd

int letterIndex = 0; // the number of the letter i am on 
int lastLetterIndex = -1; // i use this to refresh the lcd


char letter = ' '; // i use this to change the letter in the name
char lastLetter = '0'; // i use this to refresh the lcd

String highscore1Name = "";
String highscore2Name = "";
String highscore3Name = "";

String hsName = ""; // name of the highscore owner
String hsMessage = ""; // i use this to print the line with the highscore

int currentHS = 0; // i use these when showing the highscores
int lastHS = -1;

void setup() {
  // read values
  difficultyLevel = EEPROM.read(difficultyAddress); 
  lcdContrastLevel = EEPROM.read(lcdContrastAddress);
  lcdBrightnessLevel = EEPROM.read(lcdBrightnessAddress);
  matrixBrightnessLevel = EEPROM.read(matrixBrightnessAddress);
  currentSong = EEPROM.read(songAddress);
  highscore1 = EEPROM.read(highscore1Address);
  highscore2 = EEPROM.read(highscore2Address);
  highscore3 = EEPROM.read(highscore3Address);

  //declare pins
  pinMode(buttonLeftPin, INPUT_PULLUP);
  pinMode(buttonRightPin, INPUT_PULLUP);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  //prepare the lcd
  lcd.begin(50, 2);
  pinMode(contrastPin, OUTPUT);
  analogWrite(contrastPin, contrastValues[lcdContrastLevel-1]);
  pinMode(lcdBrightnessPin, OUTPUT);
  analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel-1]);

  //show the first message
  lcd.print("Welcome to the");
  lcd.setCursor(0, 1);
  lcd.print("game!");
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightnessLevel * 3);
  
  randomSeed(analogRead(5)); // random seed from a pin with noise so i get random values everytime i restart the program

  // store the symbols
  for(int i=0; i<noOfRows; ++i){
    symbols[0][i] = up[i];
  }
  for(int i=0; i<noOfRows; ++i){
    symbols[1][i] = down[i];
  }
  for(int i=0; i<noOfRows; ++i){
    symbols[2][i] = right[i];
  }
  for(int i=0; i<noOfRows; ++i){
    symbols[3][i] = left[i];
  }
  for(int i=0; i<noOfRows; ++i){
    symbols[4][i] = buttonRight[i];
  }
  for(int i=0; i<noOfRows; ++i){
    symbols[5][i] = buttonLeft[i];
  }

  //read the highscore names
  for(int i=6; i<15; ++i) {
    c = EEPROM.read(i);
    highscore1Name += (char) c;
  }
  for(int i=16; i<25; ++i) {
    c = EEPROM.read(i);
    highscore2Name += (char) c;
  }
  for(int i=26; i<35; ++i) {
    c = EEPROM.read(i);
    highscore3Name += (char) c;
  }
  Serial.begin(9600);
//  Serial.println(highscore1Name + " " + highscore1);
//  Serial.println(highscore2Name + " " + highscore2);
//  Serial.println(highscore3Name + " " + highscore3);
}

void loop() {
  //Serial.println(yVal);
  playSong(); // the song always plays 
  switch(state) { // depending on the state i do different things
    case 0: 
      welcomeMessage();
      break;
    case 1: 
      showMenu();
      break;
    case 2:
      showAbout();
      break;
    case 3:
      showSettings();
      break;
    case 4:
      showDifficulty();
      break;
    case 5:
      showLCDContrast();
      break;
    case 6:
      showLCDBright();
      break;
    case 7:
      showGameBright();
      break;
    case 8:
      showSong();
      break;
    case 9:
      showHighscores();
      break;
    case 10:
      playGame();
      break;
    case 11:
      showGameOver();
      break;
    case 12:
      showEnterName();
      break;
    case 13:
      changeLetter();
      break;
    case 14:
      overwriteHS();
      break;
    case 15:
      displaySymbolOnMatrix(symbols[symbolsToWrite[showSymbolIndex]]);
      break;
    case 16:
      showRoundComplete();
    default:
      break;
  }
}

void welcomeMessage() {
  //here i only wait for a button press, when it happens i change the state to 1 and clear the LCD
  buttonLeftReading = digitalRead (buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);
  
  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 1;
    buttonWasPressed = false;
  }
}

void showMenu() {
  //here i can scroll through the menu and on button press i go to the specified option
  
  if(currentMenuOption != lastMenuOption) { // i refresh the lcd only when i change the
    lcd.clear();                            // selected option
    switch(currentMenuOption) {
      case 0: 
        menuShowStartGame();
        break;
      case 1: 
        menuShowHighscores();
        break;
      case 2: 
        menuShowSettings();
        break;
      case 3: 
        menuShowAbout();
        break;
      default:
        break;
    }
    lastMenuOption = currentMenuOption;
  }
  if(millis() - lastBlink > blinkInterval) { // the left space near the option will blink
    if(isBlinking) {
      lcd.noBlink();
      lastBlink = millis();
    }
    else {
      lcd.blink();
      lastBlink = millis();
    }
    isBlinking = !isBlinking;
  }

  yVal = analogRead(yPin); // with the y Axis on the joystick i change the option used

  if(yVal > maxThreshold && joyMoved == false) {
    currentMenuOption++;
    joyMoved = true;
    if(currentMenuOption == 4) {
      currentMenuOption = 0;
    }
  }

  if(yVal < minThreshold && joyMoved == false) {
    currentMenuOption--;
    joyMoved = true;
    if(currentMenuOption == -1) {
      currentMenuOption = 3;
    }
  }

  if(yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);
  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  if(buttonWasPressed && !buttonIsPressed) { // after i pressed a button
    buttonWasPressed = false;
    lcd.noBlink();
    lcd.clear();
    switch(currentMenuOption) { // depending on the option selected
      case 0: // i prepare to start the game
        state = 10;
        currentScore = 0;
        roundNr = 1;
        showSymbolIndex = 0;
        roundStarted = false;
        learnMessage = 0;
        break;
      case 1: // i show the highscores
        state = 9;
        currentHS = 0;
        lastHS = -1;
        break;
      case 2: // i go to the settings
        state = 3;
        break;
      case 3: // i show the credits
        state = 2;
        lcd.print("     GAME by Marius Rusu");
        lcd.setCursor(0,1);
        lcd.print("     github.com/Tornerro");
        break;
      default:
        break;
    }
    lastMenuOption = 5;
  }
}

void menuShowStartGame() {
  lcd.print(" Start game");
  lcd.setCursor(1,1);
  lcd.print("Highscores");
  lcd.setCursor(0,0);
}

void menuShowHighscores() {
  lcd.print(" Highscores");
  lcd.setCursor(1,1);
  lcd.print("Settings");
  lcd.setCursor(0,0);
}

void menuShowSettings() {
  lcd.print(" Settings");
  lcd.setCursor(1,1);
  lcd.print("About");
  lcd.setCursor(0,0);
}

void menuShowAbout() {
  lcd.print(" Settings");
  lcd.setCursor(1,1);
  lcd.print("About");
  lcd.setCursor(0,1);
}

void showAbout() {
  if(millis() - lastScrollTime > scrollTime) {
    lcd.scrollDisplayLeft();
    lastScrollTime = millis();
  }
  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 1;
    buttonWasPressed = false;
  }
}

void showSettings() { // exactly the same logic like the main menu
  if(currentSettingsOption != lastSettingsOption) {
    lcd.clear();
    switch(currentSettingsOption) {
      case 0: 
        settingsShowDifficulty();
        break;
      case 1: 
        settingsShowLCDContrast();
        break;
      case 2: 
        settingsShowLCDBright();
        break;
      case 3: 
        settingsShowGameBright();
        break;
      case 4:
        settingsShowSong();
        break;
      case 5:
        settingsShowBack();
      default:
        break;
    }
    lastSettingsOption = currentSettingsOption;
  }
  if(millis() - lastBlink > blinkInterval) {
    if(isBlinking) {
      lcd.noBlink();
      lastBlink = millis();
    }
    else {
      lcd.blink();
      lastBlink = millis();
    }
    isBlinking = !isBlinking;
  }

  yVal = analogRead(yPin);

  if(yVal > maxThreshold && joyMoved == false) {
    currentSettingsOption++;
    joyMoved = true;
    if(currentSettingsOption == 6) {
      currentSettingsOption = 0;
    }
  }

  if(yVal < minThreshold && joyMoved == false) {
    

    currentSettingsOption--;
    joyMoved = true;
    if(currentSettingsOption == -1) {
      currentSettingsOption = 5;
    }
  }

  if(yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);
  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  if(buttonWasPressed && !buttonIsPressed) {
    buttonWasPressed = false;
    lcd.noBlink();
    lcd.clear();
    switch(currentSettingsOption) { // depending on the chosen option I go to
      case 0: // difficulty changer
        state = 4;
        break;
      case 1: // lcd contrast changer
        state = 5;
        break;
      case 2: // lcd brightness changer
      state = 6;
        break;
      case 3: // matrix brightness changerr
        for(int i = 0; i < rows; ++i) {
          lc.setRow(0, i, B11111111); // i turn all the leds of the matrix on
        }
        state = 7;
        break;
      case 4: // song changer
        state = 8;
        break;
      case 5: // go back to the main menu
        state = 1;
        currentSettingsOption = 0;
        break;
      default:
        break;
    }
    lastSettingsOption = 6;
  }
}

void settingsShowDifficulty() {
  lcd.print(" Difficulty");
  lcd.setCursor(1,1);
  lcd.print("LCD Contrast");
  lcd.setCursor(0,0);
}

void settingsShowLCDContrast() {
  lcd.print(" LCD Contrast");
  lcd.setCursor(1,1);
  lcd.print("LCD Brightness");
  lcd.setCursor(0,0);
}

void settingsShowLCDBright() {
  lcd.print(" LCD Brightness");
  lcd.setCursor(1,1);
  lcd.print("Game Brightness");
  lcd.setCursor(0,0);
}

void settingsShowGameBright() {
  lcd.print(" Game Brightness");
  lcd.setCursor(1,1);
  lcd.print("Theme song");
  lcd.setCursor(0,0);
}

void settingsShowSong() {
  lcd.print(" Theme song");
  lcd.setCursor(1,1);
  lcd.print("Back to menu");
  lcd.setCursor(0,0);
}

void settingsShowBack() {
  lcd.print(" Theme song");
  lcd.setCursor(1,1);
  lcd.print("Back to menu");
  lcd.setCursor(0,1);
}

void showDifficulty() { // here i change the difficulty
  if(difficultyLevel != lastDifficultyLevel) {
    EEPROM.update(difficultyAddress, difficultyLevel); // on change i update the eeprom value
    lcd.clear();
    lcd.print(" Difficulty");
    lcd.setCursor(1,1);
    lcd.print("Level: ");
    if(difficultyLevel == 1) {
      lcd.print("  ");
    }
    else {
      lcd.print("< ");
    }
    lcd.print(difficultyLevel);
    if(difficultyLevel != 3) {
      lcd.print(" >");
    }
    lastDifficultyLevel = difficultyLevel;
  }
  
  xVal = analogRead(xPin); // with the x Axis on the joystick i change the difficulty level

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(difficultyLevel < 3) {
      difficultyLevel ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    

    joyMoved = true;
    if(difficultyLevel > 1) {
      difficultyLevel--;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // on button press i go back to trhe settings menu
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastDifficultyLevel = -1;
  }
}

void showLCDContrast() { // same logic as the difficulty
  if(lcdContrastLevel != lastLcdContrastLevel) {
    EEPROM.update(lcdContrastAddress, lcdContrastLevel);
    analogWrite(contrastPin, contrastValues[lcdContrastLevel-1]);
    lcd.clear();
    lcd.print(" LCD Contrast");
    lcd.setCursor(1,1);
    lcd.print("Level: ");
    if(lcdContrastLevel == 1) {
      lcd.print("  ");
    }
    else {
      lcd.print("< ");
    }
    lcd.print(lcdContrastLevel);
    if(lcdContrastLevel != 4) {
      lcd.print(" >");
    }
    lastLcdContrastLevel = lcdContrastLevel;
  }
  
  xVal = analogRead(xPin);

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(lcdContrastLevel < 4) {
      lcdContrastLevel ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    
    joyMoved = true;
    if(lcdContrastLevel > 1) {
      lcdContrastLevel--;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastLcdContrastLevel = -1;
  }
}

void showLCDBright() { // same logic as the difficulty
  if(lcdBrightnessLevel != lastLcdBrightnessLevel) {
    EEPROM.update(lcdBrightnessAddress, lcdBrightnessLevel);
    analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel-1]);
    lcd.clear();
    lcd.print(" LCD Brightness");
    lcd.setCursor(1,1);
    lcd.print("Level: ");
    if(lcdBrightnessLevel == 1) {
      lcd.print("  ");
    }
    else {
      lcd.print("< ");
    }
    lcd.print(lcdBrightnessLevel);
    if(lcdBrightnessLevel != 6) {
      lcd.print(" >");
    }
    lastLcdBrightnessLevel = lcdBrightnessLevel;
  }
  
  xVal = analogRead(xPin);

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(lcdBrightnessLevel < 6) {
      lcdBrightnessLevel ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    
    joyMoved = true;
    if(lcdBrightnessLevel > 1) {
      lcdBrightnessLevel--;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastLcdBrightnessLevel = -1;
  }
}

void showGameBright() { // same logic as the difficulty
  if(matrixBrightnessLevel != lastMatrixBrightnessLevel) {
    EEPROM.update(matrixBrightnessAddress, matrixBrightnessLevel);
    lc.setIntensity(0, matrixBrightnessLevel * 3);
    lcd.clear();
    lcd.print(" Game Brightness");
    lcd.setCursor(1,1);
    lcd.print("Level: ");
    if(matrixBrightnessLevel == 1) {
      lcd.print("  ");
    }
    else {
      lcd.print("< ");
    }
    lcd.print(matrixBrightnessLevel);
    if(matrixBrightnessLevel != 5) {
      lcd.print(" >");
    }
    lastMatrixBrightnessLevel = matrixBrightnessLevel;
  }
  
  xVal = analogRead(xPin);

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(matrixBrightnessLevel < 5) {
      matrixBrightnessLevel ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    
    joyMoved = true;
    if(matrixBrightnessLevel > 1) {
      matrixBrightnessLevel--;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    for(int i = 0; i < rows; ++i) {
      lc.setRow(0, i, B00000000);
    }
    state = 3;
    buttonWasPressed = false;
    lastMatrixBrightnessLevel = -1;
  }
}

void showSong() { // same logic as the difficulty
  if(currentSong != lastSong) {
    thisNote = 0;
    EEPROM.update(songAddress, currentSong);
    lcd.clear();
    lcd.print(" Theme song");
    lcd.setCursor(0,1);
    lcd.print("Song: ");
    
    if(currentSong == 0) {
      lcd.print("  ");
    }
    else {
      lcd.print("< ");
    }
    lcd.print(songsList[currentSong]);
    if(currentSong != 2) {
      lcd.print(" >");
    }

    lastSong = currentSong;
  }

  xVal = analogRead(xPin);

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(currentSong < 2) {
      currentSong ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    
    joyMoved = true;
    if(currentSong > 0) {
      currentSong--;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastSong = -1;
  }
}

void playGame() {
  if(!roundStarted) { // if the round is not started yet
    if(!learnMessage) {
      lcd.print("Learn the bomb's"); // i show a message on the lcd
      lcd.setCursor(0, 1);
      lcd.print("defuse order");
      learnMessage = 1;
    }
    noOfSymbols = difficultyLevel + roundNr; // i calculate the number of symbols i need this round
    if(showSymbolIndex < noOfSymbols) { // if i still need to add symbols
      if(difficultyLevel == 3) { // if the difficulty level is 3 i get a random reverse value
        reverseRandom = random(100);
        reverse = (reverseRandom % 2 == 0);
      }
      else reverse = false; // else the reverse is false
      symbolsToWrite[showSymbolIndex] = random(6); // i get a random number that will be the value of the symbol that has to be typed
      showSymbolTimer = millis(); // timer for the symbol show
      wasShown = 0; // check for the symbol show
      state = 15; // i go to the state that shows the symbol
    }
    else { // if i have all the symbols i need
      currentSymbolIndex = 0; // the index for the typed symbols starts at 0
      secondsLeft = roundSeconds[difficultyLevel-1] + noOfSymbols; // i set the seconds for the round
      roundStarted = true; // i start the round
      lastTimeUpdate = millis(); // start timer for seconds left
      lcd.clear(); 
      lcd.setCursor(0, 0);
      sprintf(levelRoundString, "Level:%d Round:%d", difficultyLevel, roundNr);  // always show the difficulty level and round
      lcd.print(levelRoundString); 
      symbolTyped = -1; // set the typed symbol to -1, the value that means nothing was typed yet
    }
  }
  else { // i am playing the game now
    if(millis() - lastTimeUpdate >= second) { // time left updater
      secondsLeft--;
      lastTimeUpdate = millis();
      if(secondsLeft == 0) { // if i run out of the the game is over
        state = 11;
        gameOverState = 0; // go to the first game over state
        return;
      }
    }
    lcd.setCursor(0, 1);
    if(secondsLeft > 9) {
      sprintf(scoreTimeString, "Score:%d Time:%d", currentScore, secondsLeft); // i print of the second line of the lcd 
    }                                                                         // the score and time left
    else {
      sprintf(scoreTimeString, "Score:%d Time: %d", currentScore, secondsLeft);
    }
    lcd.print(scoreTimeString);

    //depending on the input i give symbolTyped a value
    xVal = analogRead(xPin);
    yVal = analogRead(yPin);
    buttonLeftReading = digitalRead(buttonLeftPin);
    buttonRightReading = digitalRead(buttonRightPin);

    if(yVal > maxThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 1;
      joyMoved = true;
    }

    if(yVal < minThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 0;
      joyMoved = true;
    }

    if(xVal > maxThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 2;
      joyMoved = true;
    }
  
    if(xVal < minThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 3;
      joyMoved = true;
    }

    if(xVal > minThreshold && xVal < maxThreshold && yVal > minThreshold && yVal < maxThreshold) {
      joyMoved = false;
    }

    if(buttonLeftReading == LOW && joyMoved == false) {
    buttonWasPressed = true;
    leftPressed = true;
  }
  
  if(buttonRightReading == LOW && joyMoved == false) {
    buttonWasPressed = true;
    rightPressed = true;
  }
  
    buttonIsPressed = !buttonLeftReading || !buttonRightReading;
    
    if(buttonWasPressed && !buttonIsPressed) {
      if(rightPressed) {
        symbolTyped = 4;
      }
      else if(leftPressed) {
        symbolTyped = 5;
      }
      rightPressed = false;
      leftPressed = false;
      buttonWasPressed = false;
    }

    
  
    if(symbolTyped != -1) { // when i get an input i check the value with the one in the list
      if(symbolTyped == symbolsToWrite[currentSymbolIndex]) { // if the symbol is correct
        currentSymbolIndex++; // go to next symbol in list
        symbolTyped = -1; // reset the input
        if(currentSymbolIndex == noOfSymbols) { // if i finished all the symbols
          showSymbolIndex = 0; // reset the index
          roundNr++; // go to next round
          if(difficultyLevel < 3 && roundNr == 5) { // if i get to round 5 and the difficulty is lower than 3 i increase the difficulty and go to round 1
            roundNr = 1;
            difficultyLevel++;
          }
          roundStarted = false; // round ends
          currentScore += noOfSymbols*difficultyLevel;  // increase the score
          lcd.clear();
          lcd.print("ROUND COMPLETE!");
          learnMessage = 0;
          state = 16; // go to round complete state that will stay on for 2 seconds
          roundCompleteTimer = millis(); // start the round complete state timer
        }
      }
      else { // the symbol is wrong
        roundStarted = false; // the round is over
        gameOverState = 0; // go to the first game over state
        state = 11; // the game is over
        return;
      }
    }
    
  }
}

void displaySymbolOnMatrix(byte v[]) { // here i display the symbol of the combination
  if(wasShown == 0) {
    for(int i = 0; i<8; i++) {
      if(reverse) { // if reverse is true
        if(i == 0 || i ==7)lc.setRow(0, i, v[i]+129); // each corner will have a dot, so i add 129 which is 10000001 in binary
        else lc.setRow(0, i, v[i]);
      }
      else lc.setRow(0, i, v[i]);
    }
  }
  if(millis() - showSymbolTimer >= waitTime[difficultyLevel - 1] && wasShown == 0) { // when the symbols was shown enough
    lc.clearDisplay(0); // i clear the dispplay
    wasShown = 1; 
    showSymbolTimer = millis();
  }

  if(millis() - showSymbolTimer >= waitTime[difficultyLevel - 1] && wasShown == 1) { // i wait a bit with the lcd empty
    state = 10; // go back to the game
    if(reverse) { // if reverse is true
      if(symbolsToWrite[showSymbolIndex]%2 == 0) { // since in my symbols list each even value is the opposite of the next and
        symbolsToWrite[showSymbolIndex]++;        // each unevne value is the opposite of the previous i either increment or decrement
      }                                           // the value of the symbol after i shown it
      else {
        symbolsToWrite[showSymbolIndex]--;
      }
    }
    wasShown = 0;
    showSymbolIndex++;
  }
}

void showRoundComplete() { // i just wait for 2 secound (i did this so i won't use delay and stop the song from playing)
  if(millis() - roundCompleteTimer >= roundCompleteMessageTime) {
    state = 10;
    lcd.clear();
  }
}

void showGameOver() { // game over state
  switch(gameOverState) {
    case 0: // initial state, only happens once
      gameOver0();
      break;
    case 1:
      gameOver1();
      break;
    case 2:
      gameOver2();
      break;
    case 3:
      gameOver3();
      break;
    default:
      break;
  }
}

void gameOver0() {
  lcd.print("    GAME OVER"); // show the game over
  gameOverTimer = millis(); // and start the timer
  gameOverState = 1;
  gameOverMessageVisible = true;
}

void gameOver1() {
  if(millis() - gameOverTimer >= GameOverBlinkInterval) { // make the game over blink
    if(gameOverMessageVisible) {
      lcd.clear();
      gameOverMessageVisible = false;
    }
    else {
      lcd.print("    GAME OVER");
      gameOverMessageVisible = true;
    }
    gameOverTimer = millis();
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // on button press i go to the next game over state and show to score
    gameOverState = 2;
    buttonWasPressed = false;
    lcd.clear();
    lcd.print("    You got");
    lcd.setCursor(0, 1);
    sprintf(scoreMessage, "   %d points!", currentScore); 
    lcd.print(scoreMessage); 
  }
}

void gameOver2() {
  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // on button press
    if(currentScore > highscore3) { // if the score qualifies for highscore
      buttonWasPressed = false;
      gameOverState = 3; // i go to another game over state
      lcd.clear();
      lcd.print("New highscore!"); // and show a special message
      lcd.setCursor(0, 1);
      lcd.print("Enter your name");
    }
    else { // else i go back to the main menu
      state = 1;
      buttonWasPressed = false;
      lcd.clear();
    }
  }
}

void gameOver3() { // here i only wait for a button press
  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // then the button is pressed i go the the enter name state and prepare the variables for it
    state = 12;
    currentName = "         ";
    lastName = "";
    buttonWasPressed = false;
    letterIndex = 0;
    lastLetterIndex = -1;
    onOk = false;
    lastLetter = '0';
    lcd.clear();
    
  }
}

void showEnterName() { // enter name menu
  if(currentName != lastName) { // refresh the lcd on change of the name
    lastName = currentName;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Name: ");
    lcd.print(currentName);
    lcd.setCursor(7, 1);
    lcd.print("OK");
  }
  if(letterIndex != lastLetterIndex) { // change the cursor location
    lastLetterIndex = letterIndex;
    lcd.setCursor(6 + letterIndex, 0);
  }
  
  if(millis() - lastBlink > blinkInterval) { // blinking logic
    if(isBlinking) {
      lcd.noBlink();
      lastBlink = millis();
    }
    else {
      lcd.blink();
      lastBlink = millis();
    }
    isBlinking = !isBlinking;
  }

  xVal = analogRead(xPin);

  if(onOk == false) { // if i am not on the ok option

  // i change the letter index using the x axis
  
    if(xVal > maxThreshold  && joyMoved == false) {
      joyMoved = true;
      if(letterIndex < 8) {
        letterIndex ++;
      }
    }
  
    if(xVal < minThreshold  && joyMoved == false) {
      joyMoved = true;
      if(letterIndex >0) {
        letterIndex --;
      }
    }
    
  }

  yVal = analogRead(yPin);

  if(yVal < minThreshold && joyMoved == false) { // i go up
    joyMoved = true;
    if(onOk == true) { // and leave the ok option
      Serial.println(yVal);
      onOk = false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Name: ");
      lcd.print(currentName);
      lcd.setCursor(7, 1);
      lcd.print("OK");
      lcd.setCursor(6 + letterIndex, 0);
    }
  }

  if(yVal > maxThreshold && joyMoved == false) { // i go down
    joyMoved = true;
    if(onOk == false) { // and select the ok option
      Serial.println(yVal);
      onOk = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Name: ");
      lcd.print(currentName);
      lcd.setCursor(7, 1);
      lcd.print("OK");
      lcd.setCursor(6, 1);
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold && yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // on button press
    buttonWasPressed = false;
    if(onOk == false) { // if i am on a letter i go to the letter changing state
      lastLetter = 0;
      lastName = "";
      lcd.noBlink();
      lastLetterIndex = -1;
      buttonWasPressed = false;
      letter = currentName[letterIndex];
      lcd.clear();
      state = 13;
    }
    else { // else i go to the overwrite highscores state
      state = 14;
    }
  }
}

void changeLetter() {
  if(letter != lastLetter) { // on letter change i refresh the lcd
    lcd.clear();
    lastLetter = letter;
    lcd.setCursor(0, 0);
    lcd.print("Letter: ");
    lcd.print(letter);
  }

  xVal = analogRead(xPin); // using the x axis i change the letter value 

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(letter == ' ') { // first value is ' ', then all the letter
      letter = 'A';
    }
    else if(letter < 'Z') {
      letter ++;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(letter == 'A') {
      letter = ' ';
    }
    else if(letter > 'A') {
      letter --;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);

  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  
  if(buttonWasPressed && !buttonIsPressed) { // on button press i store the letter in the name and go back to the enter name state
    currentName[letterIndex] = letter;
    buttonWasPressed = false;
    letter = currentName[letterIndex];
    lcd.clear();
    state = 12;
  }
}

void overwriteHS() { // here i overwrite the highscore values and names
  if(currentScore > highscore1) {
    highscore3 = highscore2;
    highscore3Name = highscore2Name;
    EEPROM.update(highscore3Address, highscore3); // overwrite the score
    for(int i = 16; i <= 24; ++i) {
      c = EEPROM.read(i); // get the letter in the name that goes down
      EEPROM.update(i + 10, c);  // overwrite the name
    }

    //same logic as above
    highscore2 = highscore1;
    highscore2Name = highscore1Name;
    EEPROM.update(highscore2Address, highscore2);
    for(int i = 6; i <= 14; ++i) {
      c = EEPROM.read(i);
      EEPROM.update(i + 10, c);
    }

    highscore1 = currentScore;
    highscore1Name = currentName;
    EEPROM.update(highscore1Address, highscore1);
    for(int i = 0; i < 9; ++i) {
      EEPROM.update(i + 6, currentName[i]); // write the current name
    }
  }
  else if(currentScore > highscore2) {
    highscore3 = highscore2;
    highscore3Name = highscore2Name;
    EEPROM.update(highscore3Address, highscore3);
    for(int i = 16; i <= 24; ++i) {
      c = EEPROM.read(i);
      EEPROM.update(i + 10, c); 
    }

    highscore2 = currentScore;
    highscore2Name = currentName;
    EEPROM.update(highscore2Address, highscore2);
    for(int i = 0; i < 9; ++i) {
      EEPROM.update(i + 16, currentName[i]); // write the current name
    }
  }
  else if(currentScore > highscore3) {
    highscore3 = currentScore;
    highscore3Name = currentName;
    EEPROM.update(highscore3Address, highscore3);
    for(int i = 0; i < 9; ++i) {
      EEPROM.update(i + 26, currentName[i]); // write the current name
    }
  }
  state = 1;
}

void showHighscores() { // here i shgow the highscores
  if(currentHS != lastHS) { // refresh the lcd on highscore option change
    lastHS = currentHS;
    Serial.print("HS  ");
    Serial.print(currentHS);
    Serial.print(" ");
    Serial.println(lastHS);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("# Name    Score");
    lcd.setCursor(0, 1);
    switch(currentHS) {
      case 0:
        currentScore = highscore1;
        hsName = highscore1Name;
        break;
      case 1:
        currentScore = highscore2;
        hsName = highscore2Name;
        break;
      case 2:
        currentScore = highscore3;
        hsName = highscore3Name;
        break;
      default:
        break; 
    }
    //print the values of the highscore on the lcd
    lcd.print(currentHS+1);
    lcd.setCursor(2, 1);
    lcd.print(hsName);
    lcd.setCursor(12, 1);
    lcd.print(currentScore);
  }

  yVal = analogRead(yPin); // witht he y axis i change the highscore option

  if(yVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(currentHS < 2) {
      currentHS++;
    }
  }

  if(yVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(currentHS > 0) {
      currentHS--;
    }
  }

  if(yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonLeftReading = digitalRead(buttonLeftPin);
  buttonRightReading = digitalRead (buttonRightPin);
  if(buttonLeftReading == LOW || buttonRightReading == LOW) {
    buttonWasPressed = true;
  }
  buttonIsPressed = !buttonLeftReading || !buttonRightReading;
  if(buttonWasPressed && !buttonIsPressed) { // on button press i go back to the  main menu
    buttonWasPressed = false;
    lcd.clear();
    state = 1;
  }
}

void playSong() { // i got the main logic from https://github.com/robsoncouto/arduino-songs and changed it to loop
  switch(currentSong) {
    case 0:
      return;
      break;
    case 1:
      divider = song1[thisNote + 1];
      break;
    case 2:
      divider = song2[thisNote + 1];
      break;
    default:
      break;
  }

  if (divider > 0) {
    // regular note, just proceed
    noteDuration = (wholenote[currentSong]) / divider;
  } else if (divider < 0) {
    // dotted notes are represented with negative durations!!
    noteDuration = (wholenote[currentSong]) / abs(divider);
    noteDuration *= 1.5; // increases the duration in half for dotted notes
  }

  // we only play the note for 90% of the duration, leaving 10% as a pause
  switch(currentSong) {
  case 1:
    tone(buzzerPin, song1[thisNote], noteDuration*0.9);
    break;
  case 2:
    tone(buzzerPin, song2[thisNote], noteDuration*0.9);
    break;
  default:
    break;
  }
  // Wait for the specief duration before playing the next note.
  if(millis() - songTimer >= noteDuration) {
    songTimer = millis();
    thisNote += 2; // each note has the duration, so i increment by 2
    if(thisNote >= notes[currentSong]* 2) thisNote = 0;
    noTone(buzzerPin);
  }
}
