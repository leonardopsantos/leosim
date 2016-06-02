/*
 * sim_processor_state.hh
 *
 *  Created on: 13 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_PROCESSOR_STATE_HH_
#define SRC_SIM_PROCESSOR_STATE_HH_

class sim_processor_state {
public:
	sim_processor_state();
	sim_processor_state(unsigned long int pc);
	unsigned long int get_pc();
	void set_pc(unsigned long int new_pc);
	void set_target_pc(unsigned long int new_pc);
	void branch_clear();
	void update_pc();

public:
	unsigned long int pc;
	unsigned long int target_pc;
	bool branch;
};

#endif /* SRC_SIM_PROCESSOR_STATE_HH_ */
