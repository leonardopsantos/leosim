/*
 * sim_pipeline.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include "sim_pipeline.hh"
#include "sim_instruction.hh"
#include "sim_system.hh"
#include "sim_memory.hh"


sim_pipeline::sim_pipeline(sim_system *system)
{
	this->latency = 1;
	this->next_tick_fetch = 0;
	this->next_tick_decode = 0;
	this->next_tick_execute = 0;
	this->next_tick_mem = 0;
	this->next_tick_writeback = 0;
	this->system = system;
	this->cacheiL1If = &this->system->l1icache;
	this->cachedL1If = &this->system->l1dcache;
	this->cpu = &this->system->cpu;
	this->cpu_state = &this->system->cpu.state;
}

int sim_pipeline::next_tick(unsigned long int curr_tick) {
	return 0;
}

unsigned long int sim_pipeline::fetch(unsigned long int curr_tick, unsigned long int address,
		instruction** inst)
{
	*inst = this->cacheiL1If->get_content(address);
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::decode(unsigned long int curr_tick, instruction *inst)
{
	for(int i = 0; i < inst->num_sources; i++) {
		switch(inst->sourcesTypes[i]) {
		case instSources::REGISTER:
			inst->values[i] = this->cpu->register_read(inst->sources_idx[i]);
			break;
		case instSources::IMMEDIATE:
		case instSources::MEMORY:
			break;
		case instSources::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::execute(unsigned long int curr_tick) {
	return 0;
}

unsigned long int sim_pipeline::commit(unsigned long int curr_tick) {
	return 0;
}

int sim_pipeline::clock_tick(unsigned long int curr_tick) {

	unsigned long int ti = 0;

	instruction *fetchToDecode;
	instruction *decodeToExecute;
	instruction *executeToMemory;
	instruction *memoryToCommit;

	/* COMMIT */

	/* MEMORY */

	/* EXECUTE */

	/* DECODE */
	if( curr_tick == this->next_tick_decode ) {
		this->next_tick_decode = this->decode(curr_tick, fetchToDecode);
	}
	if( this->next_tick_fetch < ti )
		ti = this->next_tick_decode;

	/* FETCH */
	if( curr_tick == this->next_tick_fetch ) {
		this->next_tick_fetch = this->fetch(curr_tick,
				this->cpu_state->get_pc(), &fetchToDecode);
	}
	if( this->next_tick_fetch < ti )
		ti = this->next_tick_fetch;

	return ti;
}
