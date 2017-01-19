/**@file 
 * 
 * @brief A sketch for getting input from the user and print the fitting feedback.
 * 
 * The user gives input through the keypad.
 * Some simple instructions are printed on the LCD.
 * A simple finite state machine is used to model the different states.
 * 
 */

#include <Keypad.h>
#include "Arduino.h"
#include <SPI.h>

// Hardware-specific library for TFT board
#include "Adafruit_GFX.h"

/**
 * Specific library for the TFT-board.
 * 
 */
#include <MCUFRIEND_kbv.h>

/**
 * For more information about this library:
 * http://playground.arduino.cc/Code/FiniteStateMachine
 */
#include <FiniteStateMachine.h>

/**
 * Instantiation of class.
 */
MCUFRIEND_kbv tft;

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


// The keypad has four rows.
const byte ROWS = 4;

// The keypad has four rows.
const byte COLS = 3;


// Mapping of the keys on the keypad.
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Connect these pins on the Arduino Mega to the row pinouts of the keypad
byte rowPins[ROWS] = {44, 43, 42, 41};

// Connect these pins on the Arduino Mega to the column pinouts of the keypad
byte colPins[COLS] = {40, 39, 38};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Identifier of the tft lcd. Must return 7789
uint16_t g_identifier;

const byte NUMBER_OF_STATES = 2;

State closing = State(&printClosing);
State opening = State(&printOpening);

// Initializing of the FSM. Start in state 'closing'.
FSM robotStateMachine = FSM(closing);

void setup(void) {
    Serial.begin(9600);
    Serial.println("start");
    Serial1.begin(9600);
    
    /* 
     *  g_identifier must be determined, 
     *  before it is given as an input for tft.begin();
     */
    static uint16_t identifier;
    g_identifier = tft.readID();
    if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) {
      g_identifier = 0x9481;
    }
    
    if (g_identifier == 0xFFFF) {
      g_identifier = 0x9341;
    }
    tft.begin(g_identifier);
    tft.setRotation(1);
    tft.fillScreen(BLACK);
}

byte stateNumber = 0;

void loop(void) {
    switch (stateNumber){
      case 0: robotStateMachine.transitionTo(closing); break;
      case 1: robotStateMachine.transitionTo(opening); break;
    }
   robotStateMachine.update();
}

/**
 * Prints instructions on the keypad.
 */
void printClosing(void){
  printHeader();
  tft.setTextSize(2);
  tft.println("Press '*' to close.");
  char key = keypad.getKey();
  Serial.println(key);
  if (key == '*'){
    tft.print(key);
    stateNumber = ++stateNumber % NUMBER_OF_STATES;
    Serial.write("S");
    Serial1.write("S");
    tft.fillScreen(BLACK);
  }
}

/**
 * Prints instructions on the keypad.
 */
void printOpening(void){
  printHeader();
  tft.setTextSize(2);
  tft.println("Press '*' to open.");
  char key = keypad.getKey();
  Serial.println(key);
  if (key == '*'){
    tft.print(key);
    stateNumber = ++stateNumber % NUMBER_OF_STATES;
    Serial.write("O");
    Serial1.write("O");
    tft.fillScreen(BLACK);
  }
}

/**
 * Prints the title of the project.
 */
void printHeader(void){
  tft.setCursor(50, 0);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.println("GRABIT");
  tft.println();
}

