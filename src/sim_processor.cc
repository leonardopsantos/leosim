/*
 * sim_processor.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include "sim_processor.hh"
#include "sim_pipeline.hh"

sim_processor::sim_processor(sim_system *system):pipeline(system)
{
	this->system = system;
}

int sim_processor::clock_tick(unsigned long int curr_tick)
{
	unsigned long int next_tick;
	next_tick = this->pipeline.clock_tick(curr_tick);
	this->state.pc += sizeof(unsigned long int);
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
