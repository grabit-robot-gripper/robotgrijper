#ifndef TmclCommand_H
#define TmclCommand_H

#include <Arduino.h>

/*
For more details, look into the TMCM-KR-842 hardware manual.
*/
class TmclCommand {
public:
    TmclCommand();
    ~TmclCommand();

    /*
    Command to stop the motor.
    */
    unsigned char *stop();

    /*
    parameter 'right':
    value 'true': gripper moves up, motor rotates counterclockwise.
    value 'false': gripper moves down, motor rotates clockwise.

    decimal: amount of microsteps the motor should rotate.
    1 microstep is 1/200 of 1.8°.
    */
    unsigned char *mvp(unsigned long int decimal, bool right);


    /* decimal: at what speed the motor should rotate */ 
    unsigned char *ror(unsigned long int decimal);
    unsigned char *rol(unsigned long int decimal);
};
#endif
