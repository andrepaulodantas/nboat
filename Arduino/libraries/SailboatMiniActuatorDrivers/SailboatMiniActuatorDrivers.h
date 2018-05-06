/*3

  SailboatMiniActuatorDrivers.h - Library for the low level commands to put the Actuator in a desired position.
  Created by Davi H. dos Santos, March 31, 2018.
  BSD license, all text above must be included in any redistribution.

*/

#ifndef SailboatMiniActuatorDrivers_h
#define SailboatMiniActuatorDrivers_h

#include "Arduino.h"
#include <Servo.h>

class SailboatMiniActuatorDrivers
{
  public:
    SailboatMiniActuatorDrivers(int rudderPin, int sailPin);
    SailboatMiniActuatorDrivers();

    void setRudderAngle(float rudderAngle);
    void setSailAngle(float sailAngle);

    float getRudderAngle();
    float getSailAngle();    
  private:
    Servo rudder, sail;
    int _rudderPin = 8, _sailPin = 9;

    //TODO calibrate. lower limit = sailboat turns counterclockwise
    float _rudderLowerLimit = 5, _rudderUpperLimit = 75;

    //TODO calibrate. lower limit = sail parallel to sailboat
    float _sailLowerLimit = 6, _sailUpperLimit = 12;
};

#endif