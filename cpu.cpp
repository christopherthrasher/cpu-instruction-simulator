#include "cpu.h"

uint32_t regFile[12] = {0};
uint32_t mem[5]      = {0};
Flags    cpsr;

static string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos || end == string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

static int regIndex(const string &name) {
    if (name.size() < 2 || name[0] != 'R') {
        return -1;
    }
    int idx = -1;
    string num = name.substr(1);
    stringstream ss(num);
    ss >> idx;
    if (ss.fail() || idx < 0 || idx > 11) {
        return -1;
    }
    return idx;
}

static int memIndex(uint32_t address) {
    if (address < 0x100 || address > 0x110) {
        return -1;
    }
    if ((address - 0x100) % 4 != 0) {
        return -1;
    }
    int idx = (address - 0x100) / 4;
    if (idx < 0 || idx >= 5) {
        return -1;
    }
    return idx;
}

static vector<string> parseOperands(const string &rest) {
    vector<string> ops;
    string temp;
    stringstream ss(rest);

    while (getline(ss, temp, ',')) {
        string cleaned = trim(temp);
        if (!cleaned.empty()) {
            ops.push_back(cleaned);
        }
    }
    return ops;
}

static void decodeOpcode(const string &token, string &base, bool &setFlags, string &cond) {

    setFlags = false;
    cond = "";

    string t = token;

    const string conds[6] = {"EQ", "NE", "GT", "LT", "GE", "LE"};
    for (int i = 0; i < 6; i++) {
        string c = conds[i];
        if (t.size() > c.size() &&
            t.substr(t.size() - c.size()) == c) {
            cond = c;
            t = t.substr(0, t.size() - c.size());
            break;
        }
    }

    if (!t.empty() && t.back() == 'S') {
        setFlags = true;
        t = t.substr(0, t.size() - 1);
    }

    base = t;
}

static bool isOpcodeToken(const string &token) {
    string base, cond;
    bool s = false;
    decodeOpcode(token, base, s, cond);

    const string validOps[] = {
        "MOV", "MVN",
        "ADD", "SUB", "AND", "ORR", "EOR",
        "CMP",
        "LDR", "STR",
        "LSL", "LSR",
        "B"
    };

    for (const string &op : validOps) {
        if (base == op) {
            return true;
        }
    }
    return false;
}

static int findLabel(const vector<string> &program,
                     const string &label) {
    for (size_t i = 0; i < program.size(); i++) {
        istringstream iss(program[i]);
        string first;
        iss >> first;
        if (first == label) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void printState() {
    cout << "Register array:" << endl;
    for (int i = 0; i < 12; i++) {
        cout << "R" << i << " =" << toHexString(regFile[i]) << " ";
    }
    cout << endl;
    cout << "NZCV: " << cpsr.N << cpsr.Z << cpsr.C << cpsr.V << endl;
    cout << "Memory array:" << endl;
    for (int i = 0; i < 5; i++) {
        cout << toHexString(mem[i]) << " ";
    }
    cout << endl;
}

static void executeOne(const vector<string> &program, int &pc) {
    string line = program[pc];

    cout << line << endl;

    istringstream iss(line);
    string first;
    iss >> first;

    string opcodeToken;
    string label;

    if (isOpcodeToken(first)) {
        opcodeToken = first;
    } else {
        label = first;
        iss >> opcodeToken;
    }

    string rest;
    getline(iss, rest);
    rest = trim(rest);
    vector<string> ops = parseOperands(rest);

    string baseOp, cond;
    bool setFlags = false;
    decodeOpcode(opcodeToken, baseOp, setFlags, cond);

    if (baseOp == "B") {
        bool take = checkCondition(cond, cpsr);
        if (take && ops.size() >= 1) {
            int target = findLabel(program, ops[0]);
            printState();
            if (target != -1) {
                pc = target;
            } else {
                pc++;
            }
        } else {
            printState();
            pc++;
        }
        return;
    }

    if (!checkCondition(cond, cpsr)) {
        printState();
        pc++;
        return;
    }

    if (baseOp == "MOV") {
        if (ops.size() < 2) {
            printState();
            pc++;
            return;
        }
        int dst = regIndex(ops[0]);
        if (dst == -1) {
            printState();
            pc++;
            return;
        }
        if (!ops[1].empty() && ops[1][0] == '#') {
            string num = ops[1].substr(1);
            regFile[dst] = parseNumber(num);
        } else {
            int src = regIndex(ops[1]);
            if (src == -1) {
                printState();
                pc++;
                return;
            }
            regFile[dst] = regFile[src];
        }
    }
    else if (baseOp == "MVN") {
        if (ops.size() < 2) {
            printState();
            pc++;
            return;
        }
        int dst = regIndex(ops[0]);
        int src = regIndex(ops[1]);
        if (dst == -1 || src == -1) {
            printState();
            pc++;
            return;
        }
        uint32_t result = ~regFile[src];
        regFile[dst] = result;
    }
    else if (baseOp == "ADD" || baseOp == "SUB" ||
             baseOp == "AND" || baseOp == "ORR" ||
             baseOp == "EOR") {

        if (ops.size() < 3) {
            printState();
            pc++;
            return;
        }
        int dst = regIndex(ops[0]);
        int rn  = regIndex(ops[1]);
        if (dst == -1 || rn == -1) {
            printState();
            pc++;
            return;
        }

        uint32_t op1 = regFile[rn];
        uint32_t op2 = 0;

        if (!ops[2].empty() && ops[2][0] == '#') {
            string num = ops[2].substr(1);
            op2 = parseNumber(num);
        } else {
            int rm = regIndex(ops[2]);
            if (rm == -1) {
                printState();
                pc++;
                return;
            }
            op2 = regFile[rm];
        }

        AluOp opCode;
        if (baseOp == "ADD")      opCode = ALU_ADD;
        else if (baseOp == "SUB") opCode = ALU_SUB;
        else if (baseOp == "AND") opCode = ALU_AND;
        else if (baseOp == "ORR") opCode = ALU_ORR;
        else                      opCode = ALU_EOR;

        regFile[dst] = aluExecute(opCode, op1, op2, cpsr, setFlags);
    }
    else if (baseOp == "CMP") {
        if (ops.size() < 2) {
            printState();
            pc++;
            return;
        }
        int rn = regIndex(ops[0]);
        if (rn == -1) {
            printState();
            pc++;
            return;
        }
        uint32_t op1 = regFile[rn];
        uint32_t op2 = 0;

        if (!ops[1].empty() && ops[1][0] == '#') {
            string num = ops[1].substr(1);
            op2 = parseNumber(num);
        } else {
            int rm = regIndex(ops[1]);
            if (rm == -1) {
                printState();
                pc++;
                return;
            }
            op2 = regFile[rm];
        }

        uint32_t result = op1 - op2;
        updateFlags(result, op1, op2, cpsr, true);
    }
    else if (baseOp == "LDR") {
        if (ops.size() < 2) {
            printState();
            pc++;
            return;
        }
        int dst = regIndex(ops[0]);
        if (dst == -1) {
            printState();
            pc++;
            return;
        }

        string memOp = ops[1];
        if (memOp.size() < 3 || memOp.front() != '[' || memOp.back() != ']') {
            printState();
            pc++;
            return;
        }
        string regName = memOp.substr(1, memOp.size() - 2);
        regName = trim(regName);
        int addrReg = regIndex(regName);
        if (addrReg == -1) {
            printState();
            pc++;
            return;
        }

        int idx = memIndex(regFile[addrReg]);
        if (idx == -1) {
            printState();
            pc++;
            return;
        }
        regFile[dst] = mem[idx];
    }
    else if (baseOp == "STR") {
        if (ops.size() < 2) {
            printState();
            pc++;
            return;
        }
        int src = regIndex(ops[0]);
        if (src == -1) {
            printState();
            pc++;
            return;
        }

        string memOp = ops[1];
        if (memOp.size() < 3 || memOp.front() != '[' || memOp.back() != ']') {
            printState();
            pc++;
            return;
        }
        string regName = memOp.substr(1, memOp.size() - 2);
        regName = trim(regName);
        int addrReg = regIndex(regName);
        if (addrReg == -1) {
            printState();
            pc++;
            return;
        }

        int idx = memIndex(regFile[addrReg]);
        if (idx == -1) {
            printState();
            pc++;
            return;
        }
        mem[idx] = regFile[src];
    }
    else if (baseOp == "LSL" || baseOp == "LSR") {
        if (ops.size() < 3) {
            printState();
            pc++;
            return;
        }
        int dst = regIndex(ops[0]);
        int rn  = regIndex(ops[1]);
        if (dst == -1 || rn == -1) {
            printState();
            pc++;
            return;
        }
        if (ops[2].empty() || ops[2][0] != '#') {
            printState();
            pc++;
            return;
        }
        string num = ops[2].substr(1);
        uint32_t sh = parseNumber(num);

        AluOp opCode = (baseOp == "LSL") ? ALU_LSL : ALU_LSR;
        regFile[dst] = aluExecute(opCode, regFile[rn], sh, cpsr, setFlags);
    }
    printState();
    pc++;
}

void runProgram(const vector<string> &program) {
    int pc = 0;
    while (pc >= 0 && pc < static_cast<int>(program.size())) {
        executeOne(program, pc);
    }
}
