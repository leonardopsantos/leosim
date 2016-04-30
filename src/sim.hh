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

#include "sim_system.hh"

class simulator {
public:
	simulator(std::ifstream &input);
	void run();

	sim_system system;

private:
	int current_tick;
};

class clockable {
public:
	virtual int clock_tick(void) =0;
	virtual int next_tick(void) =0; /**< Gets the next tick */
	virtual ~clockable() {};

private:
};

#endif /* SRC_SIM_HH_ */
