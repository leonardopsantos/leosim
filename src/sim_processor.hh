/*
 * sim_processor.hh
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_PROCESSOR_HH_
#define SRC_SIM_PROCESSOR_HH_

#include <map>

#include "sim_pipeline.hh"
#include "sim_processor_state.hh"

class sim_system;

class sim_processor {
public:
	sim_processor(sim_system *system);
	int clock_tick(unsigned long int curr_tick);
	long int register_read(unsigned long int idx);
	void register_write(unsigned long int idx, long int value);
	void print_register_bank();

public:

	long int register_bank[32];
	long int register_bank_old[32];

public:
	sim_system *system;
	sim_processor_state state;
	sim_pipeline pipeline;
};

#endif /* SRC_SIM_PROCESSOR_HH_ */
