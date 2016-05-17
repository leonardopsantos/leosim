/*
 * system.hh
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#ifndef SRC_SYSTEM_HH_
#define SRC_SYSTEM_HH_

#include <iostream>
#include <fstream>

#include "sim_memory.hh"
#include "sim_processor.hh"

class sim_system {
public:
	cache_data l1dcache;
	cache_instructions l1icache;
	sim_processor cpu;

	sim_system();
	int setup(std::ifstream &input);

	void run();

public:
	unsigned long int current_tick;
};



#endif /* SRC_SYSTEM_HH_ */
