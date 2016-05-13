
#include <map>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

sim_system::sim_system():l1dcache(20, 950),
	l1icache(20, 950), cpu(this)
{
	this->current_tick = 0;
}

int sim_system::setup(std::ifstream &input)
{
	if( l1icache.fill(input) < 0 )
		return -1;
	return 0;
}

void sim_system::run() {
	l1dcache.clock_tick(this->current_tick);
	l1icache.clock_tick(this->current_tick);
	cpu.clock_tick(this->current_tick);
}
