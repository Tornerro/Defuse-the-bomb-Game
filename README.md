# Defuse-the-bomb-Game
My first project for the Introduction To Robotics course in my third year of Bachelor's at FMI Unibuc.

## Game description
Defuse the bomb by submitting the correct combination of symbols. At the beginning of each round the matrix will show the defuse combination, make sure you can remember it. At higher difficulty you might be tricked by a reverse symbol.

## How to play
At the start of a round the matrix will show the symbols you need to submit. Upon completeing a round the number the round number will increase. If the round number gets to 5 and the difficulty level is lower than 3, the difficulty level will increase by 1 and the round number will be reset to 1. At difficulty level 3 the round number will increase forever. At difficulty level 3 the symbols might be reversed by having a dot in each corner so you will have to submit the opposite symbol. Upon completein a round you will get difficulty_level * noOfSymbols points. If your score is in the top 3, you can submit a name for the Highscores section.

## Settings options
The settings are saved in EEPROM so they remain even after a reset of the game. The options are:
* Difficulty: 3 levels( level 1 has a basic wait time for symbols, level 2 has less time to read the symbols and level 3 has the same time as level 2, but you can get reversed symbols)
* LCD Contrast: 4 levels(change the contrast by using the X axis of the joystick)
* LCD Brightness: 6 levels(same logic as contrast)
* Game Brightness: when this option is opened all the matrix LEDs will turn on. There are 5 levels(1-5) and upon changing levels the dunction setIntensity(0, 3*level) will be called
* Theme song: 3 values(OFF, Song1, Song2): the songs will play all the time until you turn it off. Unofortunately the songs are pretty short and become annoying fast since I was running out of space.

## How the code works
I use a lot of states and I switch through them in the loop function. The first state is 0 and it prints on the LCD the "Welcome to the game message". Upon the press of any button the state becomes 1(the state of the Main menu). Upon changing the main menu option using the Y axis of the joystick the LCD will display a different option. Upon pressing any button the state will change again and go to the corresponding option:
* Start game: the variables for the game will be intialized and the round symbols will be generated and showed on the matrix. After that the player has a timer to complete the combination. If the tiemr runs out or the symbol il incorrect the players loses and the Game Over state will take over and will show the game over messges upon pressnig any button. If the score is a new highscore the player will get a message and will be prompted to enter a name. You can enter 9 characters including space between the letters.
* Highscores: the first line of the LCD will always be a string composed of #  .. Name .. Score and the second line will have the value of one of the top 3 highscores. Using the Y axis of the joystick you can change the current highscore shown.
* Settings: shows a list of settings that you can choose to modify. Each setting is saved in EEPROM. The settings are described in the Setting Option above.
* About: shows on the LCD a sliding message containing my name and github link.

## Logic of trickier parts:
* Generating each round: before a round starts the varaible roundStarted is false. When it's false I check if the number of generated symbols is equal to the number of symbols i need this round. If it's lower i generate a symbol using the random function which will give me a number form 0 to 5, corresponding to the position in the sybols list. If the difficulty level is 3 i generate another random number from 0 to 99(random idea) and if the number is even the symbol will be reversed(when displayed each cornen will have a dot turned on and the player has to submit the opposite symbol ex: up -> down, R button -> L button). When i generated all the symbols i need the round will start and depending on the input the written symbol will be modified and compared to the symbols that has to be written. If the symbol is correct the curren symbol vaue will be reset. If all the symbols are correctly written by the player the LCd will print a ROUND COMPLETE message for 2 seconds and the current score will increase by difficulty_level* numberOfSymbols then the next round will start. During the round the LCD will display on the first line the difficulty level and the round number and on the second line the current score and the time left, which will update each second.
* Entering the name for a new highscore: After scoring a highscore the player will have to enter a name containing 9 characters(characters can be empty and the displayed symbol will be space). The slot that the user is currently on will be blinking and upon pressing any button a new state will trigger, allowing the change of the character using the X axis of the controller. After setting the character the user can press any button and the state will go back to the name state, allowing the user to choose the position they wish to modify. After the name has been entered the user can use the Y axis and go down to the OK option, or go back up fomr the OK to the letters of the name. Afet pressing the OK the highscores overwrite function will execute, then the user will go back to the main menu state.

## Components
* 8x8 LED Matrix
* LCD Display
* Arduino Uno Board
* Joystick
* 2 capacitors (1x 104 ceramic and 1x 10 uF)
* 3 breadboards (of different sizes)
* A lot of cables
* Matrix driver
* 2 buttons
* 1 buzzer

## Setup
![homework1](setup.jpeg)

## Video showcase: https://www.youtube.com/watch?v=pHbv_DEskZ0
