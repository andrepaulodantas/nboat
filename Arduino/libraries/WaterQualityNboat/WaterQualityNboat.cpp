/*
	WaterQualityNboat.cpp - Library for monitoring water quality into system
	Created by Andouglas Goncalves da Silva Junior, April 4, 2018
	Released into Nboat private folder
*/

#include "Arduino.h"
#include <Wire.h>
#include "WaterQualityNboat.h"

#define SENSOR_NOT_USED 0

WaterQualityNboat * WaterQualityNboat::wqNboat = 0;


static WaterQualityNboat* WaterQualityNboat::getInstance(){
	if(wqNboat == NULL){
		wqNboat = new WaterQualityNboat();
	}
	return wqNboat;
}


WaterQualityNboat::WaterQualityNboat(){
	
	Wire.begin();
	
}

WaterQualityNboat::WaterQualityNboat(int addr_ph, int addr_por, int addr_oxigen, int addr_temperature, int addr_condutivity)
{	

	_addr_ph = addr_ph;
	_addr_por = addr_por;
	_addr_oxigen = addr_oxigen;
	_addr_temperature = addr_temperature;
	_addr_condutivity = addr_condutivity;

	Wire.begin();
}

void WaterQualityNboat::startCommunication(int address)
{
	Wire.beginTransmission(address);
}

void WaterQualityNboat::closeCommunication()
{
	Wire.endTransmission();
}

void WaterQualityNboat::sendData(char *cmd){
	Wire.write(cmd);
}

void WaterQualityNboat::requestData(int address){
	Wire.requestFrom(address, 20, 1);
}

byte WaterQualityNboat::read(){
	return Wire.read();
}

bool WaterQualityNboat::dataAvailable(){
	return Wire.available();
}



char * WaterQualityNboat::getData(int sensorId, char * cmd){
	int address;
	_sensorId = sensorId;
	_cmd = cmd;

	byte in_char = 0;                //used as a 1 byte buffer to store in bound bytes from the pH Circuit.
	byte i = 0;                      //counter used for ph_data array.
	int time_ = 900;                 //used to change the delay needed depending on the command sent to the EZO Class pH Circuit.

	cmd[0] = tolower(cmd[0]);
	if(cmd[0] == 'c' || cmd[0] == 'r') time_ = 900;
	else time_ = 300;

	startCommunication(sensorId);
	sendData(_cmd);
	closeCommunication();

	if(strcmp(_cmd, "sleep") != 0){
		delay(time_);

		if(sensorId == _addr_por){
			address = _addr_por;
		}else if(sensorId == _addr_temperature){
			address = _addr_temperature;
		}else if(sensorId == _addr_oxigen){
			address = _addr_oxigen;
		}else if(sensorId == _addr_condutivity){
			address = _addr_condutivity;
		}else if(sensorId == _addr_ph){
			address = _addr_ph;
		}

		requestData(address);
		code = read();

		switch (code) {                  //switch case based on what the response code is.
	      case 1:                        //decimal 1.
	        Serial.println("Success");   //means the command was successful.
	        break;                       //exits the switch case.

	      case 2:                        //decimal 2.
	        Serial.println("Failed");    //means the command has failed.
	        break;                       //exits the switch case.

	      case 254:                      //decimal 254.
	        Serial.println("Pending");   //means the command has not yet been finished calculating.
	        break;                       //exits the switch case.

	      case 255:                      //decimal 255.
	        Serial.println("No Data");   //means there is no further data to send.
	        break;                       //exits the switch case.
	    }

	    while (dataAvailable()) {         //are there bytes to receive.
	      in_char = read();           //receive a byte.
	      _data[i] = in_char;            //load this byte into our array.
	      i += 1;                          //incur the counter for the array element.
	      if (in_char == 0) {              //if we see that we have been sent a null command.
	        i = 0;                         //reset the counter i to 0.
	        closeCommunication();        //end the I2C data transmission.
	        break;                         //exit the while loop.
	      }
	    }

	    //_dataFloat = atof(_data);
	    if(code == 1)
	    	return _data;

	    return NULL;
	}



}

int WaterQualityNboat::getNumberSensors(){
	return this->numberSensors;
}

void WaterQualityNboat::setNumberSensors(int _numberSensors){
	this->numberSensors = _numberSensors;
}

float WaterQualityNboat::getDataTemperature(){
	float v_out;
	float temp;
	int sensorPin = A1;
	v_out = analogRead(sensorPin);
	v_out*=.0048;
	v_out*=1000;
	temp= 0.0512 * v_out -20.5128;
	return temp;
}

String * WaterQualityNboat::getAllData(){
  String * data = new String[this->getNumberSensors()];
  for(int i=0; i<this->getNumberSensors();i++){
    data[i] = this->getInstance()->getData(sensors[i], "R");
    delay(1000);
  }
  return data;
}

String * WaterQualityNboat::i2cScanner(){
    byte error, address;
    int nDevices;
    int * sensorsId;
    String * listFoundSensors = new String[5];
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address);
      Serial.println("  !");
      char * result = this->getInstance()->getData(address, "i");
      listFoundSensors[nDevices] = result;
      nDevices++;      
      delay(1000);
      
      
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    

  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  
  listFoundSensors[nDevices+1] = "/0";
  return listFoundSensors;
  
}

void WaterQualityNboat::findAvailableSensors(){
   list = i2cScanner();
    int i = 0;
    while(list[i] != "\0"){
       
       String result = list[i];
       if(result[3] == 'D'){
          Serial.println(list[i]);
          sensors[i] = _addr_oxigen;
       }else if(result[3] == 'p'){
          sensors[i] = _addr_ph;
          Serial.println(list[i]);
       }else if(result[3] == 'E'){
          sensors[i] = _addr_condutivity;
          Serial.println(list[i]);
       }else if(result[3] == 'O'){
          sensors[i] = _addr_por;
          Serial.println(list[i]);
        }
       i++;
    }
    this->setNumberSensors(i);

    
}
