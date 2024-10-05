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
    11000,      // x11: Example usage for function arguments (A1)
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
    controlUnit["0010011"] = {1, 1, 1, 00, 0, 0, 0, 0, 0};

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
    string imm, func, rdl, rs1, rs2;
    int JPC, DPC;
    CW cw;
};

class EXMO{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            void copyCW(IDEX idex){

            }
    };
public:
    string rs2, rdl;
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

            }
    };
public:
    string rdl;
    int ALUOUT, LDOUT;
    CW cw;
};

int to_int(string str){
    int n = stoi(str);
    int num = n;
    int dec_value = 0;
 
    int base = 1;
 
    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;
 
        dec_value += last_digit * base;
 
        base = base * 2;
    }
 
    return dec_value;
}

string to_str(int num){
    bitset<5> binary(num);
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
    idex.JPC = ifid.NPC + SignedExtend(ir.substr(0, 20));
    idex.DPC = ifid.DPC;
    idex.imm = ir.substr(0, 12);
    idex.func = ir.substr(17, 3);
    idex.rdl = ir.substr(20, 5);
    idex.cw.controller(ir.substr(25, 7));

    if(idex.cw.ALUSrc){
        if(idex.cw.RegRead){
            idex.rs2 = ir.substr(0, 12);
        }
    }
    else {
        if(idex.cw.RegRead){
            idex.rs2 = to_str(GPR[to_int(ir.substr(8, 5))]);
        }
    }
}

int ALUControl(int ALUOp, string func){

}

int ALU(int ALUSelect, string rs1, string rs2){

}

void IE(EXMO &exmo, IDEX &idex, IFID &ifid){
    int ALUSelect = ALUControl(idex.cw.ALUOp, idex.func);
    exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.rs2);
    int ALUZeroFlag = (idex.rs1 == idex.rs2);
    exmo.cw.copyCW(idex);
    if(idex.cw.Branch && ALUZeroFlag){
        PC = (to_int(idex.imm) << 1) + ifid.NPC;
    }
    else {
        PC = PC + 4;
    }
    if(idex.cw.Jump){
        PC = idex.JPC;
    }
}

void MA(MOWB &mowb, EXMO &exmo, IDEX &idex){
    if(exmo.cw.MemWrite){
        DM[exmo.ALUOUT] = to_int(idex.rs2);
    }
    if(exmo.cw.MemRead){
        mowb.LDOUT = DM[exmo.ALUOUT];
    }

    mowb.ALUOUT = exmo.ALUOUT;
    mowb.cw.copyCW(exmo);
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
        "00000000000100010000000110110011"
        //0000000 00001 00010 000 00011 0110011
    };

    IM = machineCode;
    PC = 0;
    
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
    cout << ifid.IR << endl;
    cout << ifid.DPC << endl;
    cout << ifid.NPC << endl;

    cout << endl;

    ID(idex, ifid);

    string imm, func, rdl, rs1, rs2;
    int JPC, DPC;

    cout << "imm " << idex.imm << endl;
    cout << "func "<< idex.func << endl;
    cout << "rdl " << idex.rdl << endl;
    cout << "rs1 " << idex.rs1 << endl;
    cout << "rs2 " << idex.rs2 << endl;
    cout << "JPC " << idex.JPC << endl;
    cout << "DPC " << idex.DPC << endl;

    // IE(exmo, idex, ifid);
    // MA(mowb, exmo, idex);    
    // RW(mowb);

    return 0;
}