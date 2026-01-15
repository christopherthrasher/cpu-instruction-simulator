## CPU Instruction Simulator

A C++ program that simulates a simplified ARM-like CPU by reading and executing instructions from an input file. The simulator models register state, memory, condition flags, and instruction-level control flow, including conditional execution and branching.

## Overview
The simulator processes an instruction stream from a text file and executes each instruction sequentially while maintaining and displaying CPU state after every operation. The design emphasizes low-level systems concepts such as instruction decoding, flag-based condition checking, and memory access.

The simulated CPU includes:
- 12 general-purpose registers (R0–R11)
- A fixed memory region mapped to addresses 0x100–0x110
- NZCV condition flags for arithmetic and comparison results
- Conditional execution for all supported instructions
- Branching functionality based on flag state

All arithmetic and logical operations use 32-bit unsigned integers.

## Features
- Instruction parsing and execution from file input
- Register-based arithmetic and logical operations
- Bitwise and shift operations
- Condition flag (NZCV) tracking
- Conditional instruction execution
- Branching based on comparison results
- Memory load and store operations
- Hexadecimal state output after each instruction
- Makefile-based build system

## Supported Instructions
- MOV, MVN
- ADD, SUB, ADDS
- AND, ORR, EOR
- CMP
- LSL, LSR
- LDR, STR
- BEQ (branch on equal)

Invalid registers or invalid memory accesses are safely ignored.

## Technologies & Concepts
- C++
- Computer Architecture
- Instruction decoding
- Bitwise operations
- Conditional execution
- State simulation
- File I/O
- Makefile compilation

## How to Build and Run

### Prerequisites
- A C++ compiler that supports C++11 or newer (e.g., `g++`)
- `make` installed

### Build the Program
From the project root directory, run:
make

### Run the Program
./cpu
