#include <bits/stdc++.h>
using namespace std;

int PC;
int DM[1024];
vector<string> IM;
string GPR;

class IFID{
public:
    string IR;
    int DPC, NPC;
    
public:
    IFID(){
        
    }
};

class IDEX{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            CW(){

            }
            void controller(string str) {

            }
    };
public:
    string imm, func, rdl, rs1, rs2;
    int JPC, DPC;
    CW cw;
public:
    IDEX(){
        
    }
};

class EXMO{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            CW(){
                
            }
            void copyCW(IDEX idex){

            }
    };
public:
    string rs2, rdl;
    int ALUOUT;
    CW cw;
    
public:
    EXMO(){
        
    }
};

class MOWB{
public:
    class CW {
        public:
            int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        public:
            CW(){
                
            }
            void copyCW(EXMO exmo){

            }
    };
public:
    string rdl;
    int ALUOUT, LDOUT;
    CW cw;
    
public:
    MOWB(){
        
    }
};

int SignedExtend(string str){

}

int to_int(string str){

}

void IF(IFID ifid){
    int idx = PC/4;
    ifid.IR = IM[idx];
    ifid.DPC = PC;
    ifid.NPC = PC+4;
}

void ID(IDEX idex, IFID ifid){
    string ir = ifid.IR;
    idex.JPC = ifid.NPC + SignedExtend(ir.substr(0, 20));
    idex.DPC = ifid.DPC;
    idex.imm = ir.substr(0, 12);
    idex.func = ir.substr(18, 3);
    idex.rdl = ir.substr(21, 5);
    idex.cw.controller(ir.substr(26, 7));

    if(idex.cw.ALUSrc){
        if(idex.cw.RegRead){
            idex.rs2 = ir.substr(0, 12);
        }
    }
    else {
        if(idex.cw.RegRead){
            idex.rs2 = GPR[to_int(ir.substr(8, 5))];
        }
    }
}

int ALUControl(int ALUOp, string func){

}

int ALU(int ALUSelect, string rs1, string rs2){

}

void IE(EXMO exmo, IDEX idex, IFID ifid){
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

void MA(MOWB mowb, EXMO exmo, IDEX idex){
    if(exmo.cw.MemWrite){
        DM[exmo.ALUOUT] = to_int(idex.rs2);
    }
    if(exmo.cw.MemRead){
        mowb.LDOUT = DM[exmo.ALUOUT];
    }

    mowb.ALUOUT = exmo.ALUOUT;
    mowb.cw.copyCW(exmo);
}

void RW(MOWB mowb){
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
    };
    IM = machineCode;

    PC = 0;
    
    while(1) {
        RW(mowb);
        MA(mowb, exmo, idex);
        IE(exmo, idex, ifid);
        ID(idex, ifid);
        IF(ifid);
    }
    
    return 0;
}