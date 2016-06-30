/*
 * sim_processor.hh
 *
 *  Created on: 12 de mai de 2016
 *      Author: lsantos
 */

#ifndef SRC_SIM_PROCESSOR_HH_
#define SRC_SIM_PROCESSOR_HH_

#include <map>

#include "sim_pipeline.hh"
#include "sim_processor_state.hh"

class sim_system;

enum {
	matrix_bank_FIRST = 32,
	matrix_bank_A = matrix_bank_FIRST,
	matrix_bank_DataA,
	matrix_bank_B,
	matrix_bank_DataB,
	matrix_bank_Res,
	matrix_bank_IncA,
	matrix_bank_IncB,
	matrix_bank_Count,
	matrix_bank_Status = 40,
	matrix_bank_NUM
};

#define MATRIX_BANK_BIT_START	0x01
#define MATRIX_BANK_BIT_RUNNING	0x02
#define MATRIX_BANK_BIT_STOP	0x80

enum {
	linked_bank_FIRST = 32,
	linked_bank_El = linked_bank_FIRST,
	linked_bank_Tail,
	linked_bank_DataSearch,
	linked_bank_DataOffset,
	linked_bank_ElData,
	linked_bank_Result,
	linked_bank_DataScratch,
	linked_bank_Status = 41
};

#define LINKED_BANK_BIT_START	0x01
#define LINKED_BANK_BIT_FOUND	0x02
#define LINKED_BANK_BIT_STOP	0x80

enum {
	bfs_bank_FIRST = 32,
	bfs_bank_AdjP = bfs_bank_FIRST,
	bfs_bank_AdjPInc,
	bfs_bank_QueueTailP,
	bfs_bank_Cont,
	bfs_bank_VisitedP,
	bfs_bank_AdjN,
	bfs_bank_AdjNOld,
	bfs_bank_Status = 42,
	bfs_bank_NUM
};

#define BFS_BANK_BIT_START	0x01
#define BFS_BANK_BIT_RUNNING	0x02
#define BFS_BANK_BIT_STOP	0x08
#define BFS_BANK_STATE_IDLE	0x00
#define BFS_BANK_STATE_1	0x10
#define BFS_BANK_STATE_2	0x20
#define BFS_BANK_STATE_3	0x30
#define BFS_BANK_STATE_4	0x40
#define BFS_BANK_STATE_5	0x50
#define BFS_BANK_STATE_MASK	0xF0

class sim_processor {
public:
	sim_processor(sim_system *system);
	int clock_tick(unsigned long int curr_tick);
	long int register_read(unsigned long int idx);
	void register_write(unsigned long int idx, long long int value);
	void print_register_bank();

public:

	long long int register_bank[44];
	long long int register_bank_old[44];

public:
	sim_system *system;
	sim_processor_state state;
	sim_pipeline pipeline;
};

#endif /* SRC_SIM_PROCESSOR_HH_ */
