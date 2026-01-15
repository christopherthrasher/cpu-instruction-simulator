#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include "alu.h"

using namespace std;

extern uint32_t regFile[12];
extern uint32_t mem[5];
extern Flags cpsr;

void runProgram(const vector<string> &program);

void printState();

#endif
