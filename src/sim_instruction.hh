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
	maxInstDest
};

enum class instClasses {
	Invalid,
	ADD,
	MUL,
	MOV,
	LDR,
	STR,
	maxInstClasses
};

class instruction {
public:
	instruction();
	~instruction();
	virtual void execute();

	friend ostream& operator<<(ostream& os, const instruction& inst);
	virtual void print(ostream& where) const;

public:
	int num_sources;
	instSources sourcesTypes[4];
	long int sources_idx[4];
	int num_dests;
	instDest destsTypes[4];
	long int dests_idx[4];
	unsigned long int memory_pos;
	long int sources_values[4];
	long int destination_values[4];
};

class instructionNOP:public instruction {
public:
	instructionNOP();
	instructionNOP(unsigned long int addr);
	void execute();
	void print(ostream& where) const;
};


class instructionADD:public instruction {
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

class instructionSUB: public instruction {
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

class instructionMUL: public instruction {
public:
	instructionMUL();
	instructionMUL(unsigned long int addr, long int s1, long int s2, long int d);
	void print(ostream& where) const;
};

class instructionMLA: public instruction {
public:
	instructionMLA();
	instructionMLA(unsigned long int addr, long int s1, long int s2, long int s3, long int d);
	void print(ostream& where) const;
};

class instructionMOV:public instruction {
public:
	instructionMOV();
	instructionMOV(unsigned long int addr, long int s1, long int d);
	void print(ostream& where) const;
};

class instructionMOVImm:public instructionMOV {
public:
	instructionMOVImm();
	instructionMOVImm(unsigned long int addr, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionLDR:public instruction {
public:
	instructionLDR();
	instructionLDR(unsigned long int addr, long int s1, long int d);
	void execute();
	void print(ostream& where) const;
};

enum class ldrIndexing {
	indexing_INVALID = 0,
	indexing_Min = 1,
	indexing_OFFSET = indexing_Min,
	indexing_POST,
	indexing_PRE,
	indexing_Max,
};

class instructionLDROff:public instruction {
public:
	instructionLDROff();
	instructionLDROff(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	ldrIndexing indexing;
};

class instructionLDRPre:public instructionLDROff {
public:
	instructionLDRPre();
	instructionLDRPre(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionLDRPost:public instructionLDROff {
public:
	instructionLDRPost();
	instructionLDRPost(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionSTR:public instruction {
public:
	instructionSTR();
	instructionSTR(unsigned long int addr, long int s1, long int d);
	void print(ostream& where) const;
	void execute();
	ldrIndexing indexing;
};

class instructionSTROff:public instruction {
public:
	instructionSTROff();
	instructionSTROff(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
	ldrIndexing indexing;
};

class instructionSTRPre:public instructionSTROff {
public:
	instructionSTRPre();
	instructionSTRPre(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionSTRPost:public instructionSTROff {
public:
	instructionSTRPost();
	instructionSTRPost(unsigned long int addr, long int s1, long int imm, long int d);
	void print(ostream& where) const;
};

class instructionBR:public instruction {
public:
	instructionBR();
	instructionBR(unsigned long int addr, string mem_tag);
	void print(ostream& where) const;
	string tag;
	unsigned long int dest;
};

class instructionBRLink:public instructionBR {
public:
	instructionBRLink();
	instructionBRLink(unsigned long int addr, string mem_tag);
	void print(ostream& where) const;
};

class instructionBRX:public instruction {
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

class instructionFactory {
public:
	static instruction *buildInstruction(unsigned long int addr, string line);
};

#endif /* SRC_INSTRUCTION_HH_ */

