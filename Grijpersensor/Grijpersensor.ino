/**@file 
 * 
 * @brief A sketch for reading sensor values and sending it with Bluetooth to a serial monitor. 
 * 
 * 
 * Grijpersensor.ino is a sketch to connect the HC-06 module and communicate using the serial monitor.
 * When the HC-06 is powered, the HC-06 defaults to communication mode.
 * The default baud rate for communication mode is 9600.
 * 
 */
 

#include <SoftwareSerial.h>

/**
 * The bluetooth SoftwareSerial for the bluetooth HC-06.
 * The first argument is RX on the Arduino.
 * The second argument is TX on the Arduino.
 * The RX and TX of the Arduino are connected to the TX and RX of the Bluetooth, respectively.
 * The Arduino-TX is connected to the Bluetooth-RX through a voltage divider
 */
SoftwareSerial BTserial(2, 3);

/**
 * 
 * Amount of force sensors.
 */
#define forceSensorCount 4

char c = ' ';
 
/** BTconnected will be have the value 'false' whenthe Bluetooth is not connected and 'true' when connected */
boolean BTconnected = false;
 
// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;

/** Array containg the pins of the force sensors. */
unsigned int forceSensors[forceSensorCount] = {A1, A2, A3, A4};

/** Array containing the force sensor values. */
int forceSensorValues[forceSensorCount];

/// This function sets up the communication with the bluetooth.
void setup() 
{
    // Set the BTpin for input.
    pinMode(BTpin, INPUT); 
 
    // Start serial communication with the serial monitor on the host computer, for testing purposes.
    Serial.begin(9600);
    //Serial.println("Arduino is ready");
    // wait until the HC-05 has made a connection
    while (!BTconnected)
    {
      if ( digitalRead(BTpin)==HIGH)  { 
        BTconnected = true;
      };
    }
 
    //Serial.println("HC-05 is now connected");
    //Serial.println("");
 
    // Start serial communication with the bluetooth module.
    // HC-06 default serial speed for communication mode is 9600.
    BTserial.begin(9600);

    // Separate function to set up the timer for clocked interrupts.
    setupTimer();
}


/**
 * @brief This function sets up the timer to generate an interrupt every 125 ms.
 * Timer 1 is used.
 */
void setupTimer(){
  // Stop interrupts.
  cli();
  
  // waarde compare match register =  [ 16,000,000Hz/ (prescaler * gewenste interrupt frequentie) ] - 1
  // set timer1 interrupt at 4Hz.
  
  // Set entire TCCR1A register to 0.
  TCCR1A = 0;
  
  // Same for TCCR1B.
  TCCR1B = 0;

  // Initialize counter value to 0.
  TCNT1  = 0;
  // set compare match register for 1hz increments.
  //OCR1A = 3905;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  OCR1A = 31250;
  
  // turn on CTC mode.
  TCCR1B |= (1 << WGM12);
  
  // Set CS10 and CS12 bits for 1024 prescaler.
  TCCR1B |= (1 << CS12) | (1 << CS10);
  
  // enable timer compare interrupt.
  TIMSK1 |= (1 << OCIE1A);

  // Enable interrupts again.
  sei();
}


/// This Arduino is executed continuously.
void loop()
{
 
    //! Reads from Arduino Serial Monitor input field and sends it to HC-06.
    if (Serial.available())
    {
        c =  Serial.read();
        BTserial.write(c); 
         
    }
}


/** 
 * An interrupt service routine to log data. This function is called every 125 ms. 
 * The functions sends the sensor readings to the bluetooth SoftwareSerial. 
*/
ISR(TIMER1_COMPA_vect){

  BTserial.print(millis());
  BTserial.println(": ");
  for(int i = 0; i< forceSensorCount; i++){
    forceSensorValues[i] = analogRead(forceSensors[i]);

    // milles() returns the number of milliseconds since the Arduino board began running the current program. 
    
    
    BTserial.println(forceSensorValues[i]);
  }
  // Print empty line to separate readings at different times
  BTserial.println("");
  
   //Serial.print("Raw Value = " ); // shows pre-scaled value 
   //Serial.print(RawValue); 
   //Serial.print("\t mV = "); // shows the voltage measured 
   //Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
   //Serial.print("\t Amps = "); // shows the voltage measured 
   //Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point
   //delay(500);
}
