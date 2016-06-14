#include <iostream>
#include <stdlib.h>     /* atoi */
#include <string.h>     /* atoi */
#include <stdio.h>


#include "sim.hh"
#include "matrix.hh"

extern int debug_level;

using namespace std;

Matrix::Matrix() {
	this->rows = 0;
	this->columns = 0;
	this->data = NULL;
	this->size = 0;
}

Matrix::Matrix( int size )
{
	this->rows = size;
	this->columns = size;
	this->size = size*size;
	this->data = new int[this->size];
        int *p = this->data;
        for(int i = 0; i < this->size; i++)
            *p++ = rand();
}

Matrix::Matrix( int r, int c ):Matrix()
{
	this->rows = r;
	this->columns = c;
	this->size = this->rows*this->columns;
	this->data = new int[this->size];
}

Matrix::~Matrix()
{
	if( this->size > 0 && this->data != NULL ) {
		delete this->data;
		this->data = NULL;
		this->size = 0;
	}
}

#if 0
Matrix Matrix::operator *(const Matrix& b)
{
	Matrix res(this->rows, b.columns);

        if( this->columns != b.rows )
		return res;

        memset(res.data, '\0', res.rows*res.columns*sizeof(res.data[0]));

	int rs, cs, i;
	int *Pres, *Pa, *Pb;

	Pres = res.data;

	for(rs = 0; rs < this->rows; rs++) {
		for (cs = 0; cs < b.columns; cs++) {
			*Pres = 0;
			Pa = this->data+rs*this->columns;
			Pb = b.data+cs;
			for (i = 0; i < this->columns; i++) {
				*Pres += *Pa++*(*Pb);
				Pb += b.columns;
			}
			Pres++;
		}
	}
	return res;
}
#else

Matrix Matrix::operator *(const Matrix& b)
{
	Matrix res(this->rows, b.columns);

        if( this->columns != b.rows )
		return res;

        memset(res.data, '\0', res.rows*res.columns*sizeof(res.data[0]));

	int i, j, k, k4;
	int *Pres[4], *Pa, *Pb[4];

	k4 = b.columns/4;
	Pa = this->data;

	for(i = 0; i < this->rows; i++) {
		for (j = 0; j < this->columns; j++) {
			Pres[0] = res.data + i*b.columns;
			Pb[0] = b.data + j*b.columns;
			Pres[1] = Pres[0]+1;
			Pres[2] = Pres[0]+2;
			Pres[3] = Pres[0]+3;
			Pb[1] = Pb[0] + 1;
			Pb[2] = Pb[0] + 2;
			Pb[3] = Pb[0] + 3;
			for (k = 0; k < k4; k++) {
				*Pres[0] += *Pa*(*Pb[0]);
				Pb[0] += 4;
				Pres[0] += 4;
				*Pres[1] += *Pa*(*Pb[1]);
				Pb[1] += 4;
				Pres[1] += 4;
				*Pres[2] += *Pa*(*Pb[2]);
				Pb[2] += 4;
				Pres[2] += 4;
				*Pres[3] += *Pa*(*Pb[3]);
				Pb[3] += 4;
				Pres[3] += 4;
			}
			if( (b.columns%4) > 0 )
				*Pres[0] += *Pa*(*Pb[0]);
			if( (b.columns%4) > 1 )
				*Pres[1] += *Pa*(*Pb[1]);
			if( (b.columns%4) > 2 )
				*Pres[2] += *Pa*(*Pb[2]);
			Pa++;
		}
	}
	return res;
}
#endif

bool Matrix::operator==(const Matrix& rhs)
{
	if( this->columns != rhs.columns || this->rows != rhs.rows )
		return false;

	int *P_lhs = this->data;
	int *P_rhs = rhs.data;
	int sz = rhs.rows*rhs.columns;
	for (int i = 0; i < sz; ++i) {
		if(*P_lhs++ != *P_rhs++ )
			return false;
	}

	return true;
}

bool Matrix::operator !=(const Matrix& rhs)
{
	return !(*this == rhs);
}

ostream& operator<<(ostream& os, const Matrix& d)
{
	int rs, cs;
	int *P = d.data;
	for(rs = 0; rs < d.rows; rs++) {
		for (cs = 0; cs < d.columns; cs++) {
			os << *P++ << ", ";
		}
		os << "\n";
	}
    return os;
}

void matrix_main(simulator &leosim)
{
#ifdef DEBUG
	Matrix Ma(2,2);
	Matrix Mb(2,2);

	int da[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 16};

	memcpy(Ma.data, da, Ma.columns*Ma.rows*sizeof(int));
	memcpy(Mb.data, da, Mb.columns*Mb.rows*sizeof(int));
#else
	Matrix Ma(100,100);
	Matrix Mb(100,100);
#endif
	Matrix Mcheck = Ma*Mb;

	if( debug_level > 0 ) {
		cout << "Matriz A:\n" << Ma;
		cout << "Matriz B:\n" << Mb;
	}

	//	r0 = ponteiro para matriz A
	//	r1 = ponteiro para matriz B
	//	r30 = ponteiro para matriz resultante
	//	r2 = # de linhas A
	//	r3 = # de colunas B
	//	r4 = # de colunas A

	Matrix Mresult(Ma.rows,Mb.columns);

	leosim.system.cpu.register_bank[0] = (unsigned long int) Ma.data;
	leosim.system.cpu.register_bank[1] = (unsigned long int) Mb.data;
	leosim.system.cpu.register_bank[30] = (unsigned long int) Mresult.data;
	leosim.system.cpu.register_bank[2] = Ma.rows;
	leosim.system.cpu.register_bank[3] = Mb.columns;
	leosim.system.cpu.register_bank[4] = Ma.columns;

	leosim.system.l1dcache.fill(Ma.data, Ma.rows*Ma.columns);
	leosim.system.l1dcache.fill(Mb.data, Mb.rows*Mb.columns);

	memset(Mresult.data, '\0', Mresult.rows*Mresult.columns);

	leosim.run();

	leosim.system.l1dcache.dump(Mresult.data, Mresult.rows*Mresult.columns);

	if( debug_level > 0 ) {
		cout << "Matriz Mresult:\n" << Mresult;
		cout << "Matriz Mcheck:\n" << Mcheck;

	}
	if( Mresult == Mcheck )
		cout << "MATCHES!! :-D" << endl;
	else
		cout << "DOES NOT MATCH!! :-(" << endl;

}
