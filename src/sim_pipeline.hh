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
#include "sim_memory.hh"

#include "sim_features.hh"

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
	unsigned long int execute(unsigned long int curr_tick, instruction *inst);
	unsigned long int memory(unsigned long int curr_tick, instruction *inst);
	unsigned long int commit(unsigned long int curr_tick, instruction *inst);

	friend ostream& operator<<(ostream& os, const sim_pipeline& pipe);

	void forward_data(instruction *insta, instruction *instb);

	void set_pc_jump(instruction *inst);
	void forward_branch(instruction *inst);
public:
	sim_system *system;
	cache_instructions *cacheiL1If;
	cache_data *cachedL1If;
	sim_processor *cpu;
	sim_processor_state *cpu_state;

public:
	unsigned long int latency;

	unsigned long int next_tick_fetch;
	unsigned long int next_tick_decode;
	unsigned long int next_tick_execute;
	unsigned long int next_tick_mem;
	unsigned long int next_tick_commit;

	instruction *fetchToDecode;
	instruction *decodeToExecute;
	instruction *executeToMemory;
	instruction *memoryToCommit;
	instruction *lastCommit;
};

#endif /* SRC_SIM_PIPELINE_HH_ */
