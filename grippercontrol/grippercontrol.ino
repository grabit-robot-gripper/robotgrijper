/**@file 
 * 
 * @brief A sketch for reading input from the user interface and sending commands to the TMCM-KR-842.
 * 
 *
 */


/*
 * true: gripper moves up, motor rotates counterclockwise.
 * false: gripper moves down, motor rotates clockwise.
 */
#include <TmclCommand.h>
#include <avr/interrupt.h>  
#include <avr/io.h>

/**
 * The pin used for the input for the end limit switch.
 */
#define limit 2

//SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
int incomingByte;      // a variable to read incoming serial data into


/**
 * Initialize an instance of the class TmclCommand.
 */
TmclCommand command;


/* 
 * volatile is used because the 
 */
volatile bool endLimitReached = false;

/**
 * Array used to store the bytes of the stop Command
 */
unsigned char stopArray[9];
unsigned char rorArray[9];
unsigned char rolArray[9];

/*
 * 
 */
unsigned char mvpArray[9];


unsigned char mvpArray2[9];

/**
 * @brief Variabele used to store the state of the machine.
 * 
 * The state is checked in the interrupt routine, 
 * to set endLimitReached to 'true' in a fitting way.
 */
String state = "";


void setup(){
  // Serial used for feedback while developping.
  Serial.begin(9600);

  // Serial used for listening to commands from the user interface.
  Serial2.begin(9600);
  
  // Serial used for sending commands to the TMCM-KR-842
  Serial3.begin(9600);
  while (!Serial3) {
    ; // wait for serial port to connect. Needed for native USB port only
    
  }

  // Setup for the end limit switch.
  pinMode(limit, INPUT);
  attachInterrupt(digitalPinToInterrupt(limit), isrEndLimit, RISING); 
  
  unsigned char *stopCommand = stopCommand = command.stop();

  Serial.println("stop command");
  for(int i=0; i<9; i++){
    String str = String(stopCommand[i], HEX);
    stopArray[i] = stopCommand[i];
    Serial.println(str);
  }

  unsigned char *mvpCommand = command.mvp(60000, true);
  Serial.println("mvp command");
  for(int i=0; i<9; i++){
    String str = String(mvpCommand[i], HEX);
    mvpArray[i] = mvpCommand[i];
    Serial.println(str);
  }

  unsigned char *mvpCommand2 = command.mvp(60000, false);
  Serial.println("mvp command");
  for(int i=0; i<9; i++){
    String str = String(mvpCommand2[i], HEX);
    mvpArray2[i] = mvpCommand2[i];
    Serial.println(str);
  }

  unsigned char *rorCommand = command.ror(350);
  Serial.println("ror command");
  for(int i=0; i<9; i++){
    String str = String(rorCommand[i], HEX);
    rorArray[i] = rorCommand[i];
    Serial.println(str);
  }

  unsigned char *rolCommand = command.rol(800);
  Serial.println("ror command");
  for(int i=0; i<9; i++){
    String str = String(rolCommand[i], HEX);
    rolArray[i] = rolCommand[i];
    Serial.println(str);
  }

  //Serial3.write(mvpArray, sizeof(mvpArray));
  //Serial3.write(mvpCommand);

  //Serial3.write(rorArray, sizeof(rorArray));

  //Serial3.write(mvpArray2, sizeof(mvpArray2));
  //Serial3.write(mvpCommand2);

  Serial3.write(stopArray, sizeof(stopArray));
}
/**
 * @brief The loop listens to messages sent by the GUI.
 * 
 * 
 */
void loop(){
  if (Serial2.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial2.read();
    // If the incoming byte represents a capital S (ASCII), close the gripper.
    if (incomingByte == 'S') {
      Serial.println("sluiten");

      // Change state to "sluiten".
      state = "sluiten";
      Serial.println("state: " + state);
      Serial.println("");

      /* 
       *  Actually send the command to move the gripper 
       */
      Serial3.write(mvpArray, sizeof(mvpArray));
      
    } 
    else if (incomingByte == 'O') {
      Serial.println("openen");
      Serial.println("");

      // Change state to "openen".
      state = "openen";

      // Some feedback in the serial monitor.
      Serial.println("state: " + state);
      //Serial3.write(rolArray, sizeof(rolArray));

      Serial3.write(mvpArray2, sizeof(mvpArray2));

      /*
      Serial.println("ror command");
      for(int i=0; i<9; i++){
        String str = String(rorArray[i], HEX);
        Serial.println(str);
      }*/
      Serial3.read(); // weg?
    }
  }
  if (Serial3.available() > 0) {
        //Serial.println(Serial3.read());
  }
  
  /*if (risingEdge == true){
    Serial3.write(stopArray, sizeof(stopArray));
    risingEdge = false;
  }*/
  

}

/**
 * @brief Interrupt routine for the end limit switch.
 * 
 * 
 * When the used end limit switch is pressed, the used input pin will go from
 * LOW to HIGH. On the rising edge, this interrupt routine will be called.
 * A rising edge is generated when the switch is pressed and unpressed.
 * Because the stop command must only sent to the TMCM when the gripper is opening,
 * the state must be checked.
 * 
 * The rising edge is also checked for debouncing. If the last time since the interrupt is
 * less than 350 ms, risingEdge will not be set to true.
 */
void isrEndLimit() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 350 && state == "openen")
  {
    Serial.println("endLimitReached");
    endLimitReached = true;
    Serial3.write(stopArray, sizeof(stopArray));
  }
  lastInterruptTime = interruptTime;
}

