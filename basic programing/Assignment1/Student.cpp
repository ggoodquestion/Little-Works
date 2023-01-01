/*
 * Student.cpp
 *
 *  Created on: 2019年3月14日
 *      Author: 1MF08
 */

#include<iostream>
#include<string>
#include "Student.h"
using namespace std;

void Student::setFirstName(string name){
	firstName = name;
}

string Student::getFirstName() const{
	return firstName;
}

void Student::printFirstName() const{
	cout << "Student name is " << getFirstName() << "." << endl;
}





