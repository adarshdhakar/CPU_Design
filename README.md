# RISC-V Pipeline Simulator

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Running the Simulator](#running-the-simulator)
- [Project Structure](#project-structure)
- [Instruction Set](#instruction-set)
- [Pipeline Stages](#pipeline-stages)
- [Memory Organization](#memory-organization)
- [Control Unit](#control-unit)

## Introduction
This project is a **RISC-V Pipeline Simulator** written in C++. It simulates the execution of RISC-V assembly instructions through a pipelined processor model. The pipeline stages implemented include instruction fetch (IF), instruction decode (ID), execution (EX), memory operation (MEM), and write-back (WB). The simulator uses control signals for each stage to implement pipelined execution and includes mechanisms for stalling, data forwarding, and handling hazards.

## Features
- Five-stage pipelined processor: IF, ID, EX, MEM, and WB.
- Supports various types of RISC-V instructions (R-type, I-type, S-type, B-type, U-type, and J-type).
- Simulates data hazards and implements stalling.
- Simulates branching and jump instructions with proper control signal handling.
- Memory read and write operations are supported.
- Modular and extensible design to add more instructions and functionality.
  
## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/your-username/risc-v-pipeline-simulator.git
    cd risc-v-pipeline-simulator
    ```

2. Compile the source code using g++:
    ```bash
    g++ -o simulator main.cpp -std=c++11
    ```

3. Ensure you have a working C++11 or later compiler installed.

## Running the Simulator

1. After compiling, you can run the simulator by executing:
    ```bash
    ./simulator
    ```

2. The simulator uses a pre-defined memory setup and set of instructions stored in the `IM` array. Modify the instructions as necessary within the code.

3. The output will show the register values, pipeline stage information, and any relevant memory operations.

## Project Structure
risc-v-pipeline-simulator/ │ ├── main.cpp # Main simulator code ├── README.md # This file └── Makefile # Makefile for building the project

## Instruction Set
The simulator supports a subset of RISC-V instructions, including:

- **R-type instructions**: `add`, `sub`, `and`, `or`, `xor`, etc.
- **I-type instructions**: `addi`, `ori`, `andi`, `lw`, etc.
- **S-type instructions**: `sw`
- **B-type instructions**: `beq`, `bne`
- **U-type instructions**: `lui`, `auipc`
- **J-type instructions**: `jal`

Each instruction is associated with a specific control word that dictates how the instruction is executed at each pipeline stage.

## Pipeline Stages

The pipeline is divided into five stages, each represented by its respective class in the code:

1. **Instruction Fetch (IF)**: Fetches instructions from memory.
2. **Instruction Decode (ID)**: Decodes instructions, reads registers, and calculates branch targets.
3. **Execution (EX)**: Performs ALU operations and calculates memory addresses.
4. **Memory Operation (MEM)**: Handles data memory read and write operations.
5. **Write Back (WB)**: Writes the result back to the register file.

### Stalling
The simulator implements stalling through the `Stall` class. The simulator checks for data hazards and stalls the pipeline when necessary.

### Valid Signals
The simulator uses the `Valid` class to track the validity of stages (whether the stage contains valid data to be processed).

## Memory Organization
- **Data Memory (`DM`)**: A simple array that simulates memory. It can store and retrieve data for load/store instructions.
- **Instruction Memory (`IM`)**: A vector containing RISC-V machine code (in binary string format) for instructions.

Registers (GPRs) and memory (DM) can be initialized or modified to simulate different scenarios.

## Control Unit
The control unit is responsible for generating control signals based on the instruction type. It is implemented through the `controlWord` class and controls various pipeline operations such as register read/write, ALU source, memory read/write, etc.

Control signals are stored in a `controlUnit` map that maps instruction opcodes to specific control signal values.

Example control signals:
- **RegWrite**: Whether the instruction writes to a register.
- **ALUSrc**: Determines if the second ALU operand is a register or an immediate value.
- **MemRead**: Whether the instruction reads from memory.
- **MemWrite**: Whether the instruction writes to memory.
- **Branch**: Whether the instruction is a branch.




