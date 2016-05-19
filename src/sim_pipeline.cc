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


instructionNOP staticNOP;

sim_pipeline::sim_pipeline(sim_system *system)
{
	this->latency = 1;
	this->next_tick_fetch = 0;
	this->next_tick_decode = 0;
	this->next_tick_execute = 0;
	this->next_tick_mem = 0;
	this->next_tick_commit = 0;
	this->system = system;
	this->cacheiL1If = &this->system->l1icache;
	this->cachedL1If = &this->system->l1dcache;
	this->cpu = &this->system->cpu;
	this->cpu_state = &this->system->cpu.state;
	this->fetchToDecode = &staticNOP;
	this->decodeToExecute = &staticNOP;
	this->executeToMemory = &staticNOP;
	this->memoryToCommit = &staticNOP;
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
			inst->sources_values[i] = this->cpu->register_read(inst->sources_idx[i]);
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

unsigned long int sim_pipeline::execute(unsigned long int curr_tick, instruction *inst)
{
	inst->execute();
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::memory(unsigned long int curr_tick, instruction* inst)
{
	for(int i = 0; i < inst->num_sources; i++) {
		switch(inst->sourcesTypes[i]) {
		case instSources::MEMORY:
			inst->sources_values[i] = this->system->l1dcache.get_content(inst->sources_idx[i]);
			break;
		case instSources::REGISTER:
		case instSources::IMMEDIATE:
			break;
		case instSources::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	for(int i = 0; i < inst->num_dests; i++) {
		switch(inst->destsTypes[i]) {
		case instDest::MEMORY:
			this->system->l1dcache.set_content(inst->sources_idx[i], inst->sources_values[i]);
			break;
		case instDest::REGISTER:
			break;
		case instDest::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::commit(unsigned long int curr_tick, instruction *inst)
{
	for(int i = 0; i < inst->num_dests; i++) {
		switch(inst->destsTypes[i]) {
		case instDest::REGISTER:
			this->cpu->register_write(inst->dests_idx[i], inst->destination_values[i]);
			break;
		case instDest::MEMORY:
			break;
		case instDest::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	return curr_tick+this->latency;
}

int sim_pipeline::clock_tick(unsigned long int curr_tick) {

	unsigned long int ti = 0;

	/* COMMIT */
	if( curr_tick == this->next_tick_commit ) {
		this->next_tick_commit = this->commit(curr_tick, this->memoryToCommit);
	}

	/* MEMORY */
	if( curr_tick == this->next_tick_mem ) {
		this->next_tick_mem = this->memory(curr_tick, this->executeToMemory);
	}
	this->memoryToCommit = this->executeToMemory;

	/* EXECUTE */
	if( curr_tick == this->next_tick_execute ) {
		this->next_tick_decode = this->execute(curr_tick, this->decodeToExecute);
	}
	this->executeToMemory = this->decodeToExecute;

	/* DECODE */
	if( curr_tick == this->next_tick_decode ) {
		this->next_tick_decode = this->decode(curr_tick, this->fetchToDecode);
	}
	this->decodeToExecute = this->fetchToDecode;

	/* FETCH */
	if( curr_tick == this->next_tick_fetch ) {
		this->next_tick_fetch = this->fetch(curr_tick,
				this->cpu_state->get_pc(), &this->fetchToDecode);
	}

	/* check nearest clock tick */
	ti = this->next_tick_commit;
	if( this->next_tick_mem < ti )
		ti = this->next_tick_mem;
	if( this->next_tick_execute < ti )
		ti = this->next_tick_execute;
	if( this->next_tick_decode < ti )
		ti = this->next_tick_decode;
	if( this->next_tick_fetch < ti )
		ti = this->next_tick_fetch;

	return ti;
}
