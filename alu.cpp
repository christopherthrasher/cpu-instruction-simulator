#include "alu.h"

uint32_t parseNumber(const string &token) {
    uint32_t value = 0;
    string s = token;

    if (s.size() > 2 && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X')) {
        stringstream ss;
        ss << std::hex << s;
        ss >> value;
    } else {
        stringstream ss;
        ss << s;
        ss >> value;
    }
    return value;
}

string toHexString(uint32_t value) {
    stringstream ss;
    ss << "0x" << std::uppercase << std::hex << value;
    return ss.str();
}

void updateFlags(uint32_t result, uint32_t op1, uint32_t op2, Flags &flags, bool isSub) {
    flags.N = ((result >> 31) & 1u) != 0;
    flags.Z = (result == 0);

    if (isSub) {
        flags.C = (op1 >= op2);
    } else {
        uint64_t wide = static_cast<uint64_t>(op1) + static_cast<uint64_t>(op2);
        flags.C = (wide > 0xFFFFFFFFu);
    }

    int32_t s1 = static_cast<int32_t>(op1);
    int32_t s2 = static_cast<int32_t>(op2);
    int32_t sr = static_cast<int32_t>(result);

    if (isSub) {
        flags.V = ((s1 < 0 && s2 > 0 && sr > 0) || (s1 > 0 && s2 < 0 && sr < 0));
    } else {
        flags.V = ((s1 > 0 && s2 > 0 && sr < 0) || (s1 < 0 && s2 < 0 && sr > 0));
    }
}

bool checkCondition(const string &cond, const Flags &flags) {
    if (cond == "EQ") {
        return flags.Z == 1;
    }
    if (cond == "NE") {
        return flags.Z == 0;
    }
    if (cond == "GT") {
        return (flags.Z == 0) && (flags.N == flags.V);
    }
    if (cond == "LT") {
        return (flags.N != flags.V);
    }
    if (cond == "GE") {
        return (flags.N == flags.V);
    }
    if (cond == "LE") {
        return (flags.Z == 1) || (flags.N != flags.V);
    }
    if (cond == "") {
        return true;
    }
    return false;
}

uint32_t aluExecute(AluOp op, uint32_t a, uint32_t b, Flags &flags, bool setFlags) {

    uint32_t result = 0;

    switch (op) {
        case ALU_ADD:
            result = a + b;
            break;
        case ALU_SUB:
            result = a - b;
            break;
        case ALU_AND:
            result = a & b;
            break;
        case ALU_ORR:
            result = a | b;
            break;
        case ALU_EOR:
            result = a ^ b;
            break;
        case ALU_LSL:
            if (b < 32) {
                result = a << b;
            } else {
                result = 0;
            }
            break;
        case ALU_LSR:
            if (b < 32) {
                result = a >> b;
            } else {
                result = 0;
            }
            break;
        default:
            result = 0;
            break;
    }

    if (setFlags) {
        bool isSub = (op == ALU_SUB);
        updateFlags(result, a, b, flags, isSub);
    }

    return result;
}
