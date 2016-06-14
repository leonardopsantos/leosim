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
	this->target_pc = 0;
	this->branch = false;
}

sim_processor_state::sim_processor_state(unsigned long int pc)
{
	this->pc = pc;
	this->target_pc = 0;
	this->branch = false;
}

unsigned long int sim_processor_state::get_pc() {
	return this->pc;
}

void sim_processor_state::set_pc(unsigned long int new_pc) {
	this->pc = new_pc;
}

void sim_processor_state::set_target_pc(unsigned long int new_pc)
{
	this->target_pc = new_pc;
	this->branch = true;
}

void sim_processor_state::branch_set()
{
	this->branch = true;
}

void sim_processor_state::branch_clear()
{
	this->branch = false;
}

void sim_processor_state::update_pc()
{
	if( branch == true )
		this->pc = this->target_pc;
	else
		this->pc += 4;

	branch = false;
}
