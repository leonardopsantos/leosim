/*
 * sim.hh
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_HH_
#define SRC_SIM_HH_

#include <iostream>
#include <fstream>
#include <exception>

#include "sim_system.hh"

class simulator {
public:
	simulator();
	int setup(std::ifstream &input);
	void run();

	sim_system system;

private:
	int current_tick;
};

class exception_simulator_stop: public std::exception {
public:
	const char * what () const throw () {
	    return "STOP reached. Ending simulation.";
	}
};

//#define DEBUG

#endif /* SRC_SIM_HH_ */
