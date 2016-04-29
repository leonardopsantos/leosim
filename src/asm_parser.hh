/*
 * asm_parser.hh
 *
 *  Created on: 29 de abr de 2016
 *      Author: lsantos
 */

#ifndef SRC_ASM_PARSER_HH_
#define SRC_ASM_PARSER_HH_

#include <iostream>
#include <fstream>

class asm_parser {
public:
	static int parse_asm(std::ifstream& input, memory &mem);
};

#endif /* SRC_ASM_PARSER_HH_ */
