
#include <map>
#include <ostream>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

extern int debug_level;

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
	unsigned long int next_tick;
	l1dcache.clock_tick(this->current_tick);
	l1icache.clock_tick(this->current_tick);

	if( debug_level > 0 ) {
		cout << "Current tick: " << this->current_tick;
		cout << ", current PC: " << this->cpu.state.pc << endl;
		cout << "Pipeline status: " << endl;
	}
	next_tick = cpu.clock_tick(this->current_tick);
	if( debug_level > 0 ) {
		cpu.print_register_bank();
		cout << "========================================";
		cout << "========================================" << endl;
	}
	this->current_tick = next_tick;
}
