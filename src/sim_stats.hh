/*
 * sim_stats.hh
 *
 *  Created on: 27 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_STATS_HH_
#define SRC_SIM_STATS_HH_

#include <ostream>

#include "sim_instruction.hh"

class sim_stats {
public:
	sim_stats();
	friend ostream& operator<<(ostream& os, const sim_stats& stats);

public:
	unsigned int ticks_total;
	unsigned int ticks_halted;
	unsigned int ticks_halted_jumps;
	unsigned int instructions_total;
	unsigned int instructions_by_type[static_cast< std::size_t >( instClasses::maxInstClasses )];
};

#endif /* SRC_SIM_STATS_HH_ */
