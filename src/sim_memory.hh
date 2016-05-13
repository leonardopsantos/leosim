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

	unsigned long int clock_tick(unsigned long int curr_tick);
	unsigned long int next_tick(unsigned long int curr_tick);
	instruction *get_content(unsigned long int address);
	void set_content(unsigned long int address, instruction *inst);

	int fill(std::ifstream &infile);

	static unsigned long int get_label_address(string label);

public:
	int latency;
	std::map<unsigned long int, instruction*> content;
	std::map<string, unsigned long int> labels;
};

class cache:public memory {
public:
	cache(const cache&);
	cache(int lat, int ratio);
	unsigned long int clock_tick(unsigned long int curr_tick);
	unsigned long int next_tick(unsigned long int curr_tick);
	~cache();
private:
	int hit_ratio;
};

#endif /* SRC_MEMORY_HH_ */
