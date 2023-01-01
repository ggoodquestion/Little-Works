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
#include "CEStudent.h"
using namespace std;

int main(){
	/*Assignment1
	Student student;
	student.setFirstName("ggggggg");
	cout << "Get student firstName :" << student.getFirstName() << endl;
	student.printFirstName();
	 */

	/*Assignment2
	student.getGPA();
	Student student;
	student.setFirstName("Su");
	student.setLastName("J-J");
	student.printFirstName();
	student.printLastName();
	cout << "Test for student's first and last name: " << student.getFirstName() << " " << student.getLastName() << "." << endl;
	cout << "\nTest for student's GPA: " << student.getGPA() << setprecision(2) << "." << endl;
	student.printGPA();
	cout << "Changing student's factor of GPA...\n" << endl;
	try{
		student.computeGPA(1, 0, 0, 0, 5);
	}catch (invalid_argument e) {
		cout << "Exception: " << e.what() << "\n" << endl;
	}
	cout << "Test for changed GPA: " << student.getGPA() << endl;
	student.printGPA();

	try{
		Student student1(-1.2);
	}catch (invalid_argument e) {
		cout << "Exception: " << e.what() << "\n" << endl;
	}
	 */

	/*Assignment3
	Student s1(2);
	Student s2(2);
	s1.setFirstName("Su");
	s1.setLastName("Gary");
	s2.setFirstName("Wang");
	s2.setLastName("Green");
	if(s1 < s2){
		cout << s2.getLastName() << " " << s2.getFirstName() << "'s GPA is higher." << endl;
	}else if (!(s2 < s1)){
		cout << "Same GPA!" << endl;
	}else{
		cout << s1.getLastName() << " " << s1.getFirstName() << "'s GPA is higher." << endl;
	}
	 */

	/*Assignment4
	CEStudent cestudent(6.5);
	cestudent.setCPE(true);
	cestudent.setFirstName("Su");
	cestudent.setLastName("Jia-G");
	cestudent.printCPE();
	 */

	Student student(4);
	student.setFirstName("Green");
	student.setLastName("Chen");
	//
	Student* studentPtr = &student;
	//
	CEStudent cestudent(6.5);
	cestudent.setCPE(true);
	cestudent.setFirstName("Su");
	cestudent.setLastName("Jia-G");
	//
	studentPtr -> print();
	delete studentPtr;
	//
	studentPtr = &cestudent;
	studentPtr -> print();
	delete studentPtr;
	cout << "\n\n";
	//

}