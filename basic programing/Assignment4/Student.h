/*
 * Student.h
 *
 *  Created on: 2019 3 21
 *      Author: 1MF08
 */
#include<iostream>
#include<string>
#ifndef STUDENT_H_
#define STUDENT_H_

class Student{
public:
	explicit Student(double = 0.0);
	~Student();

	void setFirstName(std::string name);
	std::string getFirstName() const;
	void printFirstName() const;
	//
	void setLastName(std::string name);
	std::string getLastName() const;
	void printLastName() const;
	void computeGPA(int numberOfAs, int numberOfBs,
			int numberOfCs, int numberOfDs, int numberOfFs);
	double getGPA() const;
	void printGPA() const;
	bool operator<(const Student);
protected:
	double GPA;

private:
	std::string firstName;
	std::string lastName;

};



#endif /* STUDENT_H_ */
