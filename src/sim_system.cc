
#include <map>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

sim_system::sim_system(std::ifstream& input) {
	l1dcache = cache(20, 950);
	l1icache = cache(20, 950);
}
