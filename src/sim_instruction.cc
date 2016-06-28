/*
 * instruction.cc
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#include <cstdlib>
#include <regex>

#include "sim_instruction.hh"
#include "sim.hh"
#include "sim_stats.hh"

extern sim_stats simulator_stats;

extern int debug_level;

instruction::instruction() {

	this->inst_class = instClasses::Invalid;
	this->num_sources = 0;
	this->num_dests = 0;

	for (int i = 0; i < 4; ++i) {
		this->sourcesTypes[i] = instSources::Invalid;
		this->sources_idx[i] = 0;
		this->destsTypes[i] = instDest::Invalid;
		this->dests_idx[i] = 0;
		this->sources_forward[i] = false;
	}
	this->memory_pos = 0;
	this->tag = "";
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

void instruction::commit()
{}

void instruction::print(ostream& where) const {
	where << "meh!";
}

void instruction::update_stats()
{
	simulator_stats.instructions_total++;
	simulator_stats.instructions_by_type[static_cast< std::size_t >( this->inst_class )]++;
}

bool instruction::depends(instruction* b)
{
	for(int i = 0; i < this->num_sources; i++) {
		if( this->sourcesTypes[i] != instSources::REGISTER )
			continue;
		for(int j = 0; j < b->num_dests; j++) {
			if( b->destsTypes[j] != instDest::REGISTER )
				continue;
			if( this->sources_idx[i] == b->dests_idx[j] )
				return true;
		}

	}
	return false;
}

void instruction::forward_clear()
{
	for (unsigned int i = 0; i < (sizeof(this->sources_forward)/sizeof(this->sources_forward[0])); ++i) {
		this->sources_forward[i] = false;
	}
}

instructionNOP::instructionNOP() {
	this->inst_class = instClasses::NOP;
}

instructionNOP::instructionNOP(unsigned long int addr):
	instructionNOP()
{
	this->memory_pos = addr;
}

void instructionNOP::execute() {
}

void instructionNOP::print(ostream& where) const {
	where << "NOP";
}

instructionClassARITH::instructionClassARITH():instruction()
{
	this->inst_class = instClasses::ARITH;
}

instructionClassMULT::instructionClassMULT():instruction()
{
	this->inst_class = instClasses::MUL;
}

instructionClassMOV::instructionClassMOV():instruction()
{
	this->inst_class = instClasses::MOV;
}

instructionClassMEM::instructionClassMEM():instruction()
{
	this->inst_class = instClasses::MEM;
}

instructionClassCTRL::instructionClassCTRL():instruction()
{
	this->inst_class = instClasses::CTRL;
}

instructionADD::instructionADD():instructionClassARITH() {
}

instructionADD::instructionADD(unsigned long int addr, long int s1, long int s2, long int d):instructionClassARITH()
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
	where << "ADD r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", r" << this->sources_idx[1];
}

instructionADDImm::instructionADDImm():instructionADD()
{}

instructionADDImm::instructionADDImm(unsigned long int addr, long int s1, long int imm,
		long int d):instructionADD() {
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->sources_values[1] = imm;
	this->memory_pos = addr;
}

void instructionADDImm::print(ostream& where) const {
	where << "ADDImm r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", #" << this->sources_values[1];
}

instructionSUB::instructionSUB()
{
	this->inst_class = instClasses::ARITH;
}

instructionSUB::instructionSUB(unsigned long int addr, long int s1, long int s2, long int d)
{
	this->inst_class = instClasses::ARITH;
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
	this->inst_class = instClasses::ARITH;
}

instructionSUBImm::instructionSUBImm(unsigned long int addr, long int s1, long int imm,
		long int d) {
	this->inst_class = instClasses::ARITH;
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;
	this->sources_values[1] = imm;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionSUBImm::print(ostream& where) const {
	where << "SUBImm r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", #" << this->sources_values[1];
}

instructionMUL::instructionMUL():instructionClassMULT()
{
	this->inst_class = instClasses::MUL;
}

instructionMUL::instructionMUL(unsigned long int addr, long int s1, long int s2, long int d):
		instructionClassMULT()
{
	this->inst_class = instClasses::MUL;
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

void instructionMUL::execute()
{
	destination_values[0] = sources_values[0]*sources_values[1];
}

instructionMULImm::instructionMULImm():instructionClassMULT()
{}

instructionMULImm::instructionMULImm(unsigned long int addr, long int s1,
		long int imm, long int d):
	instructionClassMULT()
{
	this->inst_class = instClasses::MUL;
	this->num_sources = 2;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;

	this->num_dests = 1;
	this->sources_idx[0] = s1;
	this->sources_values[1] = imm;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;

	this->memory_pos = addr;
}

void instructionMULImm::print(ostream& where) const
{
	where << "MUL r" << this->dests_idx[0] << ", r" << this->sources_idx[0] << ", #" << this->sources_values[1];
}

void instructionMULImm::execute()
{
	destination_values[0] = sources_values[0]*sources_values[1];
}

instructionMLA::instructionMLA():instructionClassMULT()
{}

instructionMLA::instructionMLA(unsigned long int addr, long int s1, long int s2, long int s3, long int d):
		instructionClassMULT()
{
	this->inst_class = instClasses::MUL;
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

void instructionMLA::execute()
{
	this->destination_values[0] = this->sources_values[0]+
			this->sources_values[1]*this->sources_values[2];
}

instructionMLAImm::instructionMLAImm()
{}

instructionMLAImm::instructionMLAImm(unsigned long int addr, long int s1,
		long int s2, long int imm, long int d):
		instructionMLA(addr, s1, s2, imm, d)
{
	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_idx[2] = 0;
	this->sources_values[2] = imm;
}

void instructionMLAImm::print(ostream& where) const
{
	where << "MLA r" << this->dests_idx[0]
				<< ", r" << this->sources_idx[0]
				<< ", r" << this->sources_idx[1]
				<< ", #" << this->sources_values[2];
}

void instructionMLAImm::execute()
{
	this->destination_values[0] = this->sources_values[0]+
			this->sources_values[1]*this->sources_values[2];
}

instructionMOV::instructionMOV():instructionClassMOV()
{}

instructionMOV::instructionMOV(unsigned long int addr, long int s1, long int d):
		instructionClassMOV()
{
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;

	this->sources_idx[0] = s1;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMOV::execute()
{
	this->destination_values[0] = this->sources_values[0];
}

void instructionMOV::print(ostream& where) const {
	where << "MOV r" << this->dests_idx[0] << ", r" << this->sources_idx[0];
}

instructionMOVImm::instructionMOVImm():instructionMOV()
{}

instructionMOVImm::instructionMOVImm(unsigned long int addr, long int imm, long int d):
		instructionMOV()
{
	this->num_sources = 1;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::IMMEDIATE;
	this->sources_values[0] = imm;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionMOVImm::execute()
{
	this->destination_values[0] = this->sources_values[0];
}

void instructionMOVImm::print(ostream& where) const {
	where << "MOVImm r" << this->dests_idx[0] << ", #" << this->sources_values[0];
}

instructionLDR::instructionLDR():instructionClassMEM()
{
}

instructionLDR::instructionLDR(unsigned long int addr, long int s1, long int d):
		instructionClassMEM()
{
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->sourcesTypes[0] = instSources::MEMORY;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;

	this->memory_pos = addr;
}

void instructionLDR::execute()
{
	this->sources_idx[0] = this->sources_values[1];
}

void instructionLDR::print(ostream& where) const {
	where << "LDR r" << this->dests_idx[0] << ", [r" << this->sources_idx[1] << "]";
}

instructionLDROffReg::instructionLDROffReg():instructionClassMEM()
{
}

instructionLDROffReg::instructionLDROffReg(unsigned long int addr, long int s1,
		long int s2, long int d):
	instructionClassMEM()
{
	this->num_sources = 3;
	this->num_dests = 1;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;
	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_idx[2] = s2;

	this->sourcesTypes[0] = instSources::MEMORY;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionLDROffReg::print(ostream& where) const
{
	where << "LDRoff r" << this->dests_idx[0] << ", [r" << this->sources_idx[1] << ", r" << this->sources_idx[2] << "]";
}

void instructionLDROffReg::execute()
{
	this->sources_idx[0] = this->sources_values[1]+this->sources_values[2];
}

instructionLDROffImm::instructionLDROffImm():instructionClassMEM()
{}

instructionLDROffImm::instructionLDROffImm(unsigned long int addr, long int s1, long int imm, long int d):
		instructionClassMEM()
{
	this->num_sources = 3;
	this->num_dests = 1;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_values[2] = imm;

	this->sources_idx[1] = s1;

	this->sourcesTypes[0] = instSources::MEMORY;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;
	this->memory_pos = addr;
}

void instructionLDROffImm::print(ostream& where) const {
	where << "LDRoff r" << this->dests_idx[0] << ", [r" << this->sources_idx[1] << ", #" << this->sources_values[2] << "]";
}

void instructionLDROffImm::execute()
{
	this->sources_idx[0] = this->sources_values[1]+this->sources_values[2];
}

instructionLDRPreImm::instructionLDRPreImm():instructionLDROffImm()
{}

instructionLDRPreImm::instructionLDRPreImm(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionLDROffImm(addr, s1, imm, d)
{
	this->num_sources = 3;
	this->num_dests = 2;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_values[2] = imm;

	this->sources_idx[1] = s1;

	this->sourcesTypes[0] = instSources::MEMORY;
	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;

	this->destsTypes[1] = instDest::REGISTER;
	this->dests_idx[1] = s1;

	this->memory_pos = addr;
}

void instructionLDRPreImm::print(ostream& where) const {
	where << "LDRpre r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->sources_values[1] << "] !";
}

void instructionLDRPreImm::execute()
{
	this->sources_idx[0] = this->sources_values[1]+this->sources_values[2];
	this->destination_values[1] = this->sources_idx[0];
}

instructionLDRPostReg::instructionLDRPostReg():instructionLDROffReg()
{}

instructionLDRPostReg::instructionLDRPostReg(unsigned long int addr,
		long int s1, long int s2, long int d):
	instructionLDROffReg()
{
	this->num_sources = 3;
	this->num_dests = 2;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_idx[2] = s2;

	this->sourcesTypes[0] = instSources::MEMORY;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;

	this->destsTypes[1] = instDest::REGISTER;
	this->dests_idx[1] = s1;

	this->memory_pos = addr;
}

void instructionLDRPostReg::print(ostream& where) const
{
	where << "LDRpost r" << this->dests_idx[0] <<
			", [r" << this->sources_idx[1] << "], r" <<
			this->sources_idx[2];
}

void instructionLDRPostReg::execute()
{
	this->sources_idx[0] = this->sources_values[1];
	this->destination_values[1] = this->sources_values[1]+this->sources_values[2];
}

instructionLDRPostImm::instructionLDRPostImm():instructionLDROffImm()
{}

instructionLDRPostImm::instructionLDRPostImm(unsigned long int addr, long int s1, long int imm, long int d):
	instructionLDROffImm(addr, s1, imm, d)
{
	this->num_sources = 3;
	this->num_dests = 2;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_values[2] = imm;

	this->sourcesTypes[0] = instSources::MEMORY;

	this->destsTypes[0] = instDest::REGISTER;
	this->dests_idx[0] = d;

	this->destsTypes[1] = instDest::REGISTER;
	this->dests_idx[1] = s1;

	this->memory_pos = addr;
}

void instructionLDRPostImm::print(ostream& where) const {
	where << "LDRpost r" << this->dests_idx[0] <<
			", [r" << this->sources_idx[1] <<
			"], #" << this->sources_values[2];
}

void instructionLDRPostImm::execute()
{
	this->sources_idx[0] = this->sources_values[1];
	this->destination_values[1] = this->sources_values[1]+this->sources_values[2];
}

instructionSTR::instructionSTR():instructionClassMEM()
{}

instructionSTR::instructionSTR(unsigned long int addr, long int s1, long int d):
		instructionSTR()
{
	this->num_sources = 2;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sources_idx[0] = d;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->destsTypes[0] = instDest::MEMORY;

	this->memory_pos = addr;
}

void instructionSTR::execute()
{
	this->dests_idx[0] = this->sources_values[1];
	this->destination_values[0] = this->sources_values[0];
}

void instructionSTR::print(ostream& where) const {
	where << "STR r" << this->sources_idx[0] << ", [r" << this->sources_idx[1] << "]";
}

instructionSTROff::instructionSTROff():instructionClassMEM()
{}

instructionSTROff::instructionSTROff(unsigned long int addr, long int s1, long int imm, long int d):
		instructionClassMEM()
{
	this->num_sources = 3;
	this->num_dests = 1;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sources_idx[0] = d;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_values[2] = imm;

	this->destsTypes[0] = instDest::MEMORY;

	this->memory_pos = addr;
}

void instructionSTROff::print(ostream& where) const {
	where << "STRoff r" << this->sources_idx[0] << ", [r" << this->sources_idx[1] << ", #" << this->sources_values[2] << "]";
}

void instructionSTROff::execute()
{
	this->destination_values[0] = this->sources_values[0];
	this->dests_idx[0] = this->sources_values[1]+this->sources_values[2];
}

instructionSTRPre::instructionSTRPre():instructionSTROff()
{}

instructionSTRPre::instructionSTRPre(unsigned long int addr, long int s1, long int imm, long int d):
	instructionSTROff(addr, s1, imm, d)
{}

void instructionSTRPre::print(ostream& where) const {
	where << "STRpre r" << this->dests_idx[0] << ", [r" << this->sources_idx[0] << ", #" << this->sources_values[1] << "] !";
}

instructionSTRPostImm::instructionSTRPostImm():instructionSTROff()
{}

instructionSTRPostImm::instructionSTRPostImm(unsigned long int addr, long int s1, long int imm, long int d)
	:instructionSTROff(addr, s1, imm, d)
{
	this->num_sources = 3;
	this->num_dests = 2;

	this->sourcesTypes[0] = instSources::REGISTER;
	this->sources_idx[0] = d;

	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[1] = s1;

	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_values[2] = imm;

	this->destsTypes[0] = instDest::MEMORY;

	this->destsTypes[1] = instDest::REGISTER;
	this->dests_idx[1] = s1;

	this->memory_pos = addr;
}

void instructionSTRPostImm::print(ostream& where) const {
	where << "STRpost r" << this->sources_idx[0] <<
			", [r" << this->sources_idx[1] <<
			"], #" << this->sources_values[2];
}

void instructionSTRPostImm::execute()
{
	this->dests_idx[0] = this->sources_values[1];
	this->destination_values[0] = this->sources_values[0];
	this->destination_values[1] = this->sources_values[1]+this->sources_values[2];
}

instructionBR::instructionBR():instructionClassCTRL()
{}

instructionBR::instructionBR(unsigned long int addr, string mem_tag):instructionClassCTRL()
{
	this->num_sources = 1;
	this->sourcesTypes[0] = instSources::IMMEDIATE;
	this->destsTypes[0] = instDest::BRANCH;
	this->tag = mem_tag;
	this->memory_pos = addr;
}

void instructionBR::print(ostream& where) const {
	where << "B " << this->tag << " ( " << this->tag << " )";
}

instructionBRX::instructionBRX():instructionClassCTRL()
{}

instructionBRX::instructionBRX(unsigned long int addr, long int s1):
		instructionClassCTRL()
{
	this->num_sources = 1;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sources_idx[0] = s1;
	this->memory_pos = addr;
	this->destsTypes[0] = instDest::BRANCH;
}

void instructionBRX::print(ostream& where) const {
	where << "BrX r" << this->sources_idx[0];
}

instructionBRLink::instructionBRLink():instructionBR()
{}

instructionBRLink::instructionBRLink(unsigned long int addr, string mem_tag):instructionBR(addr, mem_tag) {
	this->tag = mem_tag;
}

void instructionBRLink::print(ostream& where) const {
	where << "BLr " << this->tag;
}

instructionBLX::instructionBLX():instructionBRX()
{}

instructionBLX::instructionBLX(unsigned long int addr, long int s1): instructionBRX(addr, s1) {
}

void instructionBLX::print(ostream& where) const {
	where << "BLX r" << this->sources_idx[0];
}

instructionBRConditionalClass::instructionBRConditionalClass():instructionClassCTRL()
{
	this->is_equal = false;
	this->should_jump = false;
}

void instructionBRConditionalClass::execute()
{
	if( this->is_equal == true &&
	    this->sources_values[1] == this->sources_values[2] )
		this->should_jump = true;
	else if( this->is_equal == false &&
	    this->sources_values[1] != this->sources_values[2] )
		this->should_jump = true;
	else
		this->should_jump = false;
}

instructionBRConditional::instructionBRConditional():
	instructionBRConditionalClass()
{}

instructionBRConditional::instructionBRConditional(unsigned long int addr,
		unsigned long int reg1, unsigned long int reg2, bool condition,
		string mem_tag):
	instructionBRConditionalClass()
{
	this->num_sources = 3;
	this->sourcesTypes[0] = instSources::IMMEDIATE;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_idx[1] = reg1;
	this->sources_idx[2] = reg2;
	this->memory_pos = addr;
	this->destsTypes[0] = instDest::BRANCH_CONDITIONAL;
	this->tag = mem_tag;
	this->is_equal = condition;
}

void instructionBRConditional::print(ostream& where) const
{
	if( this->is_equal == true )
		where << "BEq";
	else
		where << "BNEq";
	where << " r" << this->sources_idx[1];
	where << ", r" << this->sources_idx[2];
	where << ", " << this->tag;
}

instructionBRImmCond::instructionBRImmCond():
	instructionBRConditionalClass()
{}

instructionBRImmCond::instructionBRImmCond(unsigned long int addr,
		unsigned long int reg1, long int imm, bool condition,
		string mem_tag):
	instructionBRConditionalClass()
{
	this->num_sources = 3;
	this->sourcesTypes[0] = instSources::IMMEDIATE;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::IMMEDIATE;
	this->sources_idx[1] = reg1;
	this->sources_values[2] = imm;
	this->memory_pos = addr;
	this->destsTypes[0] = instDest::BRANCH_CONDITIONAL;
	this->tag = mem_tag;
	this->is_equal = condition;
}

void instructionBRImmCond::print(ostream& where) const
{
	if( this->is_equal == true )
		where << "BEq";
	else
		where << "BNEq";
	where << " r" << this->sources_idx[1];
	where << ", #" << this->sources_values[2];
	where << ", " << this->tag;
}

instructionBRXConditional::instructionBRXConditional():
	instructionBRConditionalClass()
{}

instructionBRXConditional::instructionBRXConditional(unsigned long int addr,
		unsigned long int reg1, unsigned long int reg2, bool condition,
		unsigned long int reg3):
	instructionBRConditionalClass()
{
	this->num_sources = 3;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_idx[0] = reg1;
	this->sources_idx[1] = reg2;
	this->sources_idx[2] = reg3;
	this->memory_pos = addr;
	this->destsTypes[0] = instDest::BRANCH_CONDITIONAL;
	this->tag = "";
	this->is_equal = condition;
}

void instructionBRXConditional::print(ostream& where) const
{
	if( this->is_equal == true )
		where << "BXEq";
	else
		where << "BXNEq";
	where << " r" << this->sources_idx[0];
	where << ", r" << this->sources_idx[1];
	where << ", r" << this->sources_idx[2];
}

instructionBRXImmCond::instructionBRXImmCond():
	instructionBRConditionalClass()
{}

instructionBRXImmCond::instructionBRXImmCond(unsigned long int addr,
		unsigned long int reg1, long int imm, bool condition,
		unsigned long int reg3):
	instructionBRConditionalClass()
{
	this->num_sources = 3;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::IMMEDIATE;
	this->sourcesTypes[2] = instSources::REGISTER;
	this->sources_idx[0] = reg1;
	this->sources_idx[1] = imm;
	this->sources_idx[2] = reg3;
	this->memory_pos = addr;
	this->destsTypes[0] = instDest::BRANCH_CONDITIONAL;
	this->tag = "";
	this->is_equal = condition;
}

void instructionBRXImmCond::print(ostream& where) const
{
	if( this->is_equal == true )
		where << "BXEq";
	else
		where << "BXNEq";
	where << " r" << this->sources_idx[0];
	where << ", #" << this->sources_idx[1];
	where << ", r" << this->sources_idx[2];
}

instructionBRLConditional::instructionBRLConditional()
{}

instructionBRLConditional::instructionBRLConditional(unsigned long int addr,
		unsigned long int reg1, unsigned long int reg2, bool condition,
		string mem_tag):
	instructionBRConditionalClass()
{
	this->num_sources = 2;
	this->sourcesTypes[0] = instSources::REGISTER;
	this->sourcesTypes[1] = instSources::REGISTER;
	this->sources_idx[0] = reg1;
	this->sources_idx[1] = reg2;
	this->memory_pos = addr;
	this->num_dests = 2;
	this->destsTypes[0] = instDest::BRANCH_CONDITIONAL;
	this->destsTypes[1] = instDest::REGISTER;
	this->dests_idx[1] = 31;
	this->destination_values[1] = addr+4;
	this->tag = mem_tag;
	this->is_equal = condition;
}

void instructionBRLConditional::print(ostream& where) const
{
	if( this->is_equal == true )
		where << "BLEq";
	else
		where << "BLNEq";
	where << " r" << this->sources_idx[0];
	where << ", r" << this->sources_idx[1];
	where << ", " << this->tag;
}

instructionEND::instructionEND()
{}

void instructionEND::print(ostream& where) const
{
	where << "This is the END!";
}

void instructionEND::commit()
{
	throw exception_simulator_stop();
}

void instructionEND::update_stats()
{}

inline long int get_immediate(string str)
{
	return (str[1] == 'x' || str[2] == 'x' ? stol(str, nullptr, 16) : stol(str));
}

instruction* instructionFactory::buildInstruction(unsigned long int addr, string line) {

	instruction *new_inst = NULL;

	smatch base_match;

	regex nop_regex("^[ \t]*nop[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex end_regex("^[ \t]*END[ \t]*$", std::regex_constants::extended);

	regex movimm_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*#(([-0-9])+|([-0]+x[0-9a-fA-F]+))", std::regex_constants::extended | std::regex_constants::icase);
	regex mov_regex("^[ \t]*mov[ \t]+r([0-9]+),[ \t]*r([0-9]+)", std::regex_constants::extended | std::regex_constants::icase);

	regex ldr_reg_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\][ \t]*$", std::regex_constants::extended | std::regex_constants::icase | std::regex_constants::icase);
	regex ldr_off_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-fA-F]+))\][ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex ldr_pre_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+),[ \t]*#(([-0-9]+)|([-0]+x[0-9a-fA-F]+))\]( !|!)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex ldr_post_regex("^[ \t]*(ldr|str)[ \t]*r([0-9]+),[ \t]*\\[r([0-9]+)\],[ \t]*(r([0-9]+)|#(([-0-9])+|([-0]+x[0-9a-fA-F]+)))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex addsub_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+)[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex addimm_regex("^[ \t]*(add|sub)[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*#(([0-9]+)|0x([0-9a-fA-F]+))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex mul_regex("^[ \t]*mul[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*(r([0-9]+)|#(([-0-9])+|([-0]+x[0-9a-fA-F]+)))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);
	regex mla_regex("^[ \t]*mla[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*r([0-9]+),[ \t]*(r([0-9]+)|#(([-0-9])+|([-0]+x[0-9a-fA-F]+)))[ \t]*$", std::regex_constants::extended | std::regex_constants::icase);

	regex branch_regex("^[ \t]*(b|bl)[ \t]*([_0-9a-zA-Z]+)$", std::regex_constants::extended | std::regex_constants::icase);
	regex branchx_regex("^[ \t]*(bx|blx)[ \t]*r([0-9]+)$", std::regex_constants::extended | std::regex_constants::icase);

	regex brancheq_regex("^[ \t]*(beq|bleq|bneq|blneq)[ \t]*r([0-9]+),[ \t]*(r([0-9]+)|#(([-0-9])+|([-0]+x[0-9a-fA-F]+))),[ \t]*([_0-9a-zA-Z]+)$",
		std::regex_constants::extended | std::regex_constants::icase);
	regex branchxeq_regex("^[ \t]*(bxeq|bxneq|blxeq|blxneq)[ \t]*r([0-9]+),[ \t]*(r([0-9]+)|#(([-0-9])+|([-0]+x[0-9a-fA-F]+))),[ \t]*r([0-9]+)$",
		std::regex_constants::extended | std::regex_constants::icase);

	if( regex_match(line, base_match, nop_regex) /*&& base_match.size() == 3 */ ) {
		new_inst = new instructionNOP(addr);
	} else if( regex_match(line, base_match, mov_regex) && base_match.size() == 3 ) {

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
			new_inst = new instructionLDROffImm(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTROff(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}

	} else if(regex_match (line, base_match, ldr_pre_regex) && base_match.size() == 8 ) {

		long int xx = (base_match[4].str()[1] == 'x' || base_match[4].str()[2] == 'x' ? stol(base_match[4].str(), nullptr, 16) : stol(base_match[4].str()));

		if( base_match[1].str() == "ldr" ) {
			new_inst = new instructionLDRPreImm(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else if( base_match[1].str() == "str" ) {
			new_inst = new instructionSTRPre(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}
	} else if(regex_match (line, base_match, ldr_post_regex) && base_match.size() == 9 ) {

		char ch = base_match[4].str()[0];

		if( base_match[1].str() == "ldr" ) {
			if( (ch == 'r' || ch == 'R') && ch != '#' ) {
				new_inst = new instructionLDRPostReg(addr, stol(base_match[3].str()), stol(base_match[5].str()), stol(base_match[2].str()));
			} else if( ch != 'r' && ch != 'R' && ch == '#' ) {
				long int xx = get_immediate(base_match[6].str());
				new_inst = new instructionLDRPostImm(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
			}
		} else if( base_match[1].str() == "str" ) {
			if( (ch == 'r' || ch == 'R') && ch != '#' ) {
//				new_inst = new instructionSTRPostReg(addr, stol(base_match[3].str()), stol(base_match[5].str()), stol(base_match[2].str()));
			} else if( ch != 'r' && ch != 'R' && ch == '#' ) {
				long int xx = get_immediate(base_match[6].str());
				new_inst = new instructionSTRPostImm(addr, stol(base_match[3].str()),xx, stol(base_match[2].str()));
			}
		}
	} else if (regex_match (line, base_match, addsub_regex) && base_match.size() == 5 ) {
		if( base_match[1].str() == "add" )
			new_inst = new instructionADD(addr, stol(base_match[3].str()), stol(base_match[4].str()), stol(base_match[2].str()));
		if( base_match[1].str() == "sub" )
			new_inst = new instructionSUB(addr, stol(base_match[3].str()), stol(base_match[4].str()), stol(base_match[2].str()));
	} else if (regex_match (line, base_match, addimm_regex) && base_match.size() == 7 ) {
		long int xx = get_immediate(base_match[4].str());
		if( base_match[1].str() == "add" ) {
			new_inst = new instructionADDImm(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		} else {if( base_match[1].str() == "sub" )
			new_inst = new instructionSUBImm(addr, stol(base_match[3].str()), xx, stol(base_match[2].str()));
		}
	} else if (regex_match (line, base_match, mul_regex) && base_match.size() == 8 ) {
		char ch = base_match[3].str()[0];
		if( (ch == 'r' || ch == 'R') && ch != '#' )
			new_inst = new instructionMUL(addr, stol(base_match[2].str()), stol(base_match[4].str()), stol(base_match[1].str()));
		else if( ch != 'r' && ch != 'R' && ch == '#' ) {
			long int xx = get_immediate(base_match[5].str());
			new_inst = new instructionMULImm(addr, stol(base_match[2].str()), xx, stol(base_match[1].str()));
		}
	} else if (regex_match (line, base_match, mla_regex) && base_match.size() == 9 ) {
		char ch = base_match[4].str()[0];
		if( (ch == 'r' || ch == 'R') && ch != '#' )
			new_inst = new instructionMLA(addr, stol(base_match[2].str()), stol(base_match[3].str()), stol(base_match[5].str()), stol(base_match[1].str()));
		else if( ch != 'r' && ch != 'R' && ch == '#' ) {
			string val = base_match[6].str();
			long int xx = (val[1] == 'x' || val[2] == 'x' ? stol(val, nullptr, 16) : stol(val));
			new_inst = new instructionMLAImm(addr, stol(base_match[2].str()), stol(base_match[3].str()), xx, stol(base_match[1].str()));
		}
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
	} else if (regex_match (line, base_match, brancheq_regex) && base_match.size() == 9 ) {

		bool cond = true;

		if( base_match[1].str().find("neq") != std::string::npos )
			cond = false;

		if( base_match[3].str()[0] == '#' ) {
			long int xx = (base_match[5].str()[1] == 'x' || base_match[5].str()[2] == 'x' ? stol(base_match[5].str(), nullptr, 16) : stol(base_match[5].str()));
			new_inst = new instructionBRImmCond(addr, stol(base_match[2].str()), xx, cond, base_match[8].str());
		} else {
			new_inst = new instructionBRConditional(addr, stol(base_match[2].str()), stol(base_match[4].str()), cond, base_match[8].str());
		}

	} else if (regex_match (line, base_match, branchxeq_regex) && base_match.size() == 9 ) {

		bool cond = false;

		if( base_match[1].str().find("neq") == std::string::npos )
			cond = true;

		if( base_match[3].str()[0] == '#' ) {
			long int xx = (base_match[5].str()[1] == 'x' || base_match[5].str()[2] == 'x' ? stol(base_match[5].str(), nullptr, 16) : stol(base_match[5].str()));
			new_inst = new instructionBRXImmCond(addr, stol(base_match[2].str()), xx, cond, stol(base_match[8].str()));
		} else {
			new_inst = new instructionBRXConditional(addr, stol(base_match[2].str()), stol(base_match[4].str()), cond, stol(base_match[8].str()));
		}
	} else if( regex_match(line, base_match, end_regex)) {
		new_inst = new instructionEND();
	}

	return new_inst;
}
