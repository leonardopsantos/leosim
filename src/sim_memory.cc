/*
 * memory.cc
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "sim_instruction.hh"
#include "sim_memory.hh"
#include "sim_system.hh"

using namespace std;

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

cache::cache(const cache& o):memory(o.latency) {
	this->hit_ratio = o.hit_ratio;
}

cache::cache(int lat, int ratio): memory(lat) {
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

int memory::fill(std::ifstream &infile) {

	if( infile.is_open() == false )
		return -1;

	unsigned long int curr_addr = 0;

	string line;
	smatch base_match;

    regex comment_regex("^[ \t]*//", std::regex_constants::extended);
	regex org_regex("^[ \t]*org:[ \t]+0x([0-9a-f]+)", std::regex_constants::extended);
	regex movimm_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*(#([0-9])+|#0x([0-9a-f]+))", std::regex_constants::extended);
	regex mov_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*r([0-9]+)", std::regex_constants::extended);
	regex ldr_reg_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\][ \t]*$", std::regex_constants::extended);
	regex ldr_off_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-f]+))\][ \t]*$", std::regex_constants::extended);
	regex ldr_pre_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-f]+))\]( !|!)[ \t]*$", std::regex_constants::extended);
	regex ldr_post_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\],[ \t]*#(([-0-9]+)|([-0]+x[0-9a-f]+))[ \t]*$", std::regex_constants::extended);

	regex ldr_addsub_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+)[ \t]*$", std::regex_constants::extended);
	regex ldr_addimm_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*#(([0-9]+)|0x([0-9a-f]+))[ \t]*$", std::regex_constants::extended);

	while( getline(infile,line) ) {
		 if(regex_search (line, comment_regex)) {
			 continue;
		} else if( regex_match(line, base_match, org_regex)) {
			if( base_match.size() == 2 ) {
				curr_addr = stol(base_match[1].str(), nullptr, 16);
				cout << "curr_addr = " << curr_addr << endl;
			}
		} else if( regex_match(line, base_match, mov_regex)) {
			if( base_match.size() == 3 ) {
				cout << "MOV r" << stoi(base_match[1].str()) << ", r" << stoi(base_match[2].str()) << endl;
			}
		} else if( regex_match(line, base_match, movimm_regex)) {
			if( base_match.size() == 5 ) {
				if( base_match[2].str()[2] == 'x' )
					cout << "MOV r" << stoi(base_match[1].str()) << ", #0x" << stoi(base_match[4].str(), nullptr, 16) << endl;
				else
					cout << "MOV r" << stoi(base_match[1].str()) << ", #" << stoi(base_match[3].str()) << endl;
			}
		} else if(regex_match (line, base_match, ldr_reg_regex)) {
//			cout << line << " : base_match.size() = " << base_match.size() << endl;
//			for (unsigned int i = 0; i < base_match.size(); ++i) {
//				cout << "base_match[" << i << "].str() = " << base_match[i].str() << endl;
//			}
			if( base_match.size() == 4 ) {
				if( base_match[1].str() == "ldr" )
					cout << "LDR r" << base_match[2].str() << ", [r" << base_match[3].str() << "]" << endl;
				if( base_match[1].str() == "str" )
					cout << "STR r" << base_match[2].str() << ", [r" << base_match[3].str() << "]" << endl;
			}
		} else if(regex_match (line, base_match, ldr_off_regex)) {
			if( base_match.size() == 7 ) {
				if( base_match[1].str() == "ldr" )
					cout << "LDR r" << base_match[2].str() << ", [r" << base_match[3].str() << ", #" << base_match[4].str() << "]" << endl;
				if( base_match[1].str() == "str" )
					cout << "STR r" << base_match[2].str() << ", [r" << base_match[3].str() << ", #" << base_match[4].str() << "]" << endl;
			}
		} else if(regex_match (line, base_match, ldr_pre_regex)) {
//			cout << "LDR pre-indexed offset : " << line << endl;
			if( base_match.size() == 8 ) {
				if( base_match[1].str() == "ldr" )
					cout << "LDR r" << base_match[2].str() << ", [r" << base_match[3].str() << ", #" << base_match[4].str() << "]!" << endl;
				if( base_match[1].str() == "str" )
					cout << "STR r" << base_match[2].str() << ", [r" << base_match[3].str() << ", #" << base_match[4].str() << "]!" << endl;
			}
		} else if(regex_match (line, base_match, ldr_post_regex)) {
//			cout << "LDR post-indexed offset : " << line << endl;
//			cout << line << " : base_match.size() = " << base_match.size() << endl;
//			for (unsigned int i = 0; i < base_match.size(); ++i) {
//				cout << "base_match[" << i << "].str() = " << base_match[i].str() << endl;
//			}
			if( base_match.size() == 7 ) {
				if( base_match[1].str() == "ldr" )
					cout << "LDR r" << base_match[2].str() << ", [r" << base_match[3].str() << "], #" << base_match[4].str() << endl;
				if( base_match[1].str() == "str" )
					cout << "STR r" << base_match[2].str() << ", [r" << base_match[3].str() << "], #" << base_match[4].str() << endl;
			}
		} else if (regex_match (line, base_match, ldr_addsub_regex)) {
			if( base_match.size() == 5 ) {
				if( base_match[1].str() == "add" )
					cout << "ADD r" << base_match[2].str() << ", r" << base_match[3].str() << ", r" << base_match[4].str() << endl;
				if( base_match[1].str() == "sub" )
					cout << "SUB r" << base_match[2].str() << ", r" << base_match[3].str() << ", r" << base_match[4].str() << endl;
			}



		} else if (regex_match (line, base_match, ldr_addimm_regex)) {
//			cout << "ADD: " << line << endl;
//			cout << line << " : base_match.size() = " << base_match.size() << endl;
//			for (unsigned int i = 0; i < base_match.size(); ++i) {
//				cout << "base_match[" << i << "].str() = " << base_match[i].str() << endl;
//			}

			if( base_match.size() == 7 ) {
				if( base_match[1].str() == "add" )
					cout << "ADD r" << base_match[2].str() << ", r" << base_match[3].str() << ", #" << base_match[4].str() << endl;
				if( base_match[1].str() == "sub" )
					cout << "SUB r" << base_match[2].str() << ", r" << base_match[3].str() << ", #" << base_match[4].str() << endl;
			}

		} else {// regex_search (ldr_reg_regex, line)
			if( line.size() > 0 )
				cout << "Not matched" << line << endl;
		}
	}
	return 0;
}
