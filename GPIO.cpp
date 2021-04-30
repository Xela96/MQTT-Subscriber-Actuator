/*
 * GPIO.cpp  Created on: 29 Apr 2015
 * Copyright (c) 2015 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring Raspberry Pi"
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European
 * Union Public License (the "EUPL") version 1.1 as published by the
 * European Union. Any use of this Software, other than as authorized
 * under this License is strictly prohibited (to the extent such use
 * is covered by a right of the copyright holder of this Software).
 *
 * This Software is provided under the License on an "AS IS" basis and
 * without warranties of any kind concerning the Software, including
 * without limitation merchantability, fitness for a particular purpose,
 * absence of defects or errors, accuracy, and non-infringement of
 * intellectual property rights other than copyright. This disclaimer
 * of warranty is an essential part of the License and a condition for
 * the grant of any rights to this Software.
 *
 * For more details, see http://www.derekmolloy.ie/
 */

#include "GPIO.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<cstdio>
#include<fcntl.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<pthread.h>
using namespace std;


/**
 *
 * @param number The GPIO number for the RPi
 */
GPIO::GPIO(int number) {
	this->number = number;
	this->debounceTime = 0;
	this->togglePeriod=100;
	this->toggleNumber=-1; //infinite number
	this->callbackFunction = NULL;
	this->threadRunning = false;

	ostringstream s;
	s << "gpio" << number;
	this->name = string(s.str());
	this->path = GPIO_PATH + this->name + "/";
	this->exportGPIO();
	// need to give Linux time to set up the sysfs structure
	usleep(250000); // 250ms delay
}

int GPIO::write(string path, string filename, string value){
   ofstream fs;
   fs.open((path + filename).c_str());
   if (!fs.is_open()){
	   perror("GPIO: write failed to open file ");
	   return -1;
   }
   fs << value;
   fs.close();
   return 0;
}

string GPIO::read(string path, string filename){
   ifstream fs;
   fs.open((path + filename).c_str());
   if (!fs.is_open()){
	   perror("GPIO: read failed to open file ");
    }
   string input;
   getline(fs,input);
   fs.close();
   return input;
}

int GPIO::write(string path, string filename, int value){
   stringstream s;
   s << value;
   return this->write(path,filename,s.str());
}

int GPIO::exportGPIO(){
   return this->write(GPIO_PATH, "export", this->number);
}

int GPIO::unexportGPIO(){
   return this->write(GPIO_PATH, "unexport", this->number);
}

int GPIO::setDirection(GPIO_DIRECTION dir){
   switch(dir){
   case INPUT: return this->write(this->path, "direction", "in");
      break;
   case OUTPUT:return this->write(this->path, "direction", "out");
      break;
   }
   return -1;
}

int GPIO::setValue(GPIO_VALUE value){
   switch(value){
   case HIGH: return this->write(this->path, "value", "1");
      break;
   case LOW: return this->write(this->path, "value", "0");
      break;
   }
   return -1;
}

int GPIO::setActiveLow(bool isLow){
   if(isLow) return this->write(this->path, "active_low", "1");
   else return this->write(this->path, "active_low", "0");
}

int GPIO::setActiveHigh(){
   return this->setActiveLow(false);
}

GPIO_VALUE GPIO::getValue(){
	string input = this->read(this->path, "value");
	if (input == "0") return LOW;
	else return HIGH;
}

GPIO_DIRECTION GPIO::getDirection(){
	string input = this->read(this->path, "direction");
	if (input == "in") return INPUT;
	else return OUTPUT;
}

GPIO::~GPIO() {
	this->unexportGPIO();
}
