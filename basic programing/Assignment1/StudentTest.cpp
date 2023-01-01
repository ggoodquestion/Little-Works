/*
 * practice.cpp
 *
 *  Created on: 2019年3月14日
 *      Author: 1MF08
 */
#include<iostream>
#include<string>
#include "Student.h"
using namespace std;

int main(){
	Student student;
	student.setFirstName("ggggggg");
	cout << "Get student firstName :" << student.getFirstName() << endl;
	student.printFirstName();
}
