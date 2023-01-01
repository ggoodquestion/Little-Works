/*
 * CEStudent.h
 *
 *  Created on: 2019¦~4¤ë11¤é
 *      Author: 1MF08
 */

#ifndef CESTUDENT_H_
#define CESTUDENT_H_

#include "Student.h"

class CEStudent : public Student{
public:
	explicit CEStudent(double = 0, bool = false);
	void setCPE(bool);
	bool getCPE() const;
	void printCPE() const;
private:
	bool CPE;
};



#endif /* CESTUDENT_H_ */
