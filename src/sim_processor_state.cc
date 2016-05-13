/*
 * sim_processor_state.cc
 *
 *  Created on: 13 de mai de 2016
 *      Author: lsantos
 */

#include "sim_processor_state.hh"

sim_processor_state::sim_processor_state()
{
	this->pc = 0;
}

sim_processor_state::sim_processor_state(unsigned long int pc)
{
	this->pc = pc;
}

unsigned long int sim_processor_state::get_pc() {
	return this->pc;
}

void sim_processor_state::set_pc(unsigned long int new_pc) {
	this->pc = new_pc;
}
