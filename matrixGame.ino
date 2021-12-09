#include<LiquidCrystal.h>
#include<EEPROM.h>
#include "LedControl.h" 

byte up[8] = {
  B00011000,
  B00011000,
  B00011000,
  B00011000,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

byte down[8] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B00011000,
  B00011000,
  B00011000,
  B00011000
};

byte right[8] = {
  B00010000,
  B00110000,
  B01110000,
  B11111111,
  B11111111,
  B01110000,
  B00110000,
  B00010000
};

byte left[8] = {
  B00001000,
  B00001100,
  B00001110,
  B11111111,
  B11111111,
  B00001110,
  B00001100,
  B00001000
};

byte circle[8] = {
  B00111100,
  B01000010,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B01000010,
  B00111100
};

byte symbols[5][8];

const int RS = 13;
const int enable = 8;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int rows = 8;
const int cols = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
 
const int difficultyAddress = 0;
const int lcdContrastAddress = 1;
const int lcdBrightnessAddress = 2;
const int matrixBrightnessAddress = 3;
const int songAddress = 4;


const int contrastPin = 6;
const int buttonPin = 7;
const int lcdBrightnessPin = 9;

const int xPin = A0;
const int yPin = A1;

int xVal = 0;
int yVal = 0;

bool joyMoved = 0;
int currentMenuOption = 0;
int lastMenuOption = -1;

int currentSettingsOption = 0;
int lastSettingsOption = -1;

const int minThreshold = 200;
const int maxThreshold = 600;

const int blinkInterval = 500;
unsigned long lastBlink = 0;
bool isBlinking = 0;
unsigned long lastScrollTime = 0;
const int scrollTime = 500;

int contrastValues[] = {85, 110, 135, 150};
int lcdBrightnessValues[] = {5, 55, 105, 155, 205, 255};

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

int state = 0;

bool buttonReading = HIGH;
bool lastReading = HIGH;
bool buttonIsPressed = false;
bool buttonWasPressed = false;

int difficultyLevel;
int lastDifficultyLevel = 0;

int lcdContrastLevel;
int lastLcdContrastLevel = 0;

int lcdBrightnessLevel;
int lastLcdBrightnessLevel = 0;

int matrixBrightnessLevel;
int lastMatrixBrightnessLevel = 0;

int currentSong = 1;
int lastSong = 0;

int randNumber;
int currentScore;
int lastScore;
int roundNr;
bool roundStarted = false;
int noOfSymbols;
int symbolsToWrite[30];
int currentSymbolIndex;
int secondsLeft;
const int second = 1000;
unsigned long lastTimeUpdate;
int symbolTyped;

char levelRoundString[20];
char scoreTimeString[20];
char scoreMessage[20];

int gameOverState;
unsigned long gameOverTimer;
const int GameOverBlinkInterval = 1000;
bool gameOverMessageVisible;


void setup() {
  difficultyLevel = EEPROM.read(difficultyAddress);
  lcdContrastLevel = EEPROM.read(lcdContrastAddress);
  lcdBrightnessLevel = EEPROM.read(lcdBrightnessAddress);
  matrixBrightnessLevel = EEPROM.read(matrixBrightnessAddress);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  lcd.begin(50, 2);
  pinMode(contrastPin, OUTPUT);
  analogWrite(contrastPin, contrastValues[lcdContrastLevel-1]);
  pinMode(lcdBrightnessPin, OUTPUT);
  analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel-1]);
  lcd.print("Welcome to the");
  lcd.setCursor(0, 1);
  lcd.print("game!");
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightnessLevel * 3);
  for(int i = 0; i < rows; ++i) {
    lc.setRow(0, i, B11111111);
  }
  randomSeed(analogRead(5));
  for(int i=0; i<8; ++i){
    symbols[0][i] = up[i];
  }
  for(int i=0; i<8; ++i){
    symbols[1][i] = down[i];
  }
  for(int i=0; i<8; ++i){
    symbols[2][i] = right[i];
  }
  for(int i=0; i<8; ++i){
    symbols[3][i] = left[i];
  }
  for(int i=0; i<8; ++i){
    symbols[4][i] = circle[i];
  }
  Serial.begin(9600);
}

void loop() {
//  Serial.println(state);
  switch(state) {
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
      //showHighscores
      break;
    case 10:
      playGame();
      break;
    case 11:
      showGameOver();
      break;
    default:
      break;
  }
}

void welcomeMessage() {
  //Serial.println(0);
  buttonReading = digitalRead (buttonPin);
  
  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 1;
    buttonWasPressed = false;
  }
}

void showMenu() {
  //Serial.println(1);
  
  if(currentMenuOption != lastMenuOption) {
    lcd.clear();
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
  if(millis() - lastBlink > blinkInterval) {
    if(isBlinking) {
      lcd.noBlink();
      lastBlink = millis();
    }
    else {
      lcd.blink();
      lastBlink = millis();
    }
  }

  yVal = analogRead(yPin);

  if(yVal > maxThreshold && joyMoved == false) {
    currentMenuOption--;
    joyMoved = true;
    if(currentMenuOption == -1) {
      currentMenuOption = 3;
    }
  }

  if(yVal < minThreshold && joyMoved == false) {
    currentMenuOption++;
    joyMoved = true;
    if(currentMenuOption == 4) {
      currentMenuOption = 0;
    }
  }

  if(yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);
  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  buttonIsPressed = !buttonReading;
  if(buttonWasPressed && !buttonIsPressed) {
    buttonWasPressed = false;
    lcd.noBlink();
    lcd.clear();
    switch(currentMenuOption) {
      case 0:
        state = 10;
        currentScore = 0;
        roundNr = 1;
        roundStarted = false;
        break;
      case 1:
        break;
      case 2:
        state = 3;
        break;
      case 3:
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
  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 1;
    buttonWasPressed = false;
  }
}

void showSettings() {
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
  }

  yVal = analogRead(yPin);

  if(yVal > maxThreshold && joyMoved == false) {
    currentSettingsOption--;
    joyMoved = true;
    if(currentSettingsOption == -1) {
      currentSettingsOption = 5;
    }
  }

  if(yVal < minThreshold && joyMoved == false) {
    currentSettingsOption++;
    joyMoved = true;
    if(currentSettingsOption == 6) {
      currentSettingsOption = 0;
    }
  }

  if(yVal > minThreshold && yVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);
  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  buttonIsPressed = !buttonReading;
  if(buttonWasPressed && !buttonIsPressed) {
    buttonWasPressed = false;
    lcd.noBlink();
    lcd.clear();
    switch(currentSettingsOption) {
      case 0:
        state = 4;
        break;
      case 1:
        state = 5;
        break;
      case 2:
      state = 6;
        break;
      case 3:
        state = 7;
        break;
      case 4:
        state = 8;
        break;
      case 5:
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

void showDifficulty() {
  if(difficultyLevel != lastDifficultyLevel) {
    EEPROM.update(difficultyAddress, difficultyLevel);
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
    if(difficultyLevel != 5) {
      lcd.print(" >");
    }
    lastDifficultyLevel = difficultyLevel;
  }
  
  xVal = analogRead(xPin);

  if(xVal > maxThreshold && joyMoved == false) {
    joyMoved = true;
    if(difficultyLevel > 1) {
      difficultyLevel--;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(difficultyLevel < 5) {
      difficultyLevel ++;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastDifficultyLevel = -1;
  }
}

void showLCDContrast() {
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
    if(lcdContrastLevel > 1) {
      lcdContrastLevel--;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(lcdContrastLevel < 4) {
      lcdContrastLevel ++;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastLcdContrastLevel = -1;
  }
}

void showLCDBright() {
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
    if(lcdBrightnessLevel > 1) {
      lcdBrightnessLevel--;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(lcdBrightnessLevel < 6) {
      lcdBrightnessLevel ++;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastLcdBrightnessLevel = -1;
  }
}

void showGameBright() {
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
    if(matrixBrightnessLevel > 1) {
      matrixBrightnessLevel--;
    }
  }

  if(xVal < minThreshold && joyMoved == false) {
    joyMoved = true;
    if(matrixBrightnessLevel < 5) {
      matrixBrightnessLevel ++;
    }
  }

  if(xVal > minThreshold && xVal < maxThreshold) {
    joyMoved = false;
  }

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastMatrixBrightnessLevel = -1;
  }
}

void showSong() {
  if(currentSong != lastSong) {
    lcd.clear();
    lcd.print(" Theme song");
    lcd.setCursor(0,1);
    lcd.print("To be added :(");
    lastSong = currentSong;
  }

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    lcd.clear();
    state = 3;
    buttonWasPressed = false;
    lastSong = -1;
  }
}

void playGame() {
  if(!roundStarted) {
    lcd.print("Learn the bomb's");
    lcd.setCursor(0, 1);
    lcd.print("defuse order");
    noOfSymbols = difficultyLevel + roundNr;
    for(int i = 0;i < noOfSymbols; ++i) {
      symbolsToWrite[i] = random(5);
      displaySymbolOnMatrix(symbols[symbolsToWrite[i]]);
    }
    currentSymbolIndex = 0;
    secondsLeft = 30;
    roundStarted = true;
    lastTimeUpdate = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(levelRoundString, "Level:%d Round:%d", difficultyLevel, roundNr); 
    lcd.print(levelRoundString); 
    symbolTyped = -1;
  }
  else {
    if(millis() - lastTimeUpdate >= second) {
      secondsLeft--;
      lastTimeUpdate = millis();
      if(secondsLeft == 0) {
        state = 11;
        gameOverState = 0;
        return;
      }
    }
    lcd.setCursor(0, 1);
    if(secondsLeft > 9) {
      sprintf(scoreTimeString, "Score:%d Time:%d", currentScore, secondsLeft);
    }
    else {
      sprintf(scoreTimeString, "Score:%d Time: %d", currentScore, secondsLeft);
    }
    lcd.print(scoreTimeString);
    xVal = analogRead(xPin);
    yVal = analogRead(yPin);
    buttonReading = digitalRead(buttonPin);

    if(yVal > maxThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 0;
      joyMoved = true;
    }

    if(yVal < minThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 1;
      joyMoved = true;
    }

    if(xVal > maxThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 3;
      joyMoved = true;
    }
  
    if(xVal < minThreshold && joyMoved == false && buttonWasPressed == false) {
      symbolTyped = 2;
      joyMoved = true;
    }

    if(xVal > minThreshold && xVal < maxThreshold && yVal > minThreshold && yVal < maxThreshold) {
      joyMoved = false;
    }

    if(buttonReading == LOW && joyMoved == false) {
    buttonWasPressed = true;
  }
  
    buttonIsPressed = !buttonReading;
    
    if(buttonWasPressed && !buttonIsPressed) {
      symbolTyped = 4;
      buttonWasPressed = false;
    }

    
  
    if(symbolTyped != -1) {
      Serial.println(symbolTyped);
      if(symbolTyped == symbolsToWrite[currentSymbolIndex]) {
        currentSymbolIndex++;
        symbolTyped = -1;
        if(currentSymbolIndex == noOfSymbols) {
          roundNr++;
          roundStarted = false;
          currentScore += noOfSymbols; 
          lcd.clear();
          lcd.print("ROUND COMPLETE!");
          delay(2000);
          lcd.clear();
        }
      }
      else {
        roundStarted = false;
        gameOverState = 0;
        state = 11;
        return;
      }
    }
    
  }
}

void displaySymbolOnMatrix(byte v[]) {
  for(int i = 0; i<8; i++) {
    lc.setRow(0, i, v[i]);
  }
  delay(1000);
  lc.clearDisplay(0);
  delay(800);
}

void showGameOver() {
  switch(gameOverState) {
    case 0:
      gameOver0();
      break;
    case 1:
      gameOver1();
      break;
    case 2:
      gameOver2();
      break;
    default:
      break;
  }
}

void gameOver0() {
  lcd.print("    GAME OVER");
  gameOverTimer = millis();
  gameOverState = 1;
  gameOverMessageVisible = true;
}

void gameOver1() {
  if(millis() - gameOverTimer >= GameOverBlinkInterval) {
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

  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
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
  buttonReading = digitalRead(buttonPin);

  if(buttonReading == LOW) {
    buttonWasPressed = true;
  }
  
  buttonIsPressed = !buttonReading;
  
  if(buttonWasPressed && !buttonIsPressed) {
    state = 1;
    buttonWasPressed = false;
    lcd.clear();
  }
}
