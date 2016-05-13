/*
 * sim.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include "sim.hh"

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
}
