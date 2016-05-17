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
//#include "sim_system.hh"

using namespace std;

memory::memory(const memory& o)
{
	this->latency = o.latency;
}

memory::memory(int lat)
{
	this->latency = lat;
}

memory::~memory()
{}

unsigned long int memory::clock_tick(unsigned long int curr_tick)
{
	return 0;
}

unsigned long int memory::next_tick(unsigned long int curr_tick)
{
	return 0;
}

cache_generic::cache_generic(const cache_generic& o):memory(o.latency)
{
	this->hit_ratio = o.hit_ratio;
}

cache_generic::cache_generic(int lat, int ratio): memory(lat)
{
	this->hit_ratio = ratio;
}

cache_generic::~cache_generic()
{}

int cache_instructions::fill(std::ifstream &infile)
{

	if( infile.is_open() == false )
		return -1;

	instruction *new_inst;

	unsigned long int curr_addr = 0;

	string line;
	smatch base_match;

    regex comment_regex("^[ \t]*//", std::regex_constants::extended);
    regex end_regex("^[ \t]*end[ \t]*$", std::regex_constants::extended);
	regex org_regex("^[ \t]*org:[ \t]+0x([0-9a-fA-F]+)", std::regex_constants::extended);
	regex label_regex("^([a-zA-Z0-9_]+):$", std::regex_constants::extended);

	while( getline(infile,line) ) {

		/* empty lines */
		if( line.length() == 0 )
			continue;

		 if(regex_search (line, comment_regex)) {
			 continue;
		} else if( regex_match(line, base_match, end_regex)) {
			cout << "END" << endl;
		} else if( regex_match(line, base_match, label_regex)) {
//			cout << "LABEL: " << line << " : base_match.size() = " << base_match.size() << endl;
//			for (unsigned int i = 0; i < base_match.size(); ++i) {
//				cout << "base_match[" << i << "].str() = " << base_match[i].str() << endl;
//			}
			string s = base_match[1].str();
			labels[s] = curr_addr;
		} else if( regex_match(line, base_match, org_regex)) {
			if( base_match.size() == 2 ) {
				curr_addr = stol(base_match[1].str(), nullptr, 16);
//				cout << "curr_addr = " << curr_addr << endl;
			}
		} else {
			new_inst = instructionFactory::buildInstruction(curr_addr, line);
			if( new_inst != NULL ) {
				content[curr_addr] = new_inst;
				curr_addr += 4;
//				cout << *new_inst << endl;
			} else {
				cout << "NOT MATCHED!: " << line << endl;
			}
		}
	}

	/* Check if all labels used in the assembly file actually exist */

	for(map<unsigned long int, instruction*>::iterator iterator = content.begin();
			iterator != content.end(); ++iterator) {

		if (dynamic_cast<instructionBR*>(iterator->second) == NULL)
			continue;

		instructionBR* br = dynamic_cast<instructionBR*>(iterator->second);

		if( labels.find(br->tag) == labels.end() ) {
			cout << "BAD LABEL " << br->tag << " !!!!" << endl;
			cout << "Can't proceed! Fix you codez!!" << endl;
			return -1;
		}

	}
	return 0;
}

cache_instructions::cache_instructions(int lat, int ratio):cache_generic(lat, ratio)
{}

cache_instructions::~cache_instructions()
{}

instruction* cache_instructions::get_content(unsigned long int address)
{
	return this->content[address];
}

void cache_instructions::set_content(unsigned long int address, instruction* inst)
{
	this->content[address] = inst;
}

unsigned long int cache_instructions::get_label_address(string label)
{
	return this->labels[label];
}

cache_data::cache_data(int lat, int ratio):cache_generic(lat, ratio)
{}

cache_data::~cache_data()
{}

long int cache_data::get_content(unsigned long int address)
{
	return this->content[address];
}

void cache_data::set_content(unsigned long int address, long int data)
{
	this->content[address] = data;
}

int cache_data::fill(std::ifstream& infile)
{
	return 0;
}
