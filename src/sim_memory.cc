/*
 * memory.cc
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#include <map>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

memory::memory() {}

memory::memory(const memory& o) {
	this->latency = o.latency;
}

memory::memory(int lat) {
	this->latency = lat;
}

memory::~memory() {}

int memory::clock_tick(void) {
	return 0;
}

int memory::next_tick(void) {
	return 0;
}

cache::cache() {}

cache::cache(const cache& o):memory(o.latency) {
	this->hit_ratio = o.hit_ratio;
}

cache::cache(int lat, int ratio){
	this->latency = lat;
	this->hit_ratio = ratio;
}

cache::~cache() {}

int cache::clock_tick(void) {
	return 0;
}

int cache::next_tick(void) {
	return 0;
}

instruction* memory::get_content(unsigned long int address) {
	return this->content[address];
}

void memory::set_content(unsigned long int address, instruction* inst) {
	this->content[address] = inst;
}
