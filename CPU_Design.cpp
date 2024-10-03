#include <bits/stdc++.h>
using namespace std;

int PC;
int DM[1024];
string IM, GPR;

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
    string imm, func, rdl, CW, rs1, rs2;
    int JPC, DPC;
    
public:
    IDEX(){
        
    }
};

class EXMO{
public:
    string rs2, rdl, CW;
    int ALUOUT;
    
public:
    EXMO(){
        
    }
};

class MOWB{
public:
    string rdl, CW;
    int ALUOUT, LDOUT;
    
public:
    MOWB(){
        
    }
};

void IF(IFID ifid){
    
}

void ID(IDEX idex, IFID ifid){
    
}

void IE(EXMO exmo, IDEX idex){
    
}

void MA(MOWB mowb, EXMO exmo){
    
}

void RW(MOWB mowb){
    
}

int main () {
    IFID ifid;
    IDEX idex;
    EXMO exmo;
    MOWB mowb;
    
    vector<string> machineCode = {
        "00000000000100010000000110110011"
    }
    
    PC = 0;
    
    while(1) {
        RW(mowb);
        MA(mowb, exmo);
        IE(exmo, idex);
        ID(idex, ifid);
        IF(ifid);
    }
    
    return 0;
}