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
	Matrix();
	Matrix(int);
	Matrix(int, int);
	Matrix operator*(const Matrix& b);
	friend std::ostream& operator<<(std::ostream& os, const Matrix& d);
};

void matrix_main(simulator &leosim);

#endif /* SRC_MATRIX_HH_ */
