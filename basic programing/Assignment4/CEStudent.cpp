/*
 * CEStudent.cpp
 *
 *  Created on: 2019�~4��11��
 *      Author: 1MF08
 */
#include<iostream>
#include<string>
#include "Student.h"
#include "CEStudent.h"

using namespace std;

CEStudent::CEStudent(double GPA, bool CPE){
	if(GPA >= 0.0){
		this->GPA=GPA;
	}else{
		throw invalid_argument("GPA cannot be negative.");
	}
	this->CPE = CPE;
}

void CEStudent::setCPE(bool CPE){
	this->CPE = CPE;
}

bool CEStudent::getCPE() const{
	return CPE;
}

void CEStudent::printCPE() const{
	string hasPass = (getCPE())? "YES" : "NO";
	cout << "[CEStudent] Is CEStudent " << getFirstName() << " " << getLastName() <<"'s CPE has pass? " << hasPass << endl;
}
