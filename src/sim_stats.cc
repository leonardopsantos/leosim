/*
 * sim_stats.cc
 *
 *  Created on: 27 de mai de 2016
 *      Author: lsantos
 */

#include "sim_stats.hh"

sim_stats::sim_stats() {
	this->ticks_total = 0;
	this->ticks_halted = 0;
	this->instructions_total = 0;
	for (unsigned int i = static_cast< std::size_t >( instClasses::Invalid ); i < static_cast< std::size_t >( instClasses::maxInstClasses ); ++i) {
		this->instructions_by_type[i] = 0;
	}
	this->ticks_halted_jumps = 0;
	this->branches_predicted_hit = 0;
	this->branches_predicted_miss = 0;
}

ostream& operator<<(ostream& os, const sim_stats& stats)
{
	os << "=========================================================" << endl;
	os << "Simulation Statistics: " << endl;
	os << "  Total ticks :  " << stats.ticks_total << endl;
	os << "  Ticks inserted due to data dependencies:  " << stats.ticks_halted << endl;
	os << "  Ticks inserted due to missed jump prediction:  " << stats.ticks_halted_jumps << endl;
	os << "  Total instruction cache read accesses: " << stats.memory_instructions_accesses << endl;
	os << "  Total data cache read accesses: " << stats.memory_data_accesses_read << endl;
	os << "  Total data cache write accesses: " << stats.memory_data_accesses_write << endl;

	os << "  Matrix accelerator cycles: " << stats.matrix_cycles << endl;
	os << "  Linked list accelerator cycles: " << stats.linked_cycles << endl;

	os << "  Total instructions committed :  " << stats.instructions_total << endl;

	int useful_insts = 0;
	unsigned int x;

	x = static_cast< std::size_t >( instClasses::NOP );

	for (unsigned int i = static_cast< std::size_t >( instClasses::First );
			i < static_cast< std::size_t >( instClasses::maxInstClasses ); ++i) {

	    const char* s = 0;
		#define PROCESS_VAL(p) case(static_cast< std::size_t >((p))): s = #p; break;
	    	switch(i){
	        PROCESS_VAL(instClasses::NOP);
	        PROCESS_VAL(instClasses::ARITH);
	        PROCESS_VAL(instClasses::MUL);
	        PROCESS_VAL(instClasses::MOV);
	        PROCESS_VAL(instClasses::MEM);
	        PROCESS_VAL(instClasses::CTRL);
	    	}
		#undef PROCESS_VAL

		if( i != x )
			useful_insts += stats.instructions_by_type[i];

	    os << "    Total " << s << " : " << stats.instructions_by_type[i] << endl;
	}
	os << "  Branch predicted hit: " << stats.branches_predicted_hit << endl;
	os << "  Branch predicted miss: " << stats.branches_predicted_miss << endl;
	os << "  CPI: " << (((float)stats.ticks_total/useful_insts)) << endl;
	os << "  IPC: " << (((float)useful_insts/stats.ticks_total)) << endl;
	os << "=========================================================" << endl;
	return os;
}
