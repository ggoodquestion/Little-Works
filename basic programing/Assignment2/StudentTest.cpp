/*
 * practice.cpp
 *
 *  Created on: 2019 3 21
 *      Author: 1MF08
 */
#include<iostream>
#include<string>
#include<iomanip>
#include<stdexcept>
#include "Student.h"
using namespace std;

int main(){
	/*Assignment1
	Student student;
	student.setFirstName("ggggggg");
	cout << "Get student firstName :" << student.getFirstName() << endl;
	student.printFirstName();
	 */

	//student.getGPA();
	Student student;
	student.setFirstName("Su");
	student.setLastName("J-J");
	student.printFirstName();
	student.printLastName();
	cout << "Test for student's first and last name: " << student.getFirstName() << " " << student.getLastName() << "." << endl;
	cout << "\nTest for student's GPA: " << student.getGPA() << setprecision(2) << "." << endl;
	student.printGPA();
	cout << "Changing student's factor of GPA...\n" << endl;

	student.computeGPA(1, 0, 0, 0, 5);

	cout << "Test for changed GPA: " << student.getGPA() << endl;
	student.printGPA();

	try{
		Student student1(-1.2);
	}catch (invalid_argument e) {
		cout << "Exception: " << e.what() << "\n" << endl;
	}

}
