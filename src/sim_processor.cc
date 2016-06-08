/*
 * sim_processor.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include <ostream>
#include <iomanip>

#include "sim_processor.hh"
#include "sim_pipeline.hh"

using namespace std;

sim_processor::sim_processor(sim_system *system):pipeline(system)
{
	this->system = system;
	memset(this->register_bank, '\0', sizeof(register_bank));
}

int sim_processor::clock_tick(unsigned long int curr_tick)
{
	unsigned long int next_tick;
	next_tick = this->pipeline.clock_tick(curr_tick);
	return next_tick;
}

long int sim_processor::register_read(unsigned long int idx)
{
	if( idx >= (sizeof(register_bank)/sizeof(register_bank[0])) )
		throw "Register index out of bounds!";

	return this->register_bank[idx];
}

void sim_processor::register_write(unsigned long int idx, long int value)
{
	if( idx >= (sizeof(register_bank)/sizeof(register_bank[0])) )
		throw "Register index out of bounds!";

	this->register_bank[idx] = value;
}

void sim_processor::print_register_bank()
{
	cout << "Register Bank: ";
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0]))/2; ++i) {
		cout << setw(4) << i;
	}
	cout << endl;
	cout << "               ";
	for (int i = (sizeof(register_bank)/sizeof(register_bank[0]))/2;
			i < (sizeof(register_bank)/sizeof(register_bank[0])); ++i) {
		cout << setw(4) << i;
	}
	cout << endl << endl;

	cout << "          00   ";
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0]))/2; ++i) {
		cout << setw(4) << register_bank[i];
	}
	cout << endl;




	cout << "          16   ";
	for (int i = (sizeof(register_bank)/sizeof(register_bank[0]))/2;
			i < (sizeof(register_bank)/sizeof(register_bank[0])); ++i) {
		cout << setw(4) << register_bank[i];
	}
	cout << endl;

	/*
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0])); ++i) {
		cout << setw(2) <<

	}*/
}
