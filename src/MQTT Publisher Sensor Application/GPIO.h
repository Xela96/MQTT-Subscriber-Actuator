/*
 * GPIO.h  Created on: 29 Apr 2015
 * Copyright (c) 2015 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring Raspberry Pi"
 * See: www.exploringrpi.com
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

#ifndef GPIO_H_
#define GPIO_H_
#include<string>
#include<fstream>
#include<sys/epoll.h>
#include<pthread.h>
using std::string;
using std::ofstream;

#define GPIO_PATH "/sys/class/gpio/"

typedef int (*CallbackType)(int);
enum GPIO_DIRECTION{ INPUT, OUTPUT };
enum GPIO_VALUE{ LOW=0, HIGH=1 };
enum GPIO_EDGE{ NONE, RISING, FALLING, BOTH };

class GPIO {
private:
	int number, debounceTime;
	string name, path;
public:
	GPIO(int number);                     // constructor exports pin
	virtual int getNumber() { return number; }

	// General Input and Output Settings
	virtual int  setDirection(GPIO_DIRECTION);
	virtual GPIO_DIRECTION getDirection();
	virtual int  setValue(GPIO_VALUE);
	virtual GPIO_VALUE getValue();
	virtual int  setActiveLow(bool isLow=true);  // low=1, high=0
	virtual int  setActiveHigh();                // default state
	virtual void setDebounceTime(int time) { this->debounceTime = time; }

	virtual ~GPIO();  // destructor unexports the pin
private:
	int write(string path, string filename, string value);
	int write(string path, string filename, int value);
	string read(string path, string filename);
	int exportGPIO();
	int unexportGPIO();
	ofstream stream;
	pthread_t thread;
	CallbackType callbackFunction;
	bool threadRunning;
	int togglePeriod;  // default 100ms
	int toggleNumber;  // default -1 (infinite)
};

#endif /* GPIO_H_ */
