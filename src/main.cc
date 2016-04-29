/*
 * main.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include <sim.hh>

using namespace std;

int usage( char *me )
{
	cout << "Usage: \n";
	cout << me << " <input file>\n";
	return 0;
}

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

	simulator leosim(infile);

	leosim.run();

	return 0;
}
