// Christopher Thrasher

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cpu.h"

using namespace std;

int main() {
    ifstream in("PP3_input.txt");
    if (!in.is_open()) {
        cerr << "Could not open PP3_input.txt" << endl;
        return 1;
    }

    vector<string> program;
    string line;

    while (getline(in, line)) {
        if (!line.empty()) {
            program.push_back(line);
        }
    }
    in.close();

    runProgram(program);

    return 0;
}
