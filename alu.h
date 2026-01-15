#ifndef ALU_H
#define ALU_H

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

struct Flags {
    bool N;
    bool Z;
    bool C;
    bool V;

    Flags() {
        N = false;
        Z = false;
        C = false;
        V = false;
    }
};

enum AluOp {
    ALU_ADD = 1,
    ALU_SUB,
    ALU_AND,
    ALU_ORR,
    ALU_EOR,
    ALU_LSL,
    ALU_LSR
};

uint32_t parseNumber(const string &token);
string    toHexString(uint32_t value);

// flags and condition helpers
void updateFlags(uint32_t result, uint32_t op1, uint32_t op2, Flags &flags, bool isSub);

bool checkCondition(const string &cond, const Flags &flags);

// alu
uint32_t aluExecute(AluOp op, uint32_t a, uint32_t b, Flags &flags,bool setFlags);

#endif
