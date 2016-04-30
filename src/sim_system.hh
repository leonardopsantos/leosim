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

class sim_system {
public:
	cache l1dcache, l1icache;

	sim_system(std::ifstream &input);
private:
};



#endif /* SRC_SYSTEM_HH_ */
