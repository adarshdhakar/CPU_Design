#include <bits/stdc++.h>
using namespace std;

map<string, int> labels;
unordered_map<string, vector<string>> functions;

void addRTypeInstructions()
{
    functions["add"] = {"R", "000", "0000000"};
    functions["sub"] = {"R", "000", "0100000"};
    functions["sll"] = {"R", "001", "0000000"};
    functions["slt"] = {"R", "010", "0000000"};
    functions["sltu"] = {"R", "011", "0000000"};
    functions["xor"] = {"R", "100", "0000000"};
    functions["srl"] = {"R", "101", "0000000"};
    functions["sra"] = {"R", "101", "0100000"};
    functions["or"] = {"R", "110", "0000000"};
    functions["and"] = {"R", "111", "0000000"};
}

void addITypeInstructions()
{
    functions["jalr"] = {"I", "000"};
    functions["addi"] = {"I", "000"};
    functions["slti"] = {"I", "010"};
    functions["sltiu"] = {"I", "011"};
    functions["xori"] = {"I", "100"};
    functions["ori"] = {"I", "110"};
    functions["andi"] = {"I", "111"};

    functions["lb"] = {"I", "000"};
    functions["lh"] = {"I", "001"};
    functions["lw"] = {"I", "010"};
    functions["lbu"] = {"I", "100"};
    functions["lhu"] = {"I", "101"};
}

void addSTypeInstructions()
{
    functions["sb"] = {"S", "000"};
    functions["sh"] = {"S", "001"};
    functions["sw"] = {"S", "010"};
}

void addBTypeInstructions()
{
    functions["beq"] = {"B", "000"};
    functions["bne"] = {"B", "001"};
    functions["blt"] = {"B", "100"};
    functions["bge"] = {"B", "101"};
    functions["bltu"] = {"B", "110"};
    functions["bgeu"] = {"B", "111"};
}

void addUTypeInstructions()
{
    functions["lui"] = {"U"};
    functions["auipc"] = {"U"};
}

void addJTypeInstructions()
{
    functions["jal"] = {"J"};
}

void initializeFunctions()
{
    addRTypeInstructions();
    addITypeInstructions();
    addSTypeInstructions();
    addBTypeInstructions();
    addUTypeInstructions();
    addJTypeInstructions();
}

unordered_map<string, string> registers = {
    {"zero", "x0"},
    {"ra", "x1"},
    {"sp", "x2"},
    {"gp", "x3"},
    {"tp", "x4"},
    {"t0", "x5"},
    {"t1", "x6"},
    {"t2", "x7"},
    {"s0", "x8"},
    {"fp", "x8"},
    {"s1", "x9"},
    {"a0", "x10"},
    {"a1", "x11"},
    {"a2", "x12"},
    {"a3", "x13"},
    {"a4", "x14"},
    {"a5", "x15"},
    {"a6", "x16"},
    {"a7", "x17"},
    {"s2", "x18"},
    {"s3", "x19"},
    {"s4", "x20"},
    {"s5", "x21"},
    {"s6", "x22"},
    {"s7", "x23"},
    {"s8", "x24"},
    {"s9", "x25"},
    {"s10", "x26"},
    {"s11", "x27"},
    {"t3", "x28"},
    {"t4", "x29"},
    {"t5", "x30"},
    {"t6", "x31"},
};

string hexDigitToBinary(char hexDigit)
{
    switch (toupper(hexDigit))
    {
    case '0':
        return "0000";
    case '1':
        return "0001";
    case '2':
        return "0010";
    case '3':
        return "0011";
    case '4':
        return "0100";
    case '5':
        return "0101";
    case '6':
        return "0110";
    case '7':
        return "0111";
    case '8':
        return "1000";
    case '9':
        return "1001";
    case 'A':
        return "1010";
    case 'B':
        return "1011";
    case 'C':
        return "1100";
    case 'D':
        return "1101";
    case 'E':
        return "1110";
    case 'F':
        return "1111";
    default:
        return "";
    }
}

class Instruction
{
public:
    string machineCode;

    char flip(char c)
    {
        return (c == '0') ? '1' : '0';
    }

    string twosComplementor(string bin)
    {
        int n = bin.length();
        int i;

        string ones, twos;
        ones = twos = "";

        for (i = 0; i < n; i++)
            ones += flip(bin[i]);

        twos = ones;
        for (i = n - 1; i >= 0; i--)
        {
            if (ones[i] == '1')
                twos[i] = '0';
            else
            {
                twos[i] = '1';
                break;
            }
        }

        if (i == -1)
            twos = '1' + twos;

        return twos;
    }

    string hexToBin(string hex)
    {
        string binary = "";
        for (char hexDigit : hex)
        {
            string bin = hexDigitToBinary(hexDigit);
            if (!bin.empty())
            {
                binary += bin;
            }
        }

        if (binary.length() > 32)
        {
            binary = binary.substr(binary.length() - 32);
        }
        else if (binary.length() < 32)
        {
            binary = string(32 - binary.length(), '0') + binary;
        }

        return binary;
    }

    string decimalToBinary(const string &decimalStr)
    {
        int decimal = stoi(decimalStr);
        string binaryStr = bitset<32>(decimal).to_string();

        if (binaryStr.empty())
        {
            binaryStr = "0";
        }

        return binaryStr;
    }

    string tolowerString(const string &func)
    {
        string str = "";
        for (const char &it : func)
        {
            str.push_back(tolower(it));
        }
        return str;
    }

    void makeStrLenBin(string &s, int n)
    {
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        {
            s.erase(0, 2);
            s = hexToBin(s);
        }
        else
        {
            s = decimalToBinary(s);
        }
        s.erase(0, 32 - n);
    }

    string findRegister(string s)
    {
        if (s[0] != 'x' && s[0] != 'X')
        {
            if (registers.find(s) != registers.end())
            {
                s = registers[s];
            }
        }

        s.erase(0, 1);
        makeStrLenBin(s, 5);
        return s;
    }

    void labelToImme(string &str, int n, int lineNum)
    {
        int offset = lineNum - labels[str] - 1;
        offset--;

        // cout << str << endl;
        if (offset > 0)
        {
            str = to_string(offset);
            makeStrLenBin(str, n);
            str = twosComplementor(str);
        }
        else
        {
            offset = -1 * offset;
            offset--;
            str = to_string(offset);
            makeStrLenBin(str, n);
        }
    }

    string getMachineCode()
    {
        return machineCode;
    }
};

class R : public Instruction
{
private:
    string opcode = "0110011", rd, func3, rs1, rs2, func7;

    void joinCodes()
    {
        cout << func7 << " " << rs2 << " " << rs1 << " " << func3 << " " << rd << " " << opcode << endl;
        machineCode = func7 + rs2 + rs1 + func3 + rd + opcode;
    }

public:
    R(vector<string> inst_break, vector<string> modeAndFunc)
    {
        this->rd = inst_break[1];
        this->rs1 = inst_break[2];
        this->rs2 = inst_break[3];

        rd = tolowerString(rd.substr(0, rd.find(',')));
        rs1 = tolowerString(rs1.substr(0, rs1.find(',')));

        rd = findRegister(rd);
        rs1 = findRegister(rs1);
        rs2 = findRegister(rs2);

        this->func3 = modeAndFunc[1];
        this->func7 = modeAndFunc[2];
        joinCodes();
    }
};

class I : public Instruction
{
private:
    string opcode = "0010011", rd, func3, rs1, imm;

    void joinCodes()
    {
        machineCode = imm + rs1 + func3 + rd + opcode;
    }

public:
    I(vector<string> inst_break, vector<string> modeAndFunc)
    {
        this->rd = inst_break[1];
        this->rs1 = inst_break[2];
        this->imm = inst_break[3];

        rd = tolowerString(rd.substr(0, rd.find(',')));
        rs1 = tolowerString(rs1.substr(0, rs1.find(',')));

        if (tolower(inst_break[0][0]) == 'l')
        {
            swap(rs1, imm);
            opcode = "0000011";
        }

        rd = findRegister(rd);
        rs1 = findRegister(rs1);
        makeStrLenBin(imm, 12);

        this->func3 = modeAndFunc[1];
        joinCodes();
    }
};

class S : public Instruction
{
private:
    string opcode = "0100011", imm1, func3, rs1, rs2, imm2;

    void joinCodes()
    {
        machineCode = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
    }

public:
    S(vector<string> inst_break, vector<string> modeAndFunc)
    {
        this->rs2 = inst_break[1];
        string str = inst_break[2];

        makeStrLenBin(str, 12);

        this->imm2 = str.substr(0, 7);
        this->imm1 = str.substr(7, 12);
        this->rs1 = inst_break[3];

        rs2 = rs2.substr(0, rs2.find(','));

        rs2 = findRegister(rs2);
        rs1 = findRegister(rs1);

        this->func3 = modeAndFunc[1];
        joinCodes();
    }
};

class B : public Instruction
{
private:
    string opcode = "1100011", imm1, func3, rs1, rs2, imm2;

    void joinCodes()
    {
        machineCode = imm2 + rs1 + rs2 + func3 + imm1 + opcode;
    }

public:
    B(vector<string> inst_break, vector<string> modeAndFunc, int lineNum)
    {
        this->rs2 = inst_break[1];
        this->rs1 = inst_break[2];

        rs2 = rs2.substr(0, rs2.find(','));
        rs1 = rs1.substr(0, rs1.find(','));

        rs2 = findRegister(rs2);
        rs1 = findRegister(rs1);

        string str = inst_break[3];
        // cout << str << endl;
        labelToImme(str, 12, lineNum);
        // cout << str << endl;
        this->imm2 = str.substr(0, 7);
        this->imm1 = str.substr(7, 12);
        // cout << this->imm2 << " " << this-> imm1 << endl;

        // labelToImme(str,  13, lineNum);
        // str.erase(str.size() - 1);
        // this->imm2 = str[0] + str.substr(2, 6);
        // this->imm1 = str.substr(8, 4) + str[1];

        this->func3 = modeAndFunc[1];
        joinCodes();
    }
};

class U : public Instruction
{
private:
    string opcode = "0110111", rd, imm;

    void joinCodes()
    {
        machineCode = imm + rd + opcode;
    }

public:
    U(vector<string> inst_break)
    {
        this->rd = inst_break[1];
        this->imm = inst_break[2];

        rd = tolowerString(rd.substr(0, rd.find(',')));
        rd = findRegister(rd);

        makeStrLenBin(imm, 20);

        joinCodes();
    }
};

class J : public Instruction
{
private:
    string opcode = "1101111", rd, imm;

    void joinCodes()
    {
        machineCode = imm + rd + opcode;
    }

public:
    J(vector<string> inst_break, int lineNum)
    {
        this->rd = inst_break[1];
        rd = tolowerString(rd.substr(0, rd.find(',')));
        rd = findRegister(rd);

        this->imm = inst_break[2];
        labelToImme(imm, 20, lineNum);

        // labelToImme(imm, 21, lineNum);
        // imm.erase(imm.size() - 1);
        // string str = imm;
        // imm = str[0] + str.substr(10, 10) + str[9] + str.substr(1,8);

        joinCodes();
    }
};

class Assembler
{
private:
    string tolowerString(const string &func)
    {
        string str = "";
        for (const char &it : func)
        {
            str.push_back(tolower(it));
        }
        return str;
    }

    vector<string> getInfo(string func)
    {
        func = tolowerString(func);
        if (functions.find(func) != functions.end())
        {
            return functions[func];
        }
        return {""};
    }

    void removeBrackets(string &instruction)
    {
        int n = instruction.size();
        for (int i = 0; i < n; i++)
        {
            if (instruction[i] == '(' || instruction[i] == ')')
            {
                instruction[i] = ' ';
            }
        }
    }

    string help_assembler(vector<string> &inst_break, vector<string> &modeAndFunc, int lineNum, string addMode)
    {
        string machineCode = "";
        if (addMode == "R")
        {
            R rMode = R(inst_break, modeAndFunc);
            machineCode = rMode.getMachineCode();
        }
        else if (addMode == "I")
        {
            I iMode = I(inst_break, modeAndFunc);
            machineCode = iMode.getMachineCode();
        }
        else if (addMode == "S")
        {
            S sMode = S(inst_break, modeAndFunc);
            machineCode = sMode.getMachineCode();
        }
        else if (addMode == "B")
        {
            B bMode = B(inst_break, modeAndFunc, lineNum);
            machineCode = bMode.getMachineCode();
        }
        else if (addMode == "U")
        {
            U uMode = U(inst_break);
            machineCode = uMode.getMachineCode();
        }
        else if (addMode == "J")
        {
            J jMode = J(inst_break, lineNum);
            machineCode = jMode.getMachineCode();
        }

        return machineCode;
    }

public:
    Assembler()
    {
        initializeFunctions();
    }

    string assemble(string &instruction, int lineNum)
    {
        removeBrackets(instruction);

        vector<string> inst_break;
        stringstream s(instruction);
        string word;

        while (getline(s, word, ' '))
        {
            inst_break.push_back(word);
        }

        vector<string> modeAndFunc = getInfo(inst_break[0]);
        string addMode = modeAndFunc[0];

        string machineCode = help_assembler(inst_break, modeAndFunc, lineNum, addMode);

        return machineCode;
    }
};

void readCodeAndFindLabel(vector<string> &assemblyCode)
{
    ifstream fin;
    fin.open("Assembly.txt");

    string line;
    int count = 1;

    while (getline(fin, line))
    {
        if (line.find(" ") == string::npos)
        {
            line.pop_back();
            labels[line] = count;
        }
        assemblyCode.push_back(line);
        // cout << line << endl;
        count++;
    }

    fin.close();
}

vector<string> convertToMachineCode(Assembler assembler, vector<string> &assemblyCode)
{
    vector<string> machineCode;
    // ofstream fout;
    // fout.open("MachineCode.txt");

    int count = 0;
    string line;
    for (int i = 1; i <= assemblyCode.size(); i++)
    {
        line = assemblyCode[i - 1];
        if (line.find(" ") != string::npos)
        {
            string str = assembler.assemble(line, i);
            machineCode.push_back(str);
            // fout << str << endl;
        }
        count++;
    }

    // fout.close();
    return machineCode;
}

void printCodes(vector<string> &code)
{
    cout << endl;
    for (auto &it : code)
    {
        // cout << "\"" << it << "\"," << endl;
        cout << it << endl;
    }
}

//--------------------------------------------------------------------Assembler Ends--------------------------------------------------------------------//

//--------------------------------------------------------------------CPU Starts--------------------------------------------------------------------//

int PC;
int DM[1024] = {0};
vector<string> IM;
int N;

class Valid
{
public:
    bool PC, ifid, idex, exmo, mowb;
    Valid()
    {
        this->PC = true;
        this->ifid = false;
        this->idex = false;
        this->exmo = false;
        this->mowb = false;
    }
};
Valid valid;

bitset<32> registerLock;

bool stopForOneTime = false;
bool bubbleInserted_data = false;
bool bubbleInserted_control = false;

int GPR[32] = {0};

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
    DM[11] = 10; //Fibonacci
    DM[1] = 1;
    DM[0] = 10; //N-Sum
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

void detectHazard(string controlWord_s3, string registerDestination_s4, string registerSource1_s3, string registerSource2_s3)
{
    if (controlWord_s3 == "0110011" || controlWord_s3 == "1100011")
    {
        if (registerDestination_s4 == registerSource1_s3 || registerDestination_s4 == registerSource2_s3)
        {
            bubbleInserted_data = true;
            registerLock[stoi(registerDestination_s4, NULL, 2)] = 1;
            cout << "data bubble inserted" << endl;
            cout << "inserted at R/B type" << endl;
        }
    }
    if (controlWord_s3 == "0010011" || controlWord_s3 == "0000011")
    {
        if (registerDestination_s4 == registerSource1_s3)
        {
            bubbleInserted_data = true;
            registerLock[stoi(registerDestination_s4, NULL, 2)] = 1;
            cout << "data bubble inserted" << endl;
            cout << "inserted at load/store" << endl;
        }
    }
    if (controlWord_s3 == "1100011" || controlWord_s3 == "1101111")
    {
        bubbleInserted_control = true;
        cout << "control bubble inserted" << endl;
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
    if (bubbleInserted_data || bubbleInserted_control)
    {
        cout << "stopped due to bubble insertion" << endl;
        return;
    }

    if (PC < N * 4)
    {
        int idx = PC / 4;
        ifid.IR = IM[idx];
        ifid.DPC = PC;
        PC = PC + 4;
        valid.ifid = true;
    }
    else
    {
        valid.PC = false;
    }
}

void ID(IDEX &idex, IFID &ifid, EXMO &exmo)
{
    if (!valid.PC)
    {
        valid.ifid = false;
        return;
    }

    if (stopForOneTime)
    {
        stopForOneTime = false;
        return;
    }

    if (bubbleInserted_data || bubbleInserted_control)
    {
        cout << "stage2 stopped due to bubble insertion" << endl;
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

    valid.idex = true;

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
    if (!valid.ifid)
    {
        valid.idex = false;
        return;
    }

    if (bubbleInserted_data)
    {
        cout << "stage 3 stopped due to bubbleInserted" << endl;
        return;
    }

    if (stopForOneTime)
    {
        cout << "stopped for one time 1" << endl;
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

    if (idex.cw.Branch)
    {
        if (ALUZeroFlag)
        {
            cout << "to_int(idex.imm2)*4 + ifid.DPC " << to_int(idex.imm2) * 4 + idex.DPC << endl;
            PC = to_int(idex.imm2) * 4 + idex.DPC;
        }
        if (bubbleInserted_control)
            stopForOneTime = true;
        bubbleInserted_control = false;
    }

    if (idex.cw.Jump)
    {
        PC = idex.JPC;
        if (bubbleInserted_control)
            stopForOneTime = true;
        bubbleInserted_control = false;
    }
    exmo.rdl = idex.rdl;
    exmo.rs2 = idex.rs2;

    valid.exmo = true;
}

void MA(MOWB &mowb, EXMO &exmo)
{
    if (!valid.idex)
    {
        valid.exmo = false;
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

    valid.mowb = true;
}

void RW(MOWB &mowb)
{
    if (!valid.exmo)
    {
        valid.mowb = false;
        return;
    }

    bool toUnlockRegister = false;

    if (mowb.cw.RegWrite)
    {
        if (mowb.cw.Mem2Reg)
        {
            GPR[to_int(mowb.rdl)] = mowb.LDOUT;
            if (registerLock[stoi(mowb.rdl, NULL, 2)] == 1)
                toUnlockRegister = true;
        }
        else
        {
            GPR[to_int(mowb.rdl)] = mowb.ALUOUT;
            if (registerLock[stoi(mowb.rdl, NULL, 2)] == 1)
                toUnlockRegister = true;
        }
    }

    if (toUnlockRegister)
    {
        bubbleInserted_data = false;
        stopForOneTime = true;
        registerLock[stoi(mowb.rdl, NULL, 2)] = 0;
    }
}

int main()
{
    vector<string> assemblyCode = {
        // "lw x5, 11(x6)", // int n = 10
        // "addi x1, x1, 1",
        // "addi x5, x5, 1",
        // "sum_loop:",
        // "beq x1, x5, done",
        // "add x2, x2, x1",
        // "addi x1, x1, 1",
        // "jal x3, sum_loop",
        // "done:",
        // "sw x2, 0(x31)"

        "lw x1, 0(x0)",
        "beq x1, x0, done",
        "addi x3, x3, 1",
        "beq x1, x3, done",
        "addi x2, x0, 1",
        "addi x4, x4, 1",
        "for:",
        "beq x2, x1, done",
        "add x3, x4, x5",
        "add x5, x4, x0",
        "add x4, x3, x0",
        "addi x2, x2, 1",
        "jal x6, for",
        "done:",
        "sw x3, 1(x0)"

        // "lw x1, 0(x0)",
        // "addi x1, x1 , 1",
        // "addi x2, x0, 1",
        // "for:",
        // "beq x2, x1, done",
        // "add x3, x3, x2",
        // "addi x2, x2, 1",
        // "jal x4, for",
        // "done:",
        // "sw x3, 1(x0)"
    };
    // readCodeAndFindLabel(assemblyCode);
    int count = 0;
    for (int i = 0; i < assemblyCode.size(); i++)
    {
        string line = assemblyCode[i];
        if (line.find(" ") == string::npos)
        {
            line.pop_back();
            labels[line] = count;
        }
        count++;
    }
    for (auto &label : labels)
    {
        cout << label.first << " " << label.second << endl;
    }

    Assembler assembler;
    vector<string> machineCode = convertToMachineCode(assembler, assemblyCode);

    printCodes(machineCode);

//--------------------------------------------------------------------Assembly to Machine Code Conversion Done--------------------------------------------------------------------//

//--------------------------------------------------------------------Machine Code to Output Starts--------------------------------------------------------------------//
    
    IFID ifid;
    IDEX idex;
    EXMO exmo;
    MOWB mowb;

    IM = machineCode;
    PC = 0;
    initControlUnit();
    initDM();
    int n = machineCode.size();
    N = n;

    while (PC < n * 4 || valid.PC || valid.ifid || valid.idex || valid.exmo || valid.mowb)
    {
        if (valid.mowb)
        {
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
        if (valid.exmo)
        {
            MA(mowb, exmo);
            cout << "stage4" << endl;
            cout << endl;
            cout << "mowb.ALUOUT " << mowb.ALUOUT << endl;
            cout << "mowb.LDOUT " << mowb.LDOUT << endl;
            cout << "mowb.rdl " << mowb.rdl << endl;
        }
        if (valid.idex)
        {
            IE(exmo, idex);
            cout << "stage3" << endl;
            cout << endl;
            cout << "ALUOUT " << exmo.ALUOUT << endl;
        }
        if (valid.ifid)
        {
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
        if (valid.PC)
        {
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