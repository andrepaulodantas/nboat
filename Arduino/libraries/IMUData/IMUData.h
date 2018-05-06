/*

  IMUData.cpp - Library for setting a position structure with latitude and longitude.
  Created by Davi H. dos Santos, March 28, 2018.
  BSD license, all text above must be included in any redistribution.

*/

#ifndef IMUData_h
#define IMUData_h

#include "Pose.h"

struct IMUData {
	Pose magnetometer;
	Pose gyroscope;
	Pose accelerometer;
};

#endif