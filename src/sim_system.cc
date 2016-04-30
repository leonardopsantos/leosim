
#include <map>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

sim_system::sim_system(std::ifstream& input):l1dcache(20, 950),
	l1icache(20, 950) {
	l1icache.fill(input);
}
