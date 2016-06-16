/*
 * main.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include "sim.hh"

#include "matrix.hh"

#ifdef DEBUG
int debug_level = 1;
#else
int debug_level = 0;
#endif

using namespace std;

int usage( char *me )
{
	cout << "Usage: \n";
	cout << me << " <input file>\n";
	return 0;
}



#if 0
int main ( int argc, char *argv[])
{
	if( argc < 2 ) {
		return usage(argv[0]);
	}
	ifstream infile(argv[1]);
	if( infile.is_open() == false ) {
		cout << "Whoa!! Can't open file " << argv[1] <<
				", cowardly giving up...\n";
	}

	simulator leosim;
	if( leosim.setup(infile) < 0 ) {
		cout << "Whoa!! Can't setup simulator! cowardly giving up...\n";
	}

	leosim.run();

	return 0;
}
#endif

#if 1
int main ( int argc, char *argv[])
{
#ifdef SIMCPU_FEATURE_MATRIXACCEL
	ifstream infile("apps/matrix_accel.S");
#else
	ifstream infile("apps/matrix.S");
#endif
	if( infile.is_open() == false ) {
		cout << "Whoa!! Can't open file " << "apps/matrix.S" <<
				", cowardly giving up...\n";
	}

	simulator leosim;
	if( leosim.setup(infile) < 0 ) {
		cout << "Whoa!! Can't setup simulator! cowardly giving up...\n";
	}

	int size = atoi(argv[1]);

	matrix_main(leosim, size);

	return 0;
}
#endif
