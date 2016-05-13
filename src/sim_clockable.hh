/*
 * sim_clockable.hh
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_CLOCKABLE_HH_
#define SRC_SIM_CLOCKABLE_HH_

class clockable {
public:
	virtual int clock_tick(unsigned long int curr_tick) =0;
	virtual int next_tick(unsigned long int curr_tick) =0; /**< Gets the next tick */
	virtual ~clockable() {};

private:
};

#endif /* SRC_SIM_CLOCKABLE_HH_ */
