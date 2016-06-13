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
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0])); i++) {
		this->register_bank[i] = 0;
		this->register_bank_old[i] = 0;
	}
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

std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m\e[41m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}

#if 0

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

#endif

void sim_processor::print_register_bank()
{
	cout << "Register Bank:" << endl;
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0])); i+=4) {
		for (int j = 0; j < 4; ++j) {
			if( register_bank_old[i+j] != register_bank[i+j] ) {
				cout << bold_on;
				cout << "    " << setw(2) << std::right << i+j << " : ";
				cout << "    " << setw(8) << std::left << register_bank[i+j];
				cout << bold_off;
			} else {
				cout << bold_off;
				cout << "    " << setw(2) << std::right << i+j << " : ";
				cout << "    " << setw(8) << std::left << register_bank[i+j];
			}
		}
		cout << endl;
	}
	for (int i = 0; i < (sizeof(register_bank)/sizeof(register_bank[0])); i++) {
		register_bank_old[i] = register_bank[i];
	}
}
