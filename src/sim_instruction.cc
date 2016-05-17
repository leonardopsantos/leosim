/*
 * instruction.cc
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#include "sim_instruction.hh"

#include <cstdlib>

instruction::instruction() {

	this->num_sources = 0;
	this->num_dests = 0;

	for (int i = 0; i < 4; ++i) {
		this->sourcesTypes[i] = instSources::Invalid;
		this->sources_idx[i] = 0;
		this->destsTypes[i] = instDest::Invalid;
		this->dests_idx[i] = 0;
	}
	this->memory_pos = 0;
	this->imm = 0;
}

instruction::~instruction() {
}

void instruction::execute()
{}

ostream& operator<<(ostream& os, const instruction& inst)
{
    inst.print(os);
    return os;
}

void instruction::print(ostream& where) const {
	where << "meh!";
}

instructionNOP::instructionNOP() {
}

instructionNOP::instructionNOP(unsigned long int addr)
{
	this->memory_pos = addr;
}

void instructionNOP::execute() {
}

void instructionNOP::print(ostream& where) const {
	where << this->memory_pos << " : NOP";
}

instructionADD::instructionADD() {
}

instructionADD::instructionADD(unsigned long int addr, long int s1, long int s2, long int d)
{
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->sources_idx[1] = s2;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionADD::execute() {
	this->destination_values[0] = this->sources_values[0] + this->sources_values[1];
}

void instructionADD::print(ostream& where) const {
	where << this->memory_pos << " : ADD r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", r" << this->sources_idx[1];
}

instructionADDImm::instructionADDImm() {
}

instructionADDImm::instructionADDImm(unsigned long int addr, long int s1, long int imm,
		long int d) {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->imm = imm;
	this->memory_pos = addr;
}

void instructionADDImm::print(ostream& where) const {
	where << "ADDImm r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", #" << this->imm;
}

instructionSUB::instructionSUB() {
}

instructionSUB::instructionSUB(unsigned long int addr, long int s1, long int s2, long int d)
{
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->sources_idx[1] = s2;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionSUB::print(ostream& where) const {
	where << "SUB r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", r" << this->sources_idx[1];
}

instructionSUBImm::instructionSUBImm() {
}

instructionSUBImm::instructionSUBImm(unsigned long int addr, long int s1, long int imm,
		long int d) {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->imm = imm;
	this->memory_pos = addr;
}

void instructionSUBImm::print(ostream& where) const {
	where << "SUBImm r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", #" << this->imm;
}

instructionMUL::instructionMUL() {
}

instructionMUL::instructionMUL(unsigned long int addr, long int s1, long int s2, long int d) {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->sources_idx[1] = s2;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMUL::print(ostream& where) const {
	where << "MUL r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", r" << this->sources_idx[1];
}

instructionMLA::instructionMLA() {
}

instructionMLA::instructionMLA(unsigned long int addr, long int s1, long int s2, long int s3, long int d) {
	this->num_sources = 3;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->sources_idx[1] = s2;
	this->sources_idx[2] = s3;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMLA::print(ostream& where) const {
	where << "MLA r" << this->dests_idx[0]
				<< ", r" << this->sources_idx[0]
				<< ", r" << this->sources_idx[1]
				<< ", r" << this->sources_idx[2];
}

instructionMOV::instructionMOV() {
}

instructionMOV::instructionMOV(unsigned long int addr, long int s1, long int d) {
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMOV::print(ostream& where) const {
	where << this->memory_pos << " : MOV r" << this->dests_idx[0] << ", r" << this->sources_idx[0];
}

instructionMOVImm::instructionMOVImm() {
}

instructionMOVImm::instructionMOVImm(unsigned long int addr, long int imm, long int d) {
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::IMMEDIATE;
	this->imm = imm;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMOVImm::print(ostream& where) const {
	where << this->memory_pos << " : MOVImm r" << this->dests_idx[0] << ", #" << this->imm;
}

instructionLDR::instructionLDR() {}

instructionLDR::instructionLDR(unsigned long int addr, long int s1, long int d) {
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;

	this->sources_idx[0] = s1;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionLDR::print(ostream& where) const {
	where << "LDR r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << "]";
}

instructionLDROff::instructionLDROff() {
	this->indexing = ldrIndexing::indexing_OFFSET;
}

instructionLDROff::instructionLDROff(unsigned long int addr, long int s1, long int imm, long int d) {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->sources_idx[0] = s1;
	this->imm = imm;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;

	this->indexing = ldrIndexing::indexing_OFFSET;
}

void instructionLDROff::print(ostream& where) const {
	where << "LDRoff r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->imm << "]";
}

instructionLDRPre::instructionLDRPre() {
	this->indexing = ldrIndexing::indexing_PRE;
}

instructionLDRPre::instructionLDRPre(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionLDROff(addr, s1, imm, d) {
	this->indexing = ldrIndexing::indexing_PRE;
}

void instructionLDRPre::print(ostream& where) const {
	where << "LDRpre r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->imm << "] !";
}

instructionLDRPost::instructionLDRPost() {
	this->indexing = ldrIndexing::indexing_POST;
}

instructionLDRPost::instructionLDRPost(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionLDROff(addr, s1, imm, d) {
	this->indexing = ldrIndexing::indexing_POST;
}

void instructionLDRPost::print(ostream& where) const {
	where << "LDRpost r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << "], #" << this->imm;
}

instructionSTR::instructionSTR() {}

instructionSTR::instructionSTR(unsigned long int addr, long int s1, long int d) {
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;

	this->sources_idx[0] = s1;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionSTR::print(ostream& where) const {
	where << "STR r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << "]";
}

instructionSTROff::instructionSTROff() {
	this->indexing = ldrIndexing::indexing_OFFSET;
}

instructionSTROff::instructionSTROff(unsigned long int addr, long int s1, long int imm, long int d) {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->sources_idx[0] = s1;
	this->imm = imm;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;

	this->indexing = ldrIndexing::indexing_OFFSET;
}

void instructionSTROff::print(ostream& where) const {
	where << "STRoff r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->imm << "]";
}

instructionSTRPre::instructionSTRPre() {
	this->indexing = ldrIndexing::indexing_PRE;
}

instructionSTRPre::instructionSTRPre(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionSTROff(addr, s1, imm, d) {
	this->indexing = ldrIndexing::indexing_PRE;
}

void instructionSTRPre::print(ostream& where) const {
	where << "STRpre r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->imm << "] !";
}

instructionSTRPost::instructionSTRPost() {
	this->indexing = ldrIndexing::indexing_POST;
}

instructionSTRPost::instructionSTRPost(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionSTROff(addr, s1, imm, d) {
	this->indexing = ldrIndexing::indexing_POST;
}

void instructionSTRPost::print(ostream& where) const {
	where << "STRpost r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << "], #" << this->imm;
}

instructionBR::instructionBR() {
}

instructionBR::instructionBR(unsigned long int addr, string mem_tag) {
	this->num_sources = 1;
	this->tag = mem_tag;
	this->memory_pos = addr;
	this->dest = 0;
}

void instructionBR::print(ostream& where) const {
	where << "B " << this->tag << " ( " << this->dest << " )";
}

instructionBRX::instructionBRX() {
}

instructionBRX::instructionBRX(unsigned long int addr, long int s1) {
	this->num_sources = 1;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sources_idx[0] = s1;
	this->memory_pos = addr;
}

void instructionBRX::print(ostream& where) const {
	where << "BrX r" << this->sources_idx[0];
}

instructionBRLink::instructionBRLink() {
}

instructionBRLink::instructionBRLink(unsigned long int addr, string mem_tag):instructionBR(addr, mem_tag) {
	this->tag = mem_tag;
}

void instructionBRLink::print(ostream& where) const {
	where << "BLr " << this->tag;
}

instructionBLX::instructionBLX() {
}

instructionBLX::instructionBLX(unsigned long int addr, long int s1): instructionBRX(addr, s1) {
}

void instructionBLX::print(ostream& where) const {
	where << "BLX r" << this->sources_idx[0];
}

instruction* instructionFactory::buildInstruction(unsigned long int addr, string line) {

	instruction *new_inst = NULL;

	smatch base_match;

	regex movimm_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*#(([-0-9])+|([-0]+x[0-9a-fA-F]+))", std::regex_constants::extended | std::regex_constants::icase);
	regex mov_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*r([0-9]+)", std::regex_constants::extended | std::regex_constants::icase);

	regex ldr_reg_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\][ \t]*$", std::regex_constants::extended | std::regex_constants::icase | std::regex_constants::icase);
	regex ldr_off_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-fA-F]+))\][ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex ldr_pre_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-fA-F]+))\]( !|!)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex ldr_post_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\],[ \t]*#(([-0-9]+)|([-0]+x[0-9a-fA-F]+))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex addsub_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex addimm_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*#(([0-9]+)|0x([0-9a-fA-F]+))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex mul_regex("^[ \t]*mul[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex mla_regex("^[ \t]*mla[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex branch_regex("^[ \t]*(b|bl)[ \t]*([0-9a-zA-Z]+)$", std::regex_constants::extended | std::regex_constants::icase);
	regex branchx_regex("^[ \t]*(bx|blx)[ \t]*r([0-9]+)$", std::regex_constants::extended | std::regex_constants::icase);

	if( regex_match(line, base_match, mov_regex) && base_match.size() == 3 ) {

		new_inst = new instructionMOV(addr, stol(base_match[2].str()), stol(base_match[1].str()));

	} else if( regex_match(line, base_match, movimm_regex) /*&& base_match.size() == 5 */) {
		long int xx = (base_match[2].str()[1] == 'x' || base_match[2].str()[2] == 'x' ? stol(base_match[2].str(), nullptr, 16) : stol(base_match[2].str()));
		new_inst = new instructionMOVImm(addr, xx, stol(base_match[1].str()));
	} else if(regex_match (line, base_match, ldr_reg_regex) && base_match.size() == 4 ) {

		if( base_match[1].str() == "ldr" ) {
			new_inst = new instructionLDR(addr, stol(base_match[3].str()), stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTR(addr, stol(base_match[3].str()), stol(base_match[2].str()));
		}

	} else if(regex_match (line, base_match, ldr_off_regex) && base_match.size() == 7 ) {

		long int xx = (base_match[4].str()[1] == 'x' || base_match[4].str()[2] == 'x' ? stol(base_match[4].str(), nullptr, 16) : stol(base_match[4].str()));

		if( base_match[1].str() == "ldr" ) {
			new_inst = new instructionLDROff(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTROff(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}

	} else if(regex_match (line, base_match, ldr_pre_regex) && base_match.size() == 8 ) {

		long int xx = (base_match[4].str()[1] == 'x' || base_match[4].str()[2] == 'x' ? stol(base_match[4].str(), nullptr, 16) : stol(base_match[4].str()));

		if( base_match[1].str() == "ldr" ) {
			new_inst = new instructionLDRPre(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTRPre(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}
	} else if(regex_match (line, base_match, ldr_post_regex) && base_match.size() == 7 ) {
		long int xx = (base_match[4].str()[1] == 'x' || base_match[4].str()[2] == 'x' ? stol(base_match[4].str(), nullptr, 16) : stol(base_match[4].str()));
		if( base_match[1].str() == "ldr" ) {
			new_inst = new instructionLDRPost(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTRPost(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}
	} else if (regex_match (line, base_match, addsub_regex) && base_match.size() == 5 ) {
		if( base_match[1].str() == "add" )
			new_inst = new instructionADD(addr, stol(base_match[3].str()), stol(base_match[4].str()), stol(base_match[2].str()));
		if( base_match[1].str() == "sub" )
			new_inst = new instructionSUB(addr, stol(base_match[3].str()), stol(base_match[4].str()), stol(base_match[2].str()));
	} else if (regex_match (line, base_match, addimm_regex) && base_match.size() == 7 ) {
		long int xx = (base_match[4].str()[1] == 'x' ? stol(base_match[6].str(), nullptr, 16) : stol(base_match[4].str()));
		if( base_match[1].str() == "add" ) {
			new_inst = new instructionADDImm(addr, stol(base_match[3].str()), xx, stol(base_match[4].str()));
		} else {if( base_match[1].str() == "sub" )
			new_inst = new instructionSUBImm(addr, stol(base_match[3].str()), xx, stol(base_match[4].str()));
		}
	} else if (regex_match (line, base_match, mul_regex) && base_match.size() == 4 ) {
		new_inst = new instructionMUL(addr, stol(base_match[2].str()), stol(base_match[3].str()), stol(base_match[1].str()));
	} else if (regex_match (line, base_match, mla_regex) && base_match.size() == 5 ) {
		new_inst = new instructionMLA(addr, stol(base_match[2].str()),
				stol(base_match[3].str()), stol(base_match[4].str()),
				stol(base_match[1].str()));
	} else if (regex_match (line, base_match, branchx_regex) && base_match.size() == 3 ) {

//		cout << line << " : base_match.size() = " << base_match.size() << endl;
//		for (unsigned int i = 0; i < base_match.size(); ++i) {
//			cout << "base_match[" << i << "].str() = " << base_match[i].str() << endl;
//		}

		if( base_match[1].str() == "bx" ) {
			new_inst = new instructionBRX(addr, stol(base_match[2].str()));
		} else if( base_match[1].str() == "blx" ) {
			new_inst = new instructionBLX(addr, stol(base_match[2].str()));
		}
	} else if (regex_match (line, base_match, branch_regex) && base_match.size() == 3 ) {
		if( base_match[1].str() == "b" ) {
			new_inst = new instructionBR(addr, base_match[2].str());
		} else if( base_match[1].str() == "bl" ) {
			string s = base_match[2].str();
			new_inst = new instructionBRLink(addr, s);
		}
	}
	return new_inst;
}
