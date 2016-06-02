/*
 * sim_pipeline.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include <ostream>

#include "sim_pipeline.hh"
#include "sim_instruction.hh"
#include "sim_system.hh"
#include "sim_memory.hh"
#include "sim_stats.hh"

extern sim_stats simulator_stats;

instructionNOP staticNOP;

using namespace std;

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
	this->lastCommit = &staticNOP;
}

ostream& operator<<(ostream& os, const sim_pipeline& pipe)
{
	os << "Fetch :  " << *pipe.fetchToDecode << endl;
	os << "Decode:  " << *pipe.decodeToExecute << endl;
	os << "Execute: " << *pipe.executeToMemory << endl;
	os << "Memory:  " << *pipe.memoryToCommit << endl;
	os << "Commit:  " << *pipe.lastCommit << endl;
	return os;
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
	if( inst->destsTypes[0] == instDest::BRANCH ) {
		unsigned long int pc_target;

		if( inst->sourcesTypes[0] == instSources::IMMEDIATE )
			pc_target = this->cacheiL1If->get_label_address(inst->tag);
		else if( inst->sourcesTypes[0] == instSources::REGISTER )
			pc_target = inst->sources_values[0];

		this->cpu_state->set_target_pc(pc_target);
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
	inst->commit();
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

int sim_pipeline::clock_tick(unsigned long int curr_tick)
{

	unsigned long int curr_pc = this->cpu_state->get_pc();
	bool halt_pipeline = false;

	if( this->fetchToDecode->depends(this->decodeToExecute) == true ||
		this->fetchToDecode->depends(this->executeToMemory) == true ||
		this->fetchToDecode->depends(this->memoryToCommit) == true )
		halt_pipeline = true;

	/* COMMIT */
	cout << "Commit:  " << *this->memoryToCommit << endl;
	this->commit(curr_tick, this->memoryToCommit);
	this->memoryToCommit->update_stats();
	this->lastCommit = this->memoryToCommit;

	/* MEMORY */
	cout << "Memory:  " << *this->executeToMemory << endl;
	this->memory(curr_tick, this->executeToMemory);
	this->memoryToCommit = this->executeToMemory;

	/* EXECUTE */
	cout << "Execute: " << *this->decodeToExecute << endl;
	this->execute(curr_tick, this->decodeToExecute);
	this->executeToMemory = this->decodeToExecute;

	if( halt_pipeline == false ) {
		/* DECODE */
		cout << "Decode:  " << *this->fetchToDecode << endl;

		this->decode(curr_tick, this->fetchToDecode);
		this->decodeToExecute = this->fetchToDecode;

		/* FETCH */
		instruction *f;
		if( this->cpu_state->branch == false ) {
			this->next_tick_fetch = this->fetch(curr_tick,
					curr_pc, &f);
		} else {
			f = NULL;
		}

		this->fetchToDecode = (f == NULL ? &staticNOP : f);

		cout << "Fetch:   " << *this->fetchToDecode << endl;

		this->cpu_state->update_pc();

		simulator_stats.ticks_halted++;
	} else {
		cout << "Decode:  " << *this->fetchToDecode << " (held)" << endl;
		cout << "Fetch:   " << *this->fetchToDecode << " (held)" << endl;
		this->decodeToExecute = &staticNOP;
	}

	simulator_stats.ticks_total++;

	return curr_tick + this->latency;
}
