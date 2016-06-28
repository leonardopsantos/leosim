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
#include <iostream>

#include "sim_instruction.hh"

class memory {
public:
	memory(const memory&);
	memory(int lat);
	~memory();
	unsigned long int clock_tick(unsigned long int curr_tick);
	unsigned long int next_tick(unsigned long int curr_tick);

public:
	int latency;
};

class cache_generic:public memory {
public:
	cache_generic(const cache_generic&);
	cache_generic(int lat, int ratio);
	~cache_generic();

public:
	int hit_ratio;
};

class cache_instructions: public cache_generic {
public:
	cache_instructions(int lat, int ratio);
	~cache_instructions();
	instruction *get_content(unsigned long int address);
	void set_content(unsigned long int address, instruction *inst);
	virtual int fill(std::ifstream &infile);
	unsigned long int get_address_by_label(string label);
	string get_label_by_address(unsigned long int addr) const;
	friend ostream& operator<<(ostream& os, const cache_instructions& cache);

public:
	std::map<unsigned long int, instruction*> content;
	std::map<string, unsigned long int> labels;
};

class cache_data: public cache_generic {
public:
	cache_data(int lat, int ratio);
	~cache_data();
	long int get_content(unsigned long int address);
	void set_content(unsigned long int address, long int data);
	virtual int fill(std::ifstream &infile);
	virtual int fill(int *data, int size);
	virtual int dump(int *data, int size);

public:
	std::map<unsigned long int, long int> content;
//	std::map<string, unsigned long int> labels;
};

#endif /* SRC_MEMORY_HH_ */
