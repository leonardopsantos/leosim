/*
 * sim_pipeline.cc
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#include <ostream>

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
	if( inst->destsTypes[0] == instDest::BRANCH && this->cpu_state->branch == false ) {
		unsigned long int pc_target;

		#ifndef SIMCPU_FEATURE_BRANCHPRED_IDIOT
		if( inst->sourcesTypes[0] == instSources::IMMEDIATE )
			pc_target = this->cacheiL1If->get_label_address(inst->tag);
		else if( inst->sourcesTypes[0] == instSources::REGISTER )
			pc_target = inst->sources_values[0];
		this->cpu_state->set_target_pc(pc_target);
		#else
		// Immediate branches were predicted as taken,
		// go to next instruction, as the PC was updated
		// when we predicted the branch
		if( inst->sourcesTypes[0] == instSources::IMMEDIATE )
			// THIS IS IMPORTANT, we need the cpu_state->branch as true!
			pc_target = this->cpu_state->get_pc()+4;
		// Need the register value, can't predict yet
		else if( inst->sourcesTypes[0] == instSources::REGISTER )
			pc_target = inst->sources_values[0];

		this->cpu_state->set_target_pc(pc_target);

		#endif
	}

	return curr_tick+this->latency;
}

void sim_pipeline::set_pc_jump(instruction *inst)
{
	unsigned long int pc_target;
	pc_target = this->cacheiL1If->get_label_address(inst->tag);
	this->cpu_state->set_target_pc(pc_target);
}

unsigned long int sim_pipeline::execute(unsigned long int curr_tick, instruction *inst)
{
	/* Execute first so we can update flags */
	inst->execute();

	if( inst->destsTypes[0] == instDest::BRANCH_CONDITIONAL ) {

		instructionBRConditionalClass *i = dynamic_cast<instructionBRConditionalClass*>(inst);
		if ( i == NULL ) throw "Bad dynamic_cast in pipeline!";

		if( i->should_jump == false)
			return curr_tick+this->latency;

		#ifdef SIMCPU_FEATURE_BRANCHPRED_IDIOT
		// Immediate branches were predicted as taken,
		// go to next instruction, as the PC was updated
		// when we predicted the branch
		if( inst->sourcesTypes[0] == instSources::IMMEDIATE ) {
			// THIS IS IMPORTANT, we need the cpu_state->branch as true!
			unsigned long int pc_target;
			pc_target = this->cpu_state->get_pc()+4;
			this->cpu_state->set_target_pc(pc_target);
			return curr_tick+this->latency;
		}
		#endif
		set_pc_jump(inst);
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
			}
		}
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

int sim_pipeline::clock_tick(unsigned long int curr_tick)
{
	unsigned long int curr_pc = this->cpu_state->get_pc();
	bool halt_pipeline = false;

	#ifdef SIMCPU_FEATURE_FORWARD
	this->decodeToExecute->forward_clear();
	if( this->decodeToExecute->depends(this->executeToMemory) == true &&
			this->executeToMemory->inst_class == instClasses::MEM &&
			this->executeToMemory->sourcesTypes[0] == instSources::MEMORY) {
		// Dependency on loads needs at least one NOP
		halt_pipeline = true;
	} else {
		forward_data(this->decodeToExecute, this->executeToMemory);
	}
	forward_data(this->decodeToExecute, this->memoryToCommit);
	#else
	if( this->fetchToDecode->depends(this->decodeToExecute) == true ||
		this->fetchToDecode->depends(this->executeToMemory) == true ||
		this->fetchToDecode->depends(this->memoryToCommit) == true )
		halt_pipeline = true;
	#endif

	/* COMMIT */
	if( debug_level > 0 )
		cout << "Commit:  " << *this->memoryToCommit << endl;
	if( this->memoryToCommit->is_dud == false )
		this->commit(curr_tick, this->memoryToCommit);

	/* MEMORY */
	if( debug_level > 0 )
		cout << "Memory:  " << *this->executeToMemory << endl;
	if( this->executeToMemory->is_dud == false )
		this->memory(curr_tick, this->executeToMemory);

	/* EXECUTE */
	#ifdef SIMCPU_FEATURE_FORWARD
	if( halt_pipeline == true ) {
		if( debug_level > 0 )
			cout << "Execute: " << *this->decodeToExecute << " (held)" << endl;
	} else {
		if( debug_level > 0 )
			cout << "Execute: " << *this->decodeToExecute << endl;
		if( this->decodeToExecute->is_dud == false )
			this->execute(curr_tick, this->decodeToExecute);
	}
	#else
	if( debug_level > 0 )
		cout << "Execute: " << *this->decodeToExecute << endl;
	if( this->decodeToExecute->is_dud == false )
		this->execute(curr_tick, this->decodeToExecute);
	#endif

	this->memoryToCommit->update_stats();
	this->lastCommit = this->memoryToCommit;
	this->memoryToCommit = this->executeToMemory;


	#ifdef SIMCPU_FEATURE_FORWARD
	if( halt_pipeline == true ) {
		this->executeToMemory = &staticNOP;
	} else {
		this->executeToMemory = this->decodeToExecute;
	}
	#else
	this->executeToMemory = this->decodeToExecute;
	#endif

	if( halt_pipeline == false ) {
		/* DECODE */
		if( debug_level > 0 )
			cout << "Decode:  " << *this->fetchToDecode << endl;

		#ifndef SIMCPU_FEATURE_BRANCHPRED_IDIOT
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
			cout << "Fetch:   " << *this->fetchToDecode << endl;
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
				cout << "Fetch:   " << *this->fetchToDecode << " (taken)" << endl;
		} else
			if( debug_level > 0 )
				cout << "Fetch:   " << *this->fetchToDecode << endl;

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
			cout << "Decode:  " << *this->fetchToDecode << " (held)" << endl;
			cout << "Fetch:   " << *this->fetchToDecode << " (held)" << endl;
		}
		#ifndef SIMCPU_FEATURE_FORWARD
		this->decodeToExecute = &staticNOP;
		#endif
		simulator_stats.ticks_halted++;
	}

	/* Executed a conditional branch that was taken,
	 * need to invalidate the decode and fetch instructions */

	simulator_stats.ticks_total++;

	return curr_tick + this->latency;
}



#if 0

int sim_pipeline::clock_tick(unsigned long int curr_tick)
{
	unsigned long int curr_pc = this->cpu_state->get_pc();
	bool halt_pipeline = false;

	#ifndef SIMCPU_FEATURE_FORWARD
	if( this->fetchToDecode->depends(this->decodeToExecute) == true ||
		this->fetchToDecode->depends(this->executeToMemory) == true ||
		this->fetchToDecode->depends(this->memoryToCommit) == true )
		halt_pipeline = true;
	#endif

	/* COMMIT */
	if( debug_level > 0 )
		cout << "Commit:  " << *this->memoryToCommit << endl;
	if( this->memoryToCommit->is_dud == false )
		this->commit(curr_tick, this->memoryToCommit);

	/* MEMORY */
	if( debug_level > 0 )
		cout << "Memory:  " << *this->executeToMemory << endl;
	if( this->executeToMemory->is_dud == false )
		this->memory(curr_tick, this->executeToMemory);

	/* EXECUTE */
	if( debug_level > 0 )
		cout << "Execute: " << *this->decodeToExecute << endl;

	#ifdef SIMCPU_FEATURE_FORWARD
	this->decodeToExecute->forward_clear();
	forward_data(this->decodeToExecute, this->executeToMemory);
	forward_data(this->decodeToExecute, this->memoryToCommit);
	#endif
	if( this->decodeToExecute->is_dud == false )
		this->execute(curr_tick, this->decodeToExecute);

	/* Executed a conditional branch that was taken,
	 * need to invalidate the decode and fetch instructions */
	#ifndef SIMCPU_FEATURE_BRANCHPRED_IDIOT
	if( this->cpu_state->branch == true ) {
		this->fetchToDecode = &staticNOP;
	}
	#else
	if( this->cpu_state->branch == false ) {
		this->fetchToDecode = &staticNOP;
	}
	#endif

	this->memoryToCommit->update_stats();
	this->lastCommit = this->memoryToCommit;
	this->memoryToCommit = this->executeToMemory;
	this->executeToMemory = this->decodeToExecute;

	if( halt_pipeline == false ) {
		/* DECODE */
		if( debug_level > 0 )
			cout << "Decode:  " << *this->fetchToDecode << endl;

		if( this->fetchToDecode->is_dud == false )
			this->decode(curr_tick, this->fetchToDecode);
		this->decodeToExecute = this->fetchToDecode;

		/* FETCH */
		instruction *f;
		if( this->cpu_state->branch == false ) {

			this->next_tick_fetch = this->fetch(curr_tick,
						curr_pc, &f);
			/* A bad fetch ends the simulation */
			this->fetchToDecode = (f == NULL ? &staticEND : f);
		} else
			this->fetchToDecode = &staticNOP;

		if( debug_level > 0 )
			cout << "Fetch:   " << *this->fetchToDecode << endl;

		#ifndef SIMCPU_FEATURE_BRANCHPRED_IDIOT
		this->cpu_state->update_pc();
		#else

		#endif
	} else {
		if( debug_level > 0 ) {
			cout << "Decode:  " << *this->fetchToDecode << " (held)" << endl;
			cout << "Fetch:   " << *this->fetchToDecode << " (held)" << endl;
		}
		this->decodeToExecute = &staticNOP;
		simulator_stats.ticks_halted++;
	}

	simulator_stats.ticks_total++;

	return curr_tick + this->latency;
}


#endif
