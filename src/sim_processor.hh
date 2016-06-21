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

enum {
	matrix_bank_FIRST = 32,
	matrix_bank_A = matrix_bank_FIRST,
	matrix_bank_DataA,
	matrix_bank_B,
	matrix_bank_DataB,
	matrix_bank_Res,
	matrix_bank_IncA,
	matrix_bank_IncB,
	matrix_bank_Count,
	matrix_bank_Status,
	matrix_bank_NUM
};

#define MATRIX_BANK_BIT_START		0x01
#define MATRIX_BANK_BIT_RUNNING		0x02
#define MATRIX_BANK_BIT_STOP		0x80

class sim_processor {
public:
	sim_processor(sim_system *system);
	int clock_tick(unsigned long int curr_tick);
	long int register_read(unsigned long int idx);
	void register_write(unsigned long int idx, long int value);
	void print_register_bank();

public:

	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	long long int register_bank[44];
	long long int register_bank_old[44];
	#else
	long int register_bank[32];
	long int register_bank_old[32];
	#endif

public:
	sim_system *system;
	sim_processor_state state;
	sim_pipeline pipeline;
};

#endif /* SRC_SIM_PROCESSOR_HH_ */
