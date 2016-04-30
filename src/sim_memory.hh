/*
 * memory.hh
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#ifndef SRC_MEMORY_HH_
#define SRC_MEMORY_HH_

#include <map>
#include <fstream>

#include "sim_instruction.hh"

class memory {
public:
	memory(const memory&);
	memory(int lat);
	~memory();

	int clock_tick(void);
	int next_tick(void);
	instruction *get_content(unsigned long int address);
	void set_content(unsigned long int address, instruction *inst);

	int fill(std::ifstream &infile);

protected:
	int latency;
	std::map<unsigned long int, instruction*> content;
};

class cache:public memory {
public:
	cache(const cache&);
	cache(int lat, int ratio);
	int clock_tick(void);
	int next_tick(void);
	~cache();
private:
	int hit_ratio;
};

#endif /* SRC_MEMORY_HH_ */
