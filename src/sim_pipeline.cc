/*
 * sim_pipeline.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include <ostream>
#include <vector>
#include <utility>

#include "sim_pipeline.hh"
#include "sim_instruction.hh"
#include "sim_system.hh"
#include "sim_memory.hh"
#include "sim_stats.hh"

#include "sim_features.hh"

extern int debug_level;

extern sim_stats simulator_stats;

instructionNOP staticNOP;
instructionEND staticEND;

using namespace std;

sim_pipeline::sim_pipeline(sim_system *system)
{
	this->latency = 1;
	this->next_tick_fetch = 0;
	this->next_tick_decode = 0;
	this->next_tick_execute = 0;
	this->next_tick_mem = 0;
	this->next_tick_commit = 0;
	this->system = system;
	this->cacheiL1If = &this->system->l1icache;
	this->cachedL1If = &this->system->l1dcache;
	this->cpu = &this->system->cpu;
	this->cpu_state = &this->system->cpu.state;
	this->fetchToDecode = &staticNOP;
	this->decodeToExecute = &staticNOP;
	this->executeToMemory = &staticNOP;
	this->memoryToCommit = &staticNOP;
	this->lastCommit = &staticNOP;
	this->branch_execute_taken = false;
	this->branch_decode_taken = false;

	pair<unsigned long int, bool> p;
	p = make_pair((unsigned int)-1, false);
	for (int i = 0; i < 10; ++i) {
		this->predictor_table[i] = p;
	}
}

ostream& operator<<(ostream& os, const sim_pipeline& pipe)
{
	os << "Fetch :  " << *pipe.fetchToDecode << endl;
	os << "Decode:  " << *pipe.decodeToExecute << endl;
	os << "Execute: " << *pipe.executeToMemory << endl;
	os << "Memory:  " << *pipe.memoryToCommit << endl;
	os << "Commit:  " << *pipe.lastCommit << endl;
	return os;
}

int sim_pipeline::next_tick(unsigned long int curr_tick) {
	return 0;
}

unsigned long int sim_pipeline::fetch(unsigned long int curr_tick, unsigned long int address,
		instruction** inst)
{
	*inst = this->cacheiL1If->get_content(address);
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::decode(unsigned long int curr_tick, instruction *inst)
{
	for(int i = 0; i < inst->num_sources; i++) {
		switch(inst->sourcesTypes[i]) {
		case instSources::REGISTER:
			inst->sources_values[i] = this->cpu->register_read(inst->sources_idx[i]);
			break;
		case instSources::IMMEDIATE:
		case instSources::MEMORY:
			break;
		case instSources::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}

	/* in the same cycle, conditional branches have precedence */
	if( inst->destsTypes[0] == instDest::BRANCH ) {

		if( branch_predictor(inst) == false )
			branch_predictor_update(inst);

		if( this->branch_execute_taken == false ) {
			unsigned long int pc_target;

			if( inst->sourcesTypes[0] == instSources::IMMEDIATE )
				pc_target = this->cacheiL1If->get_label_address(inst->tag);
			else if( inst->sourcesTypes[0] == instSources::REGISTER )
				pc_target = inst->sources_values[0];
			this->cpu_state->set_target_pc(pc_target);

			this->branch_decode_taken = true;
		}
	}

	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::execute(unsigned long int curr_tick, instruction *inst)
{
	/* Execute first so we can update flags */
	inst->execute();

	if( inst->destsTypes[0] == instDest::BRANCH_CONDITIONAL ) {

		instructionBRConditionalClass *i = dynamic_cast<instructionBRConditionalClass*>(inst);
		if ( i == NULL ) throw "Bad dynamic_cast in pipeline!";

		if( branch_predictor(inst) != i->should_jump )
			branch_predictor_update(inst);

		if( i->should_jump == false)
			return curr_tick+this->latency;

		this->branch_execute_taken = true;
	}

	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::memory(unsigned long int curr_tick, instruction* inst)
{
	for(int i = 0; i < inst->num_sources; i++) {
		switch(inst->sourcesTypes[i]) {
		case instSources::MEMORY:
			inst->destination_values[i] = this->system->l1dcache.get_content(inst->sources_idx[i]);
			break;
		case instSources::REGISTER:
		case instSources::IMMEDIATE:
			break;
		case instSources::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	for(int i = 0; i < inst->num_dests; i++) {
		switch(inst->destsTypes[i]) {
		case instDest::MEMORY:
			this->system->l1dcache.set_content(inst->dests_idx[i], inst->destination_values[i]);
			break;
		case instDest::REGISTER:
			break;
		case instDest::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	return curr_tick+this->latency;
}

unsigned long int sim_pipeline::commit(unsigned long int curr_tick, instruction *inst)
{
	inst->commit();
	for(int i = 0; i < inst->num_dests; i++) {
		switch(inst->destsTypes[i]) {
		case instDest::REGISTER:
			this->cpu->register_write(inst->dests_idx[i], inst->destination_values[i]);
			break;
		case instDest::MEMORY:
			break;
		case instDest::Invalid:
		default:
			throw "Invalid register source type!!";
		}
	}
	return curr_tick+this->latency;
}

void sim_pipeline::forward_data(instruction *insta, instruction *instb)
{
	for(int i = 0; i < insta->num_sources; i++) {
		if( insta->sourcesTypes[i] != instSources::REGISTER )
			continue;
		for(int j = 0; j < instb->num_dests; j++) {
			if( instb->destsTypes[j] != instDest::REGISTER )
				continue;

			if( insta->sources_idx[i] == instb->dests_idx[j] &&
					insta->sources_forward[i] == false) {
				insta->sources_values[i] = instb->destination_values[j];
				insta->sources_forward[i] = true;
				if( debug_level > 0 ) {
					cout << "        Forwarded [" << *insta << "] r" << insta->sources_idx[i] << " from [" << *instb << "] r" << instb->dests_idx[j] << " : "  << hex << instb->destination_values[j] << endl;
					cout << dec;
				}
			}
		}
	}
}

unsigned long sim_pipeline::get_pc_jump(instruction *inst)
{
	unsigned long int pc_target = 0;
	if( inst->destsTypes[0] == instDest::BRANCH_CONDITIONAL ||
	    inst->destsTypes[0] == instDest::BRANCH )
		pc_target = this->cacheiL1If->get_label_address(inst->tag);
	return pc_target;
}

bool sim_pipeline::branch_predictor(instruction *inst)
{
	pair<unsigned long int, bool> p;
	for (int i = 0; i < 10; ++i) {
		p = this->predictor_table[i];
		if( p.first == inst->memory_pos )
			return p.second;
	}
	return false;
}

void sim_pipeline::branch_predictor_update(instruction *inst)
{
	if( inst->destsTypes[0] != instDest::BRANCH_CONDITIONAL &&
	    inst->destsTypes[0] != instDest::BRANCH )
		return;

	pair<unsigned long int, bool> p;
	for (int i = 0; i < 10; ++i) {
		p = this->predictor_table[i];
		if( p.first == inst->memory_pos )
			return;
	}
	// not found on predictor table,
	// insert it
	for (int i = 0; i < 9; ++i) {
		this->predictor_table_new[i] = this->predictor_table[i+1];
	}
	p = make_pair(inst->memory_pos, true);
	this->predictor_table_new[9] = p;

	return;
}

void sim_pipeline::branch_predictor_commit()
{
	for (int i = 0; i < 10; ++i) {
		this->predictor_table[i] = this->predictor_table_new[i];
	}
}

void sim_pipeline::forward_branch(instruction *inst)
{
	if( inst->destsTypes[0] == instDest::BRANCH ||
		inst->destsTypes[0] == instDest::BRANCH_CONDITIONAL ) {
		unsigned long int pc_target;
		pc_target = this->cacheiL1If->get_label_address(inst->tag);
		this->cpu_state->set_target_pc(pc_target);
	}
}

void sim_pipeline::matrix_accel()
{
	unsigned long DA, DB, R, A, B, cont, inc;

	B = this->cpu->register_read(matrix_bank_B);
	A = this->cpu->register_read(matrix_bank_A);

	DA = this->cpu->register_read(matrix_bank_DataA);
	DB = this->cpu->register_read(matrix_bank_DataB);
	R = this->cpu->register_read(matrix_bank_Res);
	R += DA*DB;
	this->cpu->register_write(matrix_bank_Res, R);

	inc = this->cpu->register_read(matrix_bank_IncA);
	DA = this->system->l1dcache.get_content(A);
	A += inc;
	this->cpu->register_write(matrix_bank_A, A);
	this->cpu->register_write(matrix_bank_DataA, DA);

	inc = this->cpu->register_read(matrix_bank_IncB);
	DB = this->system->l1dcache.get_content(B);
	B += inc;
	this->cpu->register_write(matrix_bank_B, B);
	this->cpu->register_write(matrix_bank_DataB, DB);

	cont = this->cpu->register_read(matrix_bank_Count);
	this->cpu->register_write(matrix_bank_Count, --cont);

	simulator_stats.matrix_cycles++;
}

void sim_pipeline::linked_accel()
{
	unsigned long next, data, sdata, offset, tail, status;

	sdata = this->cpu->register_read(linked_bank_DataSearch);
	tail = this->cpu->register_read(linked_bank_Tail);
	next = this->cpu->register_read(linked_bank_El);
	this->cpu->register_write(linked_bank_Result, next);
	offset = this->cpu->register_read(linked_bank_DataOffset);
	data = this->system->l1dcache.get_content(next+offset);
	this->cpu->register_write(linked_bank_DataScratch, data);
	next = this->system->l1dcache.get_content(next);
	this->cpu->register_write(linked_bank_El, next);
	status = this->cpu->register_read(linked_bank_Status);

	if( data == sdata ) {
		status &= ~LINKED_BANK_BIT_START;
		status |= LINKED_BANK_BIT_FOUND | LINKED_BANK_BIT_STOP;
		this->cpu->register_write(linked_bank_Status, status);
	} else if( next == 0x00 ) {
		status &= ~LINKED_BANK_BIT_START;
		status |= LINKED_BANK_BIT_STOP;
		this->cpu->register_write(linked_bank_Status, status);
	}
	simulator_stats.linked_cycles++;
}

int sim_pipeline::clock_tick(unsigned long int curr_tick)
{
	unsigned long int pc_current = this->cpu_state->get_pc();
	unsigned long int pc_next = pc_current;

	instruction *current_fetch;

	bool halt_decode = false;
	bool halt_memory = false;

	#ifdef SIMCPU_FEATURE_FORWARD
	this->decodeToExecute->forward_clear();
	if( this->decodeToExecute->depends(this->executeToMemory) == true &&
			this->executeToMemory->inst_class == instClasses::MEM &&
			this->executeToMemory->sourcesTypes[0] == instSources::MEMORY) {
		// Dependency on loads needs at least one NOP
		halt_decode = true;
	} else {
		forward_data(this->decodeToExecute, this->executeToMemory);
	}
	forward_data(this->decodeToExecute, this->memoryToCommit);
	#else
	if( this->fetchToDecode->depends(this->decodeToExecute) == true ||
		this->fetchToDecode->depends(this->executeToMemory) == true ||
		this->fetchToDecode->depends(this->memoryToCommit) == true )
		halt_decode = true;
	#endif

	#if defined(SIMCPU_FEATURE_MATRIXACCEL) || defined(SIMCPU_FEATURE_LINKEDACCEL)
	// Matrix accelerator is active, halt memory
	long int matrix_status = this->cpu->register_read(matrix_bank_Status);
	long int linked_status = this->cpu->register_read(linked_bank_Status);
	// TODO: We should probably check if the memory unit is not being
	// used in this cycle!!

	if( matrix_status & MATRIX_BANK_BIT_START ) {
		halt_memory = true;
		matrix_accel();
		long int cont = this->cpu->register_read(matrix_bank_Count);
		if( cont == 0 ) {
			matrix_status &= ~MATRIX_BANK_BIT_START;
			matrix_status |= MATRIX_BANK_BIT_STOP;
			this->cpu->register_write(matrix_bank_Status, matrix_status);
		}
	} else if( linked_status & LINKED_BANK_BIT_START ) {
		halt_memory = true;
		linked_accel();
	} else
		halt_memory = false;

	#endif /* SIMCPU_FEATURE_MATRIXACCEL */

	/* COMMIT */
	if( debug_level > 0 )
		cout << "Commit:  (" << this->memoryToCommit->memory_pos << ") "<< *this->memoryToCommit << endl;

	this->commit(curr_tick, this->memoryToCommit);

	/* MEMORY */
	if( debug_level > 0 )
		cout << "Memory:  (" << this->executeToMemory->memory_pos << ") " << *this->executeToMemory << endl;
	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	if( halt_memory == false )
		this->memory(curr_tick, this->executeToMemory);
	#else
	this->memory(curr_tick, this->executeToMemory);
	#endif

	/* EXECUTE */
	this->branch_execute_taken = false;

	#ifdef SIMCPU_FEATURE_FORWARD
	#ifdef SIMCPU_FEATURE_MATRIXACCEL

	if( halt_decode == true || halt_memory == true ) {
	#else
	if( halt_decode == true ) {
	#endif // SIMCPU_FEATURE_MATRIXACCEL
		if( debug_level > 0 )
			cout << "Execute: (" << this->decodeToExecute->memory_pos << ") " << *this->decodeToExecute << " (held)" << endl;
	} else {
		if( debug_level > 0 )
			cout << "Execute: (" << this->decodeToExecute->memory_pos << ") " << *this->decodeToExecute << endl;
		this->execute(curr_tick, this->decodeToExecute);
	}
	#else // SIMCPU_FEATURE_FORWARD
	if( debug_level > 0 )
		cout << "Execute: " << *this->decodeToExecute << endl;
	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	if( this->decodeToExecute->is_dud == false && halt_memory == false )
	#else // SIMCPU_FEATURE_MATRIXACCEL
	if( this->decodeToExecute->is_dud == false )
	#endif // SIMCPU_FEATURE_MATRIXACCEL
		this->execute(curr_tick, this->decodeToExecute);
	#endif // SIMCPU_FEATURE_FORWARD

	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	// If we halted the memory unit, halt everything else
	if( halt_memory == true )
		halt_decode = true;
	#endif

	this->branch_decode_taken = false;
	if( halt_decode == false ) {
		/* DECODE */
		if( debug_level > 0 )
			cout << "Decode:  (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << endl;

		this->decode(curr_tick, this->fetchToDecode);

		/* FETCH */
		this->next_tick_fetch = this->fetch(curr_tick,
					pc_current, &current_fetch);
		/* A bad fetch ends the simulation */
		if( current_fetch == NULL )
			current_fetch = &staticEND;
		if( debug_level > 0 )
			cout << "Fetch:   (" << current_fetch->memory_pos << ") " << *current_fetch << endl;

	}

	#ifdef SIMCPU_FEATURE_BRANCHPRED
	/* Calculate the next PC */
	 if( halt_decode == true )
		pc_next = pc_current;
	 else if( branch_predictor(this->decodeToExecute) == false &&
			 this->branch_execute_taken == true ) {
		 pc_next = get_pc_jump(this->decodeToExecute);
	 } else if( branch_predictor(this->fetchToDecode) == false &&
			 this->branch_decode_taken == true ) {
		 pc_next = get_pc_jump(this->fetchToDecode);
	 } else if( branch_predictor(current_fetch) == true ) {
		 simulator_stats.branches_predicted_hit++;
		 pc_next = get_pc_jump(current_fetch);
	 } else {
		 pc_next = pc_current + 4;
	 }

	#else

	/* Calculate the next PC */
	 if( halt_decode == true )
		pc_next = pc_current;
	 else if( this->branch_execute_taken == true )
		 pc_next = get_pc_jump(this->decodeToExecute);
	 else if( this->branch_decode_taken == true )
		 pc_next = get_pc_jump(this->fetchToDecode);
	 else
		 pc_next = pc_current + 4;
	#endif

	/* Update barriers */
	this->memoryToCommit->update_stats();
	this->lastCommit = this->memoryToCommit;

	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	if( halt_memory == true ) {
		this->memoryToCommit = &staticNOP;
	} else {
		this->memoryToCommit = this->executeToMemory;
	}
	#else
	this->memoryToCommit = this->executeToMemory;
	#endif

	#ifdef SIMCPU_FEATURE_FORWARD

	#ifdef SIMCPU_FEATURE_MATRIXACCEL
	if( halt_memory == false ) {
		if( halt_decode == true ) {
			this->executeToMemory = &staticNOP;
		} else {
			this->executeToMemory = this->decodeToExecute;
		}
	}
	#else // SIMCPU_FEATURE_MATRIXACCEL
	if( halt_decode == true ) {
		this->executeToMemory = &staticNOP;
	} else {
		this->executeToMemory = this->decodeToExecute;
	}
	#endif // SIMCPU_FEATURE_MATRIXACCEL
	#else
	this->executeToMemory = this->decodeToExecute;
	#endif


	if( halt_decode == false ) {

		#ifdef SIMCPU_FEATURE_BRANCHPRED

		instruction *fetch_next, *decode_next;

		/* Predicted true but confirmed false, roll back */
		if( this->decodeToExecute->destsTypes[0] == instDest::BRANCH_CONDITIONAL &&
		    branch_predictor(this->decodeToExecute) != this->branch_execute_taken ) {
			decode_next = &staticNOP;
			fetch_next = &staticNOP;
			simulator_stats.ticks_halted_jumps+=2;
			simulator_stats.branches_predicted_miss++;
			simulator_stats.branches_predicted_hit--;
			/* We missed the prediction */
			if( this->branch_execute_taken == false )
				pc_next = this->decodeToExecute->memory_pos+4;
		} else if( this->fetchToDecode->destsTypes[0] == instDest::BRANCH &&
		      branch_predictor(this->fetchToDecode) != this->branch_decode_taken ) {
			pc_next = this->fetchToDecode->memory_pos+4;
			fetch_next = &staticNOP;
			simulator_stats.ticks_halted_jumps++;
			simulator_stats.branches_predicted_miss++;
			simulator_stats.branches_predicted_hit--;
			if( this->branch_decode_taken == false )
				pc_next = this->fetchToDecode->memory_pos+4;
		} else {
			decode_next = this->fetchToDecode;
			fetch_next = current_fetch;
		}

		this->decodeToExecute = decode_next;
		this->fetchToDecode = fetch_next;

#if 0
		/* just stats */
		if( this->decodeToExecute->destsTypes[0] == instDest::BRANCH_CONDITIONAL &&
		    branch_predictor(this->decodeToExecute) == this->branch_execute_taken ) {
			simulator_stats.branches_predicted_hit++;
		} else if( this->fetchToDecode->destsTypes[0] == instDest::BRANCH &&
		      branch_predictor(this->fetchToDecode) == this->branch_decode_taken ) {
			simulator_stats.branches_predicted_hit++;
		}
#endif

		#else

		// conditional branch taken at execute stage,
		// invalidate decode and fetch
		if( this->branch_execute_taken == true ) {
			this->decodeToExecute = &staticNOP;
			simulator_stats.ticks_halted_jumps++;
		} else {
			this->decodeToExecute = this->fetchToDecode;
		}

		// unconditional branch taken at execute stage,
		// invalidate fetch
		if( this->branch_execute_taken == true ||
		    this->branch_decode_taken == true ) {
			this->fetchToDecode = &staticNOP;
			simulator_stats.ticks_halted_jumps++;
		} else {
			this->fetchToDecode = current_fetch;
		}
		#endif

		this->cpu_state->set_target_pc(pc_next);
		this->cpu_state->update_pc();

	} else {
		if( debug_level > 0 ) {
			cout << "Decode:  (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << " (held)" << endl;
			cout << "Fetch:   (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << " (held)" << endl;
		}
		#if !defined(SIMCPU_FEATURE_FORWARD) || !defined(SIMCPU_FEATURE_MATRIXACCEL)
		this->decodeToExecute = &staticNOP;
		#endif
		simulator_stats.ticks_halted++;
	}

	#ifdef SIMCPU_FEATURE_BRANCHPRED
	branch_predictor_commit();
	#endif

	simulator_stats.ticks_total++;

	return curr_tick + this->latency;
}



#if 0

	int sim_pipeline::clock_tick(unsigned long int curr_tick)
	{
		unsigned long int curr_pc = this->cpu_state->get_pc();

		bool halt_decode = false;
		bool halt_memory = false;

		#ifdef SIMCPU_FEATURE_FORWARD
		this->decodeToExecute->forward_clear();
		if( this->decodeToExecute->depends(this->executeToMemory) == true &&
				this->executeToMemory->inst_class == instClasses::MEM &&
				this->executeToMemory->sourcesTypes[0] == instSources::MEMORY) {
			// Dependency on loads needs at least one NOP
			halt_decode = true;
		} else {
			forward_data(this->decodeToExecute, this->executeToMemory);
		}
		forward_data(this->decodeToExecute, this->memoryToCommit);
		#else
		if( this->fetchToDecode->depends(this->decodeToExecute) == true ||
			this->fetchToDecode->depends(this->executeToMemory) == true ||
			this->fetchToDecode->depends(this->memoryToCommit) == true )
			halt_decode = true;
		#endif



		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		// Matrix accelerator is active, halt memory
		long int matrix_status = this->cpu->register_read(matrix_bank_Status);

		// TODO: We should probably check if the memory unit is not being
		// used in this cycle!!

		if( matrix_status & MATRIX_BANK_BIT_START ) {
			halt_memory = true;
			matrix_accel();
			long int cont = this->cpu->register_read(matrix_bank_Count);
			if( cont == 0 ) {
				matrix_status &= ~MATRIX_BANK_BIT_START;
				matrix_status |= MATRIX_BANK_BIT_STOP;
				this->cpu->register_write(matrix_bank_Status, matrix_status);
			}
		} else
			halt_memory = false;
		#endif



		/* COMMIT */
		if( debug_level > 0 )
			cout << "Commit:  (" << this->memoryToCommit->memory_pos << ") "<< *this->memoryToCommit << endl;
		if( this->memoryToCommit->is_dud == false )
			this->commit(curr_tick, this->memoryToCommit);

		/* MEMORY */
		if( debug_level > 0 )
			cout << "Memory:  (" << this->executeToMemory->memory_pos << ") " << *this->executeToMemory << endl;
		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		if( this->executeToMemory->is_dud == false && halt_memory == false )
		#else
		if( this->executeToMemory->is_dud == false )
		#endif
			this->memory(curr_tick, this->executeToMemory);

		/* EXECUTE */
		#ifdef SIMCPU_FEATURE_FORWARD
		#ifdef SIMCPU_FEATURE_MATRIXACCEL

		if( halt_decode == true || halt_memory == true ) {
		#else
		if( halt_decode == true ) {
		#endif // SIMCPU_FEATURE_MATRIXACCEL
			if( debug_level > 0 )
				cout << "Execute: (" << this->decodeToExecute->memory_pos << ") " << *this->decodeToExecute << " (held)" << endl;
		} else {
			if( debug_level > 0 )
				cout << "Execute: (" << this->decodeToExecute->memory_pos << ") " << *this->decodeToExecute << endl;
			if( this->decodeToExecute->is_dud == false )
				this->execute(curr_tick, this->decodeToExecute);
		}
		#else // SIMCPU_FEATURE_FORWARD
		if( debug_level > 0 )
			cout << "Execute: " << *this->decodeToExecute << endl;
		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		if( this->decodeToExecute->is_dud == false && halt_memory == false )
		#else // SIMCPU_FEATURE_MATRIXACCEL
		if( this->decodeToExecute->is_dud == false )
		#endif // SIMCPU_FEATURE_MATRIXACCEL
			this->execute(curr_tick, this->decodeToExecute);
		#endif // SIMCPU_FEATURE_FORWARD

		this->memoryToCommit->update_stats();
		this->lastCommit = this->memoryToCommit;
		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		if( halt_memory == true ) {
			this->memoryToCommit = &staticNOP;
		} else {
			this->memoryToCommit = this->executeToMemory;
		}
		#else
		this->memoryToCommit = this->executeToMemory;
		#endif

		#ifdef SIMCPU_FEATURE_FORWARD
		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		if( halt_memory == false ) {
			if( halt_decode == true ) {
				this->executeToMemory = &staticNOP;
			} else {
				this->executeToMemory = this->decodeToExecute;
			}
		}
		#else
		if( halt_decode == true ) {
			this->executeToMemory = &staticNOP;
		} else {
			this->executeToMemory = this->decodeToExecute;
		}
		#endif // SIMCPU_FEATURE_MATRIXACCEL
		#else
		this->executeToMemory = this->decodeToExecute;
		#endif


		#ifdef SIMCPU_FEATURE_MATRIXACCEL
		// If we halted the memory unit, halt everything else
		if( halt_memory == true )
			halt_decode = true;
		#endif

		if( halt_decode == false ) {
			/* DECODE */
			if( debug_level > 0 )
				cout << "Decode:  (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << endl;

			#ifndef SIMCPU_FEATURE_BRANCHPRED
			// conditional branch taken
			if( this->cpu_state->branch == true ) {
				this->fetchToDecode = &staticNOP;
				simulator_stats.ticks_halted_jumps++;
			} else {
				if( this->fetchToDecode->is_dud == false )
					this->decode(curr_tick, this->fetchToDecode);
			}
			this->decodeToExecute = this->fetchToDecode;

			/* FETCH */
			instruction *f;

			// branch taken
			if( this->cpu_state->branch == true ) {
				this->fetchToDecode = &staticNOP;
				simulator_stats.ticks_halted_jumps++;
			} else {
				this->next_tick_fetch = this->fetch(curr_tick,
							curr_pc, &f);
				/* A bad fetch ends the simulation */
				this->fetchToDecode = (f == NULL ? &staticEND : f);
			}

			if( debug_level > 0 )
				cout << "Fetch:   (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << endl;
			#else

			if( this->fetchToDecode->is_dud == false )
				this->decode(curr_tick, this->fetchToDecode);
			this->decodeToExecute = this->fetchToDecode;

			/* FETCH */
			instruction *f;

			this->next_tick_fetch = this->fetch(curr_tick,
						curr_pc, &f);
			/* A bad fetch ends the simulation */
			this->fetchToDecode = (f == NULL ? &staticEND : f);

			// predict taken
			if( (this->fetchToDecode->destsTypes[0] == instDest::BRANCH_CONDITIONAL ||
			     this->fetchToDecode->destsTypes[0] == instDest::BRANCH) &&
			     this->fetchToDecode->sourcesTypes[0] == instSources::IMMEDIATE ) {
				set_pc_jump(this->fetchToDecode);
				if( debug_level > 0 )
					cout << "Fetch:   (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << " (taken)" << endl;
			} else
				if( debug_level > 0 )
					cout << "Fetch:   (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << endl;

			// conditional branch was NOT taken
			// branch was NOT taken
			// we guessed taken, invalidate decode, invalidate fetch, fix PC
			if( this->executeToMemory->destsTypes[0] == instDest::BRANCH_CONDITIONAL &&
					this->cpu_state->branch == false ) {

				this->decodeToExecute = &staticNOP;
				this->fetchToDecode = &staticNOP;
				// next PC should be EXECUTE inst +4
				this->cpu_state->set_target_pc(this->executeToMemory->memory_pos+4);
				simulator_stats.ticks_halted_jumps += 2;

			} else if( this->decodeToExecute->destsTypes[0] == instDest::BRANCH &&
					this->cpu_state->branch == false ) {

				this->fetchToDecode = &staticNOP;
				// next PC should be EXECUTE inst +4
				this->cpu_state->set_target_pc(this->decodeToExecute->memory_pos+4);
				simulator_stats.ticks_halted_jumps++;
			}
			#endif

			this->cpu_state->update_pc();
		} else {
			if( debug_level > 0 ) {
				cout << "Decode:  (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << " (held)" << endl;
				cout << "Fetch:   (" << this->fetchToDecode->memory_pos << ") " << *this->fetchToDecode << " (held)" << endl;
			}
			#if !defined(SIMCPU_FEATURE_FORWARD) || !defined(SIMCPU_FEATURE_MATRIXACCEL)
			this->decodeToExecute = &staticNOP;
			#endif
			simulator_stats.ticks_halted++;
		}

		simulator_stats.ticks_total++;

		return curr_tick + this->latency;
	}


#endif
