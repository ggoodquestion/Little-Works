/*
 * Student.cpp
 *
 *  Created on: 2019 3 21
 *      Author: 1MF08
 */

#include<iostream>
#include<string>
#include<stdexcept>
#include "Student.h"
#include<iomanip>
using namespace std;

Student::Student(double GPA){
	if(GPA >= 0.0){
		this->GPA = GPA;
	}else{
		throw invalid_argument("GPA cannot be negative.");
	}
}

Student::~Student(){
	cout << "Destroying Student" << endl;
}

void Student::setFirstName(string name){
	firstName = name;
}

string Student::getFirstName() const{
	return firstName;
}

void Student::printFirstName() const{
	cout << "Student's first name is " << getFirstName() << "." << endl;
}

void Student::setLastName(string name){
	lastName = name;
}

string Student::getLastName() const{
	return lastName;
}

void Student::printLastName() const{
	cout << "Student's last name is " << getLastName() << "." << endl;
}

void Student::computeGPA(int numberOfAs, int numberOfBs, int numberOfCs, int numberOfDs, int numberOfFs){
	double gpa;
	gpa = (4.0*numberOfAs + 3.0*numberOfBs + 2.0*numberOfCs + 1.0*numberOfDs) / (numberOfAs + numberOfBs + numberOfCs + numberOfDs + numberOfFs);
	if(gpa >= 0.0){
		GPA = gpa;
	}else{
		throw invalid_argument("GPA cannot be negative.");
	}
}

double Student::getGPA() const{
	return GPA;
}

void Student::printGPA() const{
	cout << "Student's GPA is " << getGPA() << ".\n" << endl;
}

bool Student::operator <(const Student s2){
	Student s1 = *this;
	if(s1.getGPA() < s2.getGPA()){
		return true;
	}else{
		return false;
	}
}

void Student::print() const{
	cout << getFirstName() << " " << getLastName() << ", GPA: " << getGPA() << endl;
}
