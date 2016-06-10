/*
 * instruction.hh
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#ifndef SRC_INSTRUCTION_HH_
#define SRC_INSTRUCTION_HH_

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

enum class instSources {
	Invalid,
	REGISTER,
	IMMEDIATE,
	MEMORY,
	maxInstSources
};

enum class instDest {
	Invalid,
	REGISTER,
	MEMORY,
	BRANCH,
	BRANCH_CONDITIONAL,
	maxInstDest
};

enum class instClasses {
	Invalid,
	First = 1,
	NOP = First,
	ARITH,
	MUL,
	MOV,
	MEM,
	CTRL,
	maxInstClasses
};

class instruction {
public:
	instruction();
	~instruction();
	virtual void execute();
	virtual void commit();
	virtual void update_stats();

	friend ostream& operator<<(ostream& os, const instruction& inst);
	virtual void print(ostream& where) const;

	bool depends(instruction *b);

public:
	instClasses inst_class;
	int num_sources;
	instSources sourcesTypes[4];
	long int sources_idx[4];
	int num_dests;
	instDest destsTypes[4];
	long int dests_idx[4];
	unsigned long int memory_pos;
	long int sources_values[4];
	long int destination_values[4];
	string tag;
	bool is_dud; /**< Instruction does nothing */
};

class instructionNOP:public instruction {
public:
	instructionNOP();
	instructionNOP(unsigned long int addr);
	void execute();
	void print(ostream& where) const;
};

class instructionClassARITH:public instruction {
public:
	instructionClassARITH();
};

class instructionClassMULT:public instruction {
public:
	instructionClassMULT();
};

class instructionClassMOV:public instruction {
public:
	instructionClassMOV();
};

class instructionClassMEM:public instruction {
public:
	instructionClassMEM();
};


class instructionClassCTRL:public instruction {
public:
	instructionClassCTRL();
};

class instructionADD:public instructionClassARITH {
public:
	instructionADD();
	instructionADD(unsigned long int addr, long int s1, long int s2, long int d);
	void execute();
	void print(ostream& where) const;
};

class instructionADDImm:public instructionADD {
public:
	instructionADDImm();
	instructionADDImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionSUB: public instructionClassARITH {
public:
	instructionSUB();
	instructionSUB(unsigned long int addr, long int s1, long int s2, long int d);
	void print(ostream& where) const;
};

class instructionSUBImm:public instructionSUB {
public:
	instructionSUBImm();
	instructionSUBImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionMUL: public instructionClassMULT {
public:
	instructionMUL();
	instructionMUL(unsigned long int addr, long int s1, long int s2, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionMULImm: public instructionClassMULT {
public:
	instructionMULImm();
	instructionMULImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionMLA: public instructionClassMULT {
public:
	instructionMLA();
	instructionMLA(unsigned long int addr, long int s1, long int s2, long int s3, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionMLAImm: public instructionMLA {
public:
	instructionMLAImm();
	instructionMLAImm(unsigned long int addr, long int s1, long int s2, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionMOV:public instructionClassMOV {
public:
	instructionMOV();
	instructionMOV(unsigned long int addr, long int s1, long int d);
	void execute();
	void print(ostream& where) const;
};

class instructionMOVImm:public instructionMOV {
public:
	instructionMOVImm();
	instructionMOVImm(unsigned long int addr, long int imm, long int d);
	void execute();
	void print(ostream& where) const;
};

class instructionLDR:public instructionClassMEM {
public:
	instructionLDR();
	instructionLDR(unsigned long int addr, long int s1, long int d);
	void execute();
	void print(ostream& where) const;
};

class instructionLDROffReg:public instructionClassMEM {
public:
	instructionLDROffReg();
	instructionLDROffReg(unsigned long int addr, long int s1, long int s2, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionLDROffImm:public instructionClassMEM {
public:
	instructionLDROffImm();
	instructionLDROffImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionLDRPreImm:public instructionLDROffImm {
public:
	instructionLDRPreImm();
	instructionLDRPreImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionLDRPostReg:public instructionLDROffReg {
public:
	instructionLDRPostReg();
	instructionLDRPostReg(unsigned long int addr, long int s1, long int s2, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionLDRPostImm:public instructionLDROffImm {
public:
	instructionLDRPostImm();
	instructionLDRPostImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionSTR:public instructionClassMEM {
public:
	instructionSTR();
	instructionSTR(unsigned long int addr, long int s1, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionSTROff:public instructionClassMEM {
public:
	instructionSTROff();
	instructionSTROff(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionSTRPre:public instructionSTROff {
public:
	instructionSTRPre();
	instructionSTRPre(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionSTRPostImm:public instructionSTROff {
public:
	instructionSTRPostImm();
	instructionSTRPostImm(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	void execute();
};

class instructionBR:public instructionClassCTRL {
public:
	instructionBR();
	instructionBR(unsigned long int addr, string mem_tag);
	void print(ostream& where) const;
};

class instructionBRLink:public instructionBR {
public:
	instructionBRLink();
	instructionBRLink(unsigned long int addr, string mem_tag);
	void print(ostream& where) const;
};

class instructionBRX:public instructionClassCTRL {
public:
	instructionBRX();
	instructionBRX(unsigned long int addr, long int s1);
	void print(ostream& where) const;
};

class instructionBLX:public instructionBRX {
public:
	instructionBLX();
	instructionBLX(unsigned long int addr, long int s1);
	void print(ostream& where) const;
};

class instructionBRConditionalClass:public instructionClassCTRL {
public:
	instructionBRConditionalClass();

public:
	bool is_equal;
	bool should_jump;
};

class instructionBRConditional:public instructionBRConditionalClass {
public:
	instructionBRConditional();
	instructionBRConditional(unsigned long int addr, unsigned long int reg1, unsigned long int reg2, bool condition, string mem_tag);
	void print(ostream& where) const;
	void execute();
};

class instructionBRLConditional:public instructionBRConditionalClass {
public:
	instructionBRLConditional();
	instructionBRLConditional(unsigned long int addr, unsigned long int reg1, unsigned long int reg2, bool condition, string mem_tag);
	void print(ostream& where) const;
};

class instructionBRImmCond:public instructionBRConditionalClass {
public:
	instructionBRImmCond();
	instructionBRImmCond(unsigned long int addr, unsigned long int reg1, long int imm, bool condition, string mem_tag);
	void print(ostream& where) const;
};

class instructionBRXConditional:public instructionBRConditionalClass {
public:
	instructionBRXConditional();
	instructionBRXConditional(unsigned long int addr, unsigned long int reg1, unsigned long int reg2, bool condition, unsigned long int reg3);
	void print(ostream& where) const;
};

class instructionBRXImmCond:public instructionBRConditionalClass {
public:
	instructionBRXImmCond();
	instructionBRXImmCond(unsigned long int addr, unsigned long int reg1, long int imm, bool condition, unsigned long int reg3);
	void print(ostream& where) const;
};

class instructionEND:public instruction {
public:
	instructionEND();
	void print(ostream& where) const;
	void commit();
	void update_stats();
};

class instructionFactory {
public:
	static instruction *buildInstruction(unsigned long int addr, string line);
};

#endif /* SRC_INSTRUCTION_HH_ */

