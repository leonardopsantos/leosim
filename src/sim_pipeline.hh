/*
 * sim_pipeline.hh
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_PIPELINE_HH_
#define SRC_SIM_PIPELINE_HH_

#include "sim_clockable.hh"
#include "sim_processor_state.hh"
#include "sim_instruction.hh"

class sim_system;
class memory;
class sim_processor;

class sim_pipeline {
public:
	sim_pipeline(sim_system *system);
	int clock_tick(unsigned long int curr_tick);
	int next_tick(unsigned long int curr_tick); /**< Gets the next tick */

	unsigned long int fetch(unsigned long int curr_tick, unsigned long int address, instruction **inst);
	unsigned long int decode(unsigned long int curr_tick, instruction *inst);
	unsigned long int execute(unsigned long int curr_tick);
	unsigned long int commit(unsigned long int curr_tick);

public:
	sim_system *system;
	memory *cacheiL1If;
	memory *cachedL1If;
	sim_processor *cpu;
	sim_processor_state *cpu_state;

public:
	unsigned long int latency;

	unsigned long int next_tick_fetch;
	unsigned long int next_tick_decode;
	unsigned long int next_tick_execute;
	unsigned long int next_tick_mem;
	unsigned long int next_tick_writeback;

	instruction *fetchToDecode;
	instruction *decodeToExecute;
	instruction *executeToMemory;
	instruction *memoryToCommit;
};

#endif /* SRC_SIM_PIPELINE_HH_ */