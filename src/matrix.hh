/*
 * matrix.hh
 *
 *  Created on: 10 de jun de 2016
 *      Author: lsantos
 */

#ifndef SRC_MATRIX_HH_
#define SRC_MATRIX_HH_

#include <iostream>

#include "sim.hh"

class Matrix {

public:
	int rows;
	int columns;
	int *data;
	unsigned int size;
	Matrix();
	Matrix(int);
	Matrix(int, int);
	~Matrix();
	Matrix operator*(const Matrix& b);
	bool operator==(const Matrix& rhs);
	bool operator!=(const Matrix& rhs);
	friend std::ostream& operator<<(std::ostream& os, const Matrix& d);
};

void matrix_main(simulator &leosim, int size);

#endif /* SRC_MATRIX_HH_ */
