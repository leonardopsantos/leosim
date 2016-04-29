/*
 * sim.cc
 *
 *  Created on: 28 de abr de 2016
 *      Author: lsantos
 */

#include <iostream>
#include <fstream>

#include "sim.hh"
#include "sim_instruction.hh"

simulator::simulator(std::ifstream &input){

	this->current_tick = 0;


}

void simulator::run() {
}
