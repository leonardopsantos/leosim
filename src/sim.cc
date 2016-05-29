/*
 * sim.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include "sim.hh"
#include "sim_stats.hh"

sim_stats simulator_stats;

simulator::simulator():system()
{
	this->current_tick = 0;
}

int simulator::setup(std::ifstream &input)
{
	if( system.setup(input) < 0 )
		return -1;

	return 0;
}

void simulator::run() {
	try {
		while(1)
			system.run();

	} catch (exception_simulator_stop &stp) {
//		cout << stp.what() << endl;
		cout << simulator_stats << endl;
	}
}

