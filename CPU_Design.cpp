#include <bits/stdc++.h>
using namespace std;

int PC;
int DM[1024];
vector<string> IM;
int GPR[32] = {
    0,          // x0: Always 0 (hardwired)
    5,          // x1: Example value for return address (RA)
    100,        // x2: Example value for stack pointer (SP)
    200,        // x3: Example value for global pointer (GP)
    300,        // x4: Example value for thread pointer (TP)
    1000,       // x5: Example usage as temporary register (T0)
    1500,       // x6: Example usage as temporary register (T1)
    2000,       // x7: Example usage as temporary register (T2)
    0x7FFFFFFF, // x8: Example usage as saved register (S0/FP)
    4000,       // x9: Example usage as saved register (S1)
    10000,      // x10: Example usage for function arguments (A0)
    10000,      // x11: Example usage for function arguments (A1)
    12000,      // x12: Example usage for function arguments (A2)
    13000,      // x13: Example usage for function arguments (A3)
    14000,      // x14: Example usage for function arguments (A4)
    15000,      // x15: Example usage for function arguments (A5)
    16000,      // x16: Example usage for function arguments (A6)
    17000,      // x17: Example usage for function arguments (A7)
    18000,      // x18: Example usage as saved register (S2)
    19000,      // x19: Example usage as saved register (S3)
    20000,      // x20: Example usage as saved register (S4)
    21000,      // x21: Example usage as saved register (S5)
    22000,      // x22: Example usage as saved register (S6)
    23000,      // x23: Example usage as saved register (S7)
    24000,      // x24: Example usage as saved register (S8)
    25000,      // x25: Example usage as saved register (S9)
    26000,      // x26: Example usage as saved register (S10)
    27000,      // x27: Example usage as saved register (S11)
    28000,      // x28: Example usage as temporary register (T3)
    29000,      // x29: Example usage as temporary register (T4)
    30000,      // x30: Example usage as temporary register (T5)
    31000       // x31: Example usage as temporary register (T6)
};

class controlWord {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
};

map<string, controlWord> controlUnit;

void initControlUnit() {
    // R-type (e.g., add, sub, and, or)
    controlUnit["0110011"] = {1, 1, 0, 10, 0, 0, 0, 0, 0};

    // I-type (e.g., addi, ori, andi)
    controlUnit["0010011"] = {1, 1, 1, 11, 0, 0, 0, 0, 0};

    // I-type (Load, e.g., lw)
    controlUnit["0000011"] = {1, 1, 1, 00, 0, 0, 1, 0, 1};

    // S-type (Store, e.g., sw)
    controlUnit["0100011"] = {1, 0, 1, 00, 0, 0, 0, 1, -1}; // Mem2Reg is X (Don't care)

    // B-type (Branch, e.g., beq, bne)
    controlUnit["1100011"] = {1, 0, 0, 01, 1, 0, 0, 0, -1}; // Mem2Reg is X (Don't care)

    // U-type (e.g., lui, auipc)
    controlUnit["0110111"] = {0, 1, 1, 00, 0, 0, 0, 0, 0};

    // J-type (e.g., jal)
    controlUnit["1101111"] = {0, 1, 0, 00, 0, 1, 0, 0, 0};
}

void initDM() {
    for(int i = 0; i < 1024; i++) {
        DM[i] = 2*i;
    }
}

class IFID{
public:
    string IR;
    int DPC, NPC;
};

class IDEX{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
            void controller(string str) {
                controlWord cow = controlUnit[str];
                this->RegRead = cow.RegRead;
                this->RegWrite = cow.RegWrite;
                this->ALUSrc = cow.ALUSrc;
                this->ALUOp = cow.ALUOp;
                this->Branch = cow.Branch;
                this->Jump = cow.Jump;
                this->MemRead = cow.MemRead;
                this->MemWrite = cow.MemWrite;
                this->Mem2Reg = cow.Mem2Reg;
            }
    };
public:
    string imm1, imm2, func, rdl, rs1, rs2;
    int JPC, DPC;
    CW cw;
};

class EXMO{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            void copyCW(IDEX &idex){
                this->RegRead = idex.cw.RegRead;
                this->RegWrite = idex.cw.RegWrite;
                this->ALUSrc = idex.cw.ALUSrc;
                this->ALUOp = idex.cw.ALUOp;
                this->Branch = idex.cw.Branch;
                this->Jump = idex.cw.Jump;
                this->MemRead = idex.cw.MemRead;
                this->MemWrite = idex.cw.MemWrite;
                this->Mem2Reg = idex.cw.Mem2Reg;
            }
    };
public:
    string rdl;
    int ALUOUT;
    CW cw;
};

class MOWB{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            void copyCW(EXMO exmo){
                this->RegRead = exmo.cw.RegRead;
                this->RegWrite = exmo.cw.RegWrite;
                this->ALUSrc = exmo.cw.ALUSrc;
                this->ALUOp = exmo.cw.ALUOp;
                this->Branch = exmo.cw.Branch;
                this->Jump = exmo.cw.Jump;
                this->MemRead = exmo.cw.MemRead;
                this->MemWrite = exmo.cw.MemWrite;
                this->Mem2Reg = exmo.cw.Mem2Reg;
            }
    };
public:
    string rdl;
    int ALUOUT, LDOUT;
    CW cw;
};

// int to_int(string binaryStr){
//     int decimalValue = 0;
//     int length = binaryStr.length();

//     for (int i = 0; i < length; i++) {
//         char bit = binaryStr[length - 1 - i];

//         if (bit == '1') {
//             decimalValue += (1 << i); 
//         }
//     }

//     return decimalValue;
// }

int to_int(string binaryStr) {
    int decimalValue = 0;
    int length = binaryStr.length();
    bool isNegative = (binaryStr[0] == '1'); // Check if MSB is 1 (negative in 2's complement)

    if (!isNegative) {
        // Handle positive binary numbers (same as before)
        for (int i = 0; i < length; i++) {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1') {
                decimalValue += (1 << i); 
            }
        }
    } else {
        // Handle negative numbers in 2's complement
        for (int i = 0; i < length; i++) {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1' && i != length - 1) { // Exclude the sign bit
                decimalValue += (1 << i);
            }
        }

        // Subtract 2^n to account for 2's complement
        decimalValue -= (1 << (length - 1));
    }

    return decimalValue;
}

string to_str(int num){
    cout << num << endl;
    bitset<32> binary(num);
    return binary.to_string();
}

int SignedExtend(string imm){
    string extended_imm = imm;
    if(imm[0] == '0'){
        extended_imm = "000000000000" + extended_imm;
    }
    else {
        extended_imm = "111111111111" + extended_imm;
    }

    return to_int(extended_imm);
}

void IF(IFID &ifid){
    int idx = PC/4;
    ifid.IR = IM[idx];
    ifid.DPC = PC;
    ifid.NPC = PC+4;
}

void ID(IDEX &idex, IFID &ifid){
    string ir = ifid.IR;
    idex.JPC = ifid.NPC + 4*SignedExtend(ir.substr(0, 20));
    idex.DPC = ifid.DPC;
    idex.imm1 = ir.substr(0, 12);
    idex.imm2 = ir.substr(0, 7) + ir.substr(20, 5);
    idex.func = ir.substr(17, 3);
    idex.rdl = ir.substr(20, 5);
    idex.cw.controller(ir.substr(25, 7));

    if(idex.cw.RegRead){
        cout << "ir.substr(12, 5)" << " " << ir.substr(12, 5) << endl;
        cout << "GPR[to_int(ir.substr(12, 5))]" << " " << GPR[to_int(ir.substr(12, 5))] << endl;
        idex.rs1 = to_str(GPR[to_int(ir.substr(12, 5))]);
    }
    if(idex.cw.ALUSrc && (ir.substr(25, 7) == "0010011" || ir.substr(25, 7) == "0000011")){
        if(idex.cw.RegRead){
            cout << "idex.imm1" << " " << idex.imm1 << endl;
            idex.rs2 = idex.imm1;
        }
    }
    else {
        if(idex.cw.RegRead){
            cout << "ir.substr(7, 5)" << " " << ir.substr(7, 5) << endl;
            cout << "GPR[to_int(ir.substr(7, 5))]" << " " << GPR[to_int(ir.substr(7, 5))] << endl;
            idex.rs2 = to_str(GPR[to_int(ir.substr(7, 5))]);
        }
    }
}

string ALUControl(int ALUOp, string func, string func7) {
    string ALUSelect;
    
    switch (ALUOp) {
        case 00:  // Load/Store (for I-type, S-type)
            ALUSelect = "0010";  // Perform ADD for address calculation
            break;
        case 01:  // Branch (for B-type)
            if (func == "000") {  // beq
                ALUSelect = "0110";  // Perform SUB for comparison
            } else if (func == "001") {  // bne
                ALUSelect = "0110";  // SUB for comparison
            } else {
                ALUSelect = "1111";  // Undefined
            }
            break;
        case 10:  // R-type instructions
            if (func == "000") {  // ADD
                ALUSelect = func7 == "0000000" ? "0010" : "0110";  // ADD or SUB
            } else if (func == "001") {  // SLL
                ALUSelect = "0011";  // Shift left
            } else if (func == "111") {  // AND
                ALUSelect = "0000";
            } else if (func == "110") {  // OR
                ALUSelect = "0001";
            } else {
                ALUSelect = "1111";  // Undefined
            }
            break;
        case 11:  // I-type instructions
            if (func == "000") {  // ADDI
                ALUSelect = "0010";  // ADD immediate
            } else if (func == "010") {  // SLTI
                ALUSelect = "0111";  // Set less than immediate
            } else if (func == "100") {  // XORI
                ALUSelect = "0011";  // XOR immediate
            } else {
                ALUSelect = "1111";  // Undefined
            }
            break;
        default:
            ALUSelect = "1111";  // Undefined operation
            break;
    }

    return ALUSelect;
}

int ALU(string ALUSelect, string rs1, string rs2) {
    // Convert input strings to integers
    int operand1 = bitset<32>(rs1).to_ulong();
    int operand2 = bitset<32>(rs2).to_ulong();
    int result = 0;

    // Perform the operation based on ALUSelect
    if (ALUSelect == "0000") {  // AND
        result = operand1 & operand2;
    } else if (ALUSelect == "0001") {  // OR
        result = operand1 | operand2;
    } else if (ALUSelect == "0010") {  // ADD
        result = operand1 + operand2;
    } else if (ALUSelect == "0110") {  // SUB
        result = operand1 - operand2;
    } else if (ALUSelect == "0011") {  // SLL
        result = operand1 << (operand2 & 0x1F);  // Logical Shift Left
    } else if (ALUSelect == "0100") {  // SRL
        result = operand1 >> (operand2 & 0x1F);  // Logical Shift Right
    } else if (ALUSelect == "0101") {  // SRA
        result = operand1 >> (operand2 & 0x1F);  // Arithmetic Shift Right
    } else if (ALUSelect == "0111") {  // SLT (Set Less Than)
        result = (operand1 < operand2) ? 1 : 0;
    } else if (ALUSelect == "1000") {  // SLTU (Set Less Than Unsigned)
        result = ((unsigned int)operand1 < (unsigned int)operand2) ? 1 : 0;
    } else if (ALUSelect == "1111") {  // Undefined operation
        std::cerr << "Error: Undefined ALU operation." << std::endl;
        return -1;  // Return -1 to indicate an error
    }

    return result;  // Return the result of the ALU operation
}

void IE(EXMO &exmo, IDEX &idex, IFID &ifid){
    string ALUSelect = ALUControl(idex.cw.ALUOp, idex.func, ifid.IR.substr(0, 7));
    cout << "ALUSelect " << ALUSelect << endl;
    string opcode = ifid.IR.substr(25, 7);
    if(opcode == "0100011" || opcode == "1100011"){
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.imm2);
    }
    else {
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.rs2);
    }
    int ALUZeroFlag = (idex.rs1 == idex.rs2);
    exmo.cw.copyCW(idex);
    if(idex.cw.Branch && ALUZeroFlag){
        cout << "(to_int(idex.imm2) << 1) " << (to_int(idex.imm2) << 1) << endl;
        PC = to_int(idex.imm2)*4 + ifid.NPC;
    }
    else {
        PC = PC + 4;
    }
    if(idex.cw.Jump){
        PC = idex.JPC;
    }
    exmo.rdl = idex.rdl;
}

void MA(MOWB &mowb, EXMO &exmo, IDEX &idex){
    cout << "exmo.ALUOUT " << exmo.ALUOUT << endl;
    if(exmo.cw.MemWrite){
        DM[exmo.ALUOUT] = to_int(idex.rs2);
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }
    if(exmo.cw.MemRead){
        mowb.LDOUT = DM[exmo.ALUOUT];
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }

    mowb.ALUOUT = exmo.ALUOUT;
    mowb.cw.copyCW(exmo);
    mowb.rdl = exmo.rdl;
}

void RW(MOWB &mowb){
    if(mowb.cw.RegWrite){
        if(mowb.cw.Mem2Reg){
            GPR[to_int(mowb.rdl)] = mowb.LDOUT;
        }
        else {
            GPR[to_int(mowb.rdl)] = mowb.ALUOUT;
        }
    }
}

int main () {
    IFID ifid;
    IDEX idex;
    EXMO exmo;
    MOWB mowb;
    
    vector<string> machineCode = {
        // "00000000000100010000000110110011" 
        // 0000000 00001 00010 000 00011 0110011 -> R-Type
        // "00000000101000110000001010010011"
        //000000001010 00110 000 00101 0010011 -> I-Type
        // "00000000000000110010000110110111"
        //00000000000000110010 00011 0110111 -> U-Type
        // "00000000010000011010010100100011"
        //0000000 00100 00011 010 01010 0100011 -> S-Type
        // "00000000101000100010001010000011"
        //000000001010 00100 010 00101 0000011 -> L-Type
        // "11111111111111111110001111101111"
        //11111111111111111110 00111 1101111 -> J-Type
        // "11111110101101010000110111100011"
        //1111111 01011 01010 000 11011 1100011 -> B-Type
    };

    IM = machineCode;
    PC = 0;
    initControlUnit();
    initDM();

    // while(1) {
    //     RW(mowb);
    //     MA(mowb, exmo, idex);
    //     IE(exmo, idex, ifid);
    //     ID(idex, ifid);
    //     IF(ifid);
    // }    
    
    cout << "Loaded in the IM" << endl;
    for(int i = 0; i < IM.size(); i++){
        cout << IM[i] << endl;
    }
    cout << endl;

    IF(ifid);
    cout << "IR " << ifid.IR << endl;
    cout << "DPC " << ifid.DPC << endl;
    cout << "NPC " << ifid.NPC << endl;

    cout << endl;

    ID(idex, ifid);

    cout << "imm1 " << idex.imm1 << endl;
    cout << "imm2 " << idex.imm2 << endl;
    cout << "func "<< idex.func << endl;
    cout << "rdl " << idex.rdl << endl;
    cout << "rs1 " << idex.rs1 << endl;
    cout << "rs2 " << idex.rs2 << endl;
    cout << "JPC " << idex.JPC << endl;
    cout << "DPC " << idex.DPC << endl;
    cout << endl;
    cout << "RegRead " << idex.cw.RegRead << endl;
    cout << "RegWrite " << idex.cw.RegWrite << endl;
    cout << "ALUSrc " << idex.cw.ALUSrc << endl;
    cout << "ALUOp " << idex.cw.ALUOp  << endl;
    cout << "Branch " << idex.cw.Branch << endl;
    cout << "Jump " << idex.cw.Jump << endl;
    cout << "MemRead " << idex.cw.MemRead << endl;
    cout << "MemWrite " << idex.cw.MemWrite << endl;
    cout << "Mem2Reg " << idex.cw.Mem2Reg << endl;
    
    IE(exmo, idex, ifid);
    cout << endl;
    cout << "ALUOUT " << exmo.ALUOUT << endl;

    MA(mowb, exmo, idex);
    cout << endl;
    cout << "mowb.ALUOUT " << mowb.ALUOUT << endl;
    cout << "mowb.LDOUT " << mowb.LDOUT << endl;
    cout << "mowb.rdl " << mowb.rdl << endl;  

    RW(mowb);
    cout << endl;
    cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    cout << "to_int(mowb.rdl) " << to_int(mowb.rdl) << endl;
    cout << "GPR[to_int(mowb.rdl)] " << GPR[to_int(mowb.rdl)] << endl;
    cout << "PC " << PC << endl;
    return 0;
}