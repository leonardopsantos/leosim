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

int debug_level = 1;

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


int main ()
{
	ifstream infile("apps/matrix.S");
	if( infile.is_open() == false ) {
		cout << "Whoa!! Can't open file " << "apps/matrix.S" <<
				", cowardly giving up...\n";
	}

	simulator leosim;
	if( leosim.setup(infile) < 0 ) {
		cout << "Whoa!! Can't setup simulator! cowardly giving up...\n";
	}

	matrix_main(leosim);

	return 0;
}

