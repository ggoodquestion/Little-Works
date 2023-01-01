/*
 * Student.h
 *
 *  Created on: 2019年3月14日
 *      Author: 1MF08
 */
#include<iostream>
#include<string>
#ifndef STUDENT_H_
#define STUDENT_H_

class Student{
public:
	void setFirstName(std::string name);

	std::string getFirstName() const;

	void printFirstName() const;
private:
	std::string firstName;
};



#endif /* STUDENT_H_ */
