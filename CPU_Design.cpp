#include <bits/stdc++.h>
using namespace std;

int PC;
int DM[1024]={0};
vector<string> IM;
int N;

bitset<32> registerLock;
bool stage1Active = true;
bool stage2Active = false;
bool stage3Active = false;
bool stage4Active = false;
bool stage5Active = false;
bool stopForOneTime_1 = false;
bool stopForOneTime_2 = false;

bool bubbleInserted_data = false;
bool bubbleInserted_control = false;

int GPR[32]={0};

class controlWord
{
public:
    int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
};

map<string, controlWord> controlUnit;

void initControlUnit()
{
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

void initDM()
{
    // for (int i = 0; i < 1024; i++)
    // {
    //     DM[i] = 2 * i;
    // }
    // DM[11] = 10;
    // DM[1] = 1;
    DM[0] = 1;
}

class IFID
{
public:
    string IR;
    int DPC, NPC;
};

class IDEX
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        void controller(string str)
        {
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
    string imm1, imm2, func, rdl, rs1, rs2, IR;
    int JPC, DPC;
    CW cw;
};

class EXMO
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;

    public:
        void copyCW(IDEX &idex)
        {
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
    string rdl, rs2;
    int ALUOUT;
    CW cw;
};

class MOWB
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;

    public:
        void copyCW(EXMO exmo)
        {
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

int to_int(string binaryStr)
{
    int decimalValue = 0;
    int length = binaryStr.length();
    bool isNegative = (binaryStr[0] == '1');

    if (!isNegative)
    {
        for (int i = 0; i < length; i++)
        {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1')
            {
                decimalValue += (1 << i);
            }
        }
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1' && i != length - 1)
            {
                decimalValue += (1 << i);
            }
        }

        decimalValue -= (1 << (length - 1));
    }

    return decimalValue;
}

void detectHazard(string controlWord_s3, string registerDestination_s4, string registerSource1_s3, string registerSource2_s3){
    if(controlWord_s3 == "0110011" || controlWord_s3 == "1100011"){
        if (registerDestination_s4 == registerSource1_s3 || registerDestination_s4 == registerSource2_s3) {
            bubbleInserted_data = true;
            registerLock[stoi(registerDestination_s4,NULL,2)]=1;
            cout<<"data bubble inserted"<<endl;
            cout << "inserted at R/B type" << endl;
        }
    }
    if(controlWord_s3 == "0010011" || controlWord_s3 == "0000011"){
        if (registerDestination_s4 == registerSource1_s3) {
            bubbleInserted_data = true;
            registerLock[stoi(registerDestination_s4,NULL,2)]=1;
            cout<<"data bubble inserted"<<endl;
            cout << "inserted at load/store" <<endl;
        }
    }
    if(controlWord_s3 == "1100011" || controlWord_s3 == "1101111"){
        bubbleInserted_control = true;
        cout<<"control bubble inserted"<<endl;
    } 
}

string to_str(int num)
{
    cout << num << endl;
    bitset<32> binary(num);
    return binary.to_string();
}

int SignedExtend(string imm)
{
    string extended_imm = imm;
    if (imm[0] == '0')
    {
        extended_imm = "000000000000" + extended_imm;
    }
    else
    {
        extended_imm = "111111111111" + extended_imm;
    }

    return to_int(extended_imm);
}

void IF(IFID &ifid)
{
    if(bubbleInserted_data || bubbleInserted_control){
        cout << "stopped due to bubble insertion" << endl;
        return;
    }

    if(PC < N*4){
        int idx = PC / 4;
        ifid.IR = IM[idx];
        ifid.DPC = PC;
        PC = PC + 4;
        stage2Active = true;
    }
    else {
        stage1Active = false;
    }
}

void ID(IDEX &idex, IFID &ifid, EXMO &exmo)
{
    if(!stage1Active){
        stage2Active = false;
        return;
    }

    if(stopForOneTime_1){
        stopForOneTime_1 = false;
        return;
    }

    if(bubbleInserted_data || bubbleInserted_control){
        cout<< "stage2 stopped due to bubble insertion"<<endl;
        return;
    }

    string ir = ifid.IR;
    idex.JPC = ifid.DPC + 4 * SignedExtend(ir.substr(0, 20));
    idex.DPC = ifid.DPC;
    idex.imm1 = ir.substr(0, 12);
    idex.imm2 = ir.substr(0, 7) + ir.substr(20, 5);
    idex.func = ir.substr(17, 3);
    idex.rdl = ir.substr(20, 5);
    idex.cw.controller(ir.substr(25, 7));
    idex.IR = ifid.IR;

    stage3Active = true;
    
    detectHazard(idex.IR.substr(25, 7), exmo.rdl, ir.substr(12, 5), ir.substr(7, 5));
}

string ALUControl(int ALUOp, string func, string func7)
{
    string ALUSelect;

    switch (ALUOp)
    {
    case 00:                // Load/Store (for I-type, S-type)
        ALUSelect = "0010"; // Perform ADD for address calculation
        break;
    case 01: // Branch (for B-type)
        if (func == "000")
        {                       // beq
            ALUSelect = "0110"; // Perform SUB for comparison
        }
        else if (func == "001")
        {                       // bne
            ALUSelect = "0110"; // SUB for comparison
        }
        else
        {
            ALUSelect = "1111"; // Undefined
        }
        break;
    case 10: // R-type instructions
        if (func == "000")
        {                                                     // ADD
            ALUSelect = func7 == "0000000" ? "0010" : "0110"; // ADD or SUB
        }
        else if (func == "001")
        {                       // SLL
            ALUSelect = "0011"; // Shift left
        }
        else if (func == "111")
        { // AND
            ALUSelect = "0000";
        }
        else if (func == "110")
        { // OR
            ALUSelect = "0001";
        }
        else
        {
            ALUSelect = "1111"; // Undefined
        }
        break;
    case 11: // I-type instructions
        if (func == "000")
        {                       // ADDI
            ALUSelect = "0010"; // ADD immediate
        }
        else if (func == "010")
        {                       // SLTI
            ALUSelect = "0111"; // Set less than immediate
        }
        else if (func == "100")
        {                       // XORI
            ALUSelect = "0011"; // XOR immediate
        }
        else
        {
            ALUSelect = "1111"; // Undefined
        }
        break;
    default:
        ALUSelect = "1111"; // Undefined operation
        break;
    }

    return ALUSelect;
}

int ALU(string ALUSelect, string rs1, string rs2)
{
    int operand1 = stoi(rs1, NULL, 2);
    int operand2 = stoi(rs2, NULL, 2);
    int result = 0;

    if (ALUSelect == "0000")
    { // AND
        result = operand1 & operand2;
    }
    else if (ALUSelect == "0001")
    { // OR
        result = operand1 | operand2;
    }
    else if (ALUSelect == "0010")
    { // ADD
        result = operand1 + operand2;
    }
    else if (ALUSelect == "0110")
    { // SUB
        result = operand1 - operand2;
    }
    else if (ALUSelect == "0011")
    {                                           // SLL
        result = operand1 << (operand2 & 0x1F); // Logical Shift Left
    }
    else if (ALUSelect == "0100")
    {                                           // SRL
        result = operand1 >> (operand2 & 0x1F); // Logical Shift Right
    }
    else if (ALUSelect == "0101")
    {                                           // SRA
        result = operand1 >> (operand2 & 0x1F); // Arithmetic Shift Right
    }
    else if (ALUSelect == "0111")
    { // SLT (Set Less Than)
        result = (operand1 < operand2) ? 1 : 0;
    }
    else if (ALUSelect == "1000")
    { // SLTU (Set Less Than Unsigned)
        result = ((unsigned int)operand1 < (unsigned int)operand2) ? 1 : 0;
    }
    else if (ALUSelect == "1111")
    { // Undefined operation
        std::cerr << "Error: Undefined ALU operation." << std::endl;
        return -1; // Return -1 to indicate an error
    }

    return result;
}

void IE(EXMO &exmo, IDEX &idex)
{
    if(!stage2Active){
        stage3Active = false;
        return;
    }

    if(bubbleInserted_data){
        cout<<"stage 3 stopped due to bubbleInserted"<<endl;
        return;
    }

    if(stopForOneTime_1){
        cout<<"stopped for one time 1"<<endl;
        return;
    }

    string ir = idex.IR;

    if (idex.cw.RegRead)
    {
        cout << "ir.substr(12, 5)" << " " << ir.substr(12, 5) << endl;
        cout << "GPR[to_int(ir.substr(12, 5))]" << " " << GPR[to_int(ir.substr(12, 5))] << endl;
        idex.rs1 = to_str(GPR[to_int(ir.substr(12, 5))]);
    }
    if (idex.cw.ALUSrc && (ir.substr(25, 7) == "0010011" || ir.substr(25, 7) == "0000011"))
    {
        if (idex.cw.RegRead)
        {
            cout << "idex.imm1" << " " << idex.imm1 << endl;
            idex.rs2 = idex.imm1;
        }
    }
    else
    {
        if (idex.cw.RegRead)
        {
            cout << "ir.substr(7, 5)" << " " << ir.substr(7, 5) << endl;
            cout << "GPR[to_int(ir.substr(7, 5))]" << " " << GPR[to_int(ir.substr(7, 5))] << endl;
            idex.rs2 = to_str(GPR[to_int(ir.substr(7, 5))]);
        }
    }

    string ALUSelect = ALUControl(idex.cw.ALUOp, idex.func, idex.IR.substr(0, 7));
    cout << "ALUSelect " << ALUSelect << endl;
    string opcode = idex.IR.substr(25, 7);
    if (opcode == "0100011" || opcode == "1100011")
    {
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.imm2);
    }
    else
    {
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.rs2);
    }
    int ALUZeroFlag = (idex.rs1 == idex.rs2);
    exmo.cw.copyCW(idex);

    if (idex.cw.Branch){
        if(ALUZeroFlag){
            cout << "to_int(idex.imm2)*4 + ifid.DPC " << to_int(idex.imm2) * 4 + idex.DPC << endl;
            PC = to_int(idex.imm2) * 4 + idex.DPC;
        }
        if(bubbleInserted_control) stopForOneTime_1 = true;
        bubbleInserted_control = false;
    }

    if (idex.cw.Jump)
    {
        PC = idex.JPC;
        if(bubbleInserted_control) stopForOneTime_1 = true;
        bubbleInserted_control = false;
    }
    exmo.rdl = idex.rdl;
    exmo.rs2 = idex.rs2;

    stage4Active = true;  
}

void MA(MOWB &mowb, EXMO &exmo)
{
    if(!stage3Active) {
        stage4Active = false;
        return;
    }

    cout << "exmo.ALUOUT " << exmo.ALUOUT << endl;
    if (exmo.cw.MemWrite)
    {
        DM[exmo.ALUOUT] = to_int(exmo.rs2);
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }
    if (exmo.cw.MemRead)
    {
        mowb.LDOUT = DM[exmo.ALUOUT];
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }

    mowb.ALUOUT = exmo.ALUOUT;
    mowb.cw.copyCW(exmo);
    mowb.rdl = exmo.rdl;

    stage5Active = true;  
}

void RW(MOWB &mowb)
{
    if (!stage4Active) {
        stage5Active = false;
        return;
    }

    bool toUnlockRegister = false;

    if (mowb.cw.RegWrite)
    {
        if (mowb.cw.Mem2Reg)
        {
            GPR[to_int(mowb.rdl)] = mowb.LDOUT;
            if(registerLock[stoi(mowb.rdl,NULL,2)]==1) toUnlockRegister = true;
        }
        else
        {
            GPR[to_int(mowb.rdl)] = mowb.ALUOUT;
            if(registerLock[stoi(mowb.rdl,NULL,2)]==1) toUnlockRegister = true;
        }
    }

    if(toUnlockRegister) {
        bubbleInserted_data = false;
        stopForOneTime_1 = true;
        registerLock[stoi(mowb.rdl,NULL,2)]=0;
    }
}

int main()
{
    IFID ifid;
    IDEX idex;
    EXMO exmo;
    MOWB mowb;

    vector<string> machineCode = {
        "00000000000000000010000010000011",
        "00000000000000001000011001100011",
        "00000000000100011000000110010011",
        "00000000001100001000010101100011",
        "00000000000100000000000100010011",
        "00000000000100100000001000010011",
        "00000000000100010000001101100011",
        "00000000010100100000000110110011",
        "00000000000000100000001010110011",
        "00000000000000011000001000110011",
        "00000000000100010000000100010011",
        "11111111111111111011001101101111",
        "00000000001100000010000010100011"
    };

    IM = machineCode;
    PC = 0;
    initControlUnit();
    initDM();
    int n = machineCode.size();
    N = n;

    while(PC < n*4 || stage1Active || stage2Active || stage3Active || stage4Active || stage5Active){
        if(stage5Active){
            RW(mowb);
            cout << "stage5" << endl;
            cout << endl;
            cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
            cout << "to_int(mowb.rdl) " << to_int(mowb.rdl) << endl;
            cout << "GPR[to_int(mowb.rdl)] " << GPR[to_int(mowb.rdl)] << endl;
            cout << "PC " << PC << endl;
            cout << "GPR[0] " << GPR[0] << endl;
            cout << "GPR[1] " << GPR[1] << endl;
            cout << "GPR[2] " << GPR[2] << endl;
            cout << "GPR[3] " << GPR[3] << endl;
            cout << "GPR[4] " << GPR[4] << endl;
            cout << "GPR[5] " << GPR[5] << endl;
            cout << "DM[0] " << DM[0] << endl;
            cout << "DM[1] " << DM[1] << endl;
        }
        if(stage4Active) {
            MA(mowb, exmo);
            cout << "stage4" << endl;
            cout << endl;
            cout << "mowb.ALUOUT " << mowb.ALUOUT << endl;
            cout << "mowb.LDOUT " << mowb.LDOUT << endl;
            cout << "mowb.rdl " << mowb.rdl << endl;
        }
        if(stage3Active) {
            IE(exmo, idex);
            cout << "stage3" << endl;
            cout << endl;
            cout << "ALUOUT " << exmo.ALUOUT << endl;
        }
        if(stage2Active) {
            cout << "stage2" << endl;
            ID(idex, ifid, exmo);
            
            cout << "imm1 " << idex.imm1 << endl;
            cout << "imm2 " << idex.imm2 << endl;
            cout << "func " << idex.func << endl;
            cout << "rdl " << idex.rdl << endl;
            cout << "rs1 " << idex.rs1 << endl;
            cout << "rs2 " << idex.rs2 << endl;
            cout << "JPC " << idex.JPC << endl;
            cout << "DPC " << idex.DPC << endl;
            cout << endl;
            cout << "RegRead " << idex.cw.RegRead << endl;
            cout << "RegWrite " << idex.cw.RegWrite << endl;
            cout << "ALUSrc " << idex.cw.ALUSrc << endl;
            cout << "ALUOp " << idex.cw.ALUOp << endl;
            cout << "Branch " << idex.cw.Branch << endl;
            cout << "Jump " << idex.cw.Jump << endl;
            cout << "MemRead " << idex.cw.MemRead << endl;
            cout << "MemWrite " << idex.cw.MemWrite << endl;
            cout << "Mem2Reg " << idex.cw.Mem2Reg << endl;
        }
        if(stage1Active) {
            cout << "stage1" << endl;
            cout << endl
             << "Instruction: " << PC / 4 << endl;
            IF(ifid);
            cout << "IR " << ifid.IR << endl;
            cout << "DPC " << ifid.DPC << endl;
            cout << "NPC " << ifid.NPC << endl;

            cout << endl;
        }
    }
    return 0;
}