/*
 * sim.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include "sim.hh"

simulator::simulator(std::ifstream &input):system(input) {
	this->current_tick = 0;
}

void simulator::run() {
}
