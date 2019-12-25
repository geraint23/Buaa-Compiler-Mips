#ifndef GENMIPS_GENMIPS_H
#define GENMIPS_GENMIPS_H

#include "parser.h"
#include <fstream>
#include <map>
#include <utility>

using namespace std;

set<string> regSet;
map<string, string> addressMap;  //字符串和对应的地址名字 @t的偏移
vector<string> para_push;
vector<string> mipsCode;

set<string> t_in_mem;   //!!
map<string, string> varRegMap;  //!!

int local_space;

void add_t_to_mem(const string &t_name) {
    t_in_mem.insert(t_name);
}

bool must_get_from_mem(const string &t_name) {
    return (t_in_mem.find(t_name) != t_in_mem.end());
}

void addToMap(const string &var, string reg) {
    varRegMap[var] = std::move(reg);
}

bool isPowerOf2(unsigned int a) {
    return (a & (a - 1)) == 0;
}

int get_bit_offset(unsigned int a) {
    int cnt = 0;
    a >>= 1;
    while (a != 0) {
        a >>= 1;
        cnt++;
    }
    return cnt;
}

void releaseReg(const string &regName) {
    regSet.erase(regName);
}

string getConstName(SymTableptr s) {
    return s->name + "_level" + to_string(s->level);
}

string getReg() {
    for (int i = 0; i <= 9; i++) {
        string t = "$t" + to_string(i);
        if (regSet.find(t) == regSet.end()) {
            regSet.insert(t);
            return t;
        }
    }
    return "full";
}

string createLabel() {
    static int strCount = 1;
    string str = "GeraintString" + to_string(strCount);
    strCount++;
    return str;
}

bool isConst(SymTableptr s) {
    return s->type == CONST_INT || s->type == CONST_CHAR;
}

void genData(ofstream &mips) {  //.data段只存字符串和常量
    mipsCode.emplace_back(".data");//mips << ".data" << endl;
    level = 1;
    for (auto &iter : quadraExpVec) {
        if (!iter.arg1.empty() &&
            (iter.op == PRINT_STR || iter.op == PRINT_INT || iter.op == PRINT_CHAR)) {    //int和char的arg1可能有字符串
            if (addressMap.count(iter.arg1) == 0) {
                string label = createLabel();
                addressMap[iter.arg1] = label;
                mipsCode.emplace_back(label + ":.asciiz\"" + iter.arg1 +
                                      "\"");//mips << label << ":.asciiz\"" + iter.arg1 + "\"" << endl;
            }
        } else if (iter.op == CONST_INT_DEF || iter.op == CONST_CHAR_DEF) {
            if (iter.op == CONST_INT_DEF) {
                mipsCode.emplace_back(iter.result + "_level" + iter.arg2 + ":.word " + iter.arg1);
                //mips << iter.result + "_level" + iter.arg2 << ":.word " << iter.arg1 << endl;
            } else {
                mipsCode.emplace_back(iter.result + "_level" + iter.arg2 + ":.word \'" + iter.arg1 + "\'");
                //mips << iter.result + "_level" + iter.arg2 << ":.word \'" << iter.arg1 << "\'" << endl;
            }
        }
    }
    mipsCode.emplace_back(R"(newline:.asciiz"\n")");//mips << R"(newline:.asciiz"\n")" << endl;
}

void genGlobalVar(ofstream &mips) {
    int offset = 0;
    for (auto &iter : quadraExpVec) {
        if (iter.op == INT_DEF_OUTER || iter.op == CHAR_DEF_OUTER) {
            offset += 4;
        } else if (iter.op == INT_ARRAY_DEF_OUTER || iter.op == CHAR_ARRAY_DEF_OUTER) {
            offset += (4 * stoi(iter.arg1));
        }
    }
    int tmp = 0;
    for (auto &iter : quadraExpVec) {
        SymTableptr s;
        if (iter.op == INT_DEF_OUTER || iter.op == CHAR_DEF_OUTER) {
            s = findInSymTable(iter.result, level);
            tmp += 4;
            s->offset = to_string(offset - tmp);
        } else if (iter.op == INT_ARRAY_DEF_OUTER || iter.op == CHAR_ARRAY_DEF_OUTER) {
            s = findInSymTable(iter.result, level);
            tmp += (4 * stoi(iter.arg1));
            s->offset = to_string(offset - tmp);
        }
    }
    if (offset != 0) {
        mipsCode.emplace_back("subi, $sp, $sp, " + to_string(offset));
        //mips << "subi, $sp, $sp, " << offset << endl;
    }
    mipsCode.emplace_back("move, $gp, $sp");
    //mips << "move, $gp, $sp" << endl;
}

string getVarReg(string arg, ofstream &mips) {
    SymTableptr s = findInSymTable(arg, level);
    string reg;
    if (s != nullptr) {
        reg = getReg();
        if (isConst(s)) {        // 是常量，在.data
            mipsCode.emplace_back("lw " + reg + ", " + getConstName(s));
            //mips << "lw " << reg << ", " << getConstName(s) << endl;
        } else if (s->level == 1) {   //全局变量
            mipsCode.emplace_back("lw " + reg + ", " + s->offset + "($gp)");
            //mips << "lw " << reg << ", " << s->offset << "($gp)" << endl;
        } else {
            mipsCode.emplace_back("lw " + reg + ", " + s->offset + "($sp)");// 局部变量
            //mips << "lw " << reg << ", " << s->offset << "($sp)" << endl;
        }
    } else if (arg[0] != '@') {     // 数字或者字符
        reg = getReg();
        mipsCode.emplace_back("li " + reg + ", " + arg);
        //mips << "li " << reg << ", " << arg << endl;
    } else {                        // @t
        if (must_get_from_mem(arg)) {   //!!
            reg = getReg();
            mipsCode.emplace_back("lw " + reg + ", " + addressMap[arg] + "($sp)");
        } else {
            return varRegMap[arg];
        }
        //mips << "lw " << reg << ", " << addressMap[arg] << "($sp)" << endl;
    }
    return reg;
}

void printInt(ofstream &mips, const string &arg2) {
    SymTableptr s = findInSymTable(arg2, level);
    string reg;
    if (s != nullptr) {
        if (isConst(s)) {             //常量在.data
            mipsCode.emplace_back("lw $a0, " + getConstName(s));
            //mips << "lw $a0, " << getConstName(s) << endl;
        } else if (s->level == 1) {
            mipsCode.emplace_back("lw $a0, " + s->offset + "($gp)");//局部变量，在栈里
            //mips << "lw $a0, " << s->offset << "($gp)" << endl;
        } else {                         //局部变量，在栈里
            mipsCode.emplace_back("lw $a0, " + s->offset + "($sp)");//局部变量，在栈里
            //mips << "lw $a0, " << s->offset << "($sp)" << endl;
        }
    } else if (arg2[0] == '@') {
        reg = getVarReg(arg2, mips);
        mipsCode.emplace_back("move $a0, " + reg);  //!!
        releaseReg(varRegMap[arg2]);
        //mips << "lw $a0, " << addressMap[arg2] << "($sp)" << endl;
    } else {
        mipsCode.emplace_back("li $a0, " + arg2);
        //mips << "li $a0, " << arg2 << endl;
    }
    mipsCode.emplace_back("li $v0, 1");
    mipsCode.emplace_back("syscall");
    //mips << "li $v0, 1" << endl << "syscall" << endl;
}

void printChar(ofstream &mips, const string &arg2) {
    SymTableptr s = findInSymTable(arg2, level);
    string reg;
    if (s != nullptr) {
        if (isConst(s)) {             //常量在.data
            mipsCode.emplace_back("lw $a0, " + getConstName(s));
            //mips << "lw $a0, " << getConstName(s) << endl;
        } else if (s->level == 1) {                         //局部变量，在栈里
            mipsCode.emplace_back("lw $a0, " + s->offset + "($gp)");
            //mips << "lw $a0, " << s->offset << "($gp)" << endl;
        } else {
            mipsCode.emplace_back("lw $a0, " + s->offset + "($sp)");
            // mips << "lw $a0, " << s->offset << "($sp)" << endl;
        }
    } else if (arg2[0] == '@') {
        reg = getVarReg(arg2, mips);
        mipsCode.emplace_back("move $a0, " + reg);  //!!
        //mips << "lw $a0, " << addressMap[arg2] << "($sp)" << endl;
    } else {
        mipsCode.emplace_back("li $a0, " + arg2);
        //mips << "li $a0, " << arg2 << endl;
    }
    mipsCode.emplace_back("li $v0, 11");
    mipsCode.emplace_back("syscall");
    //mips << "li $v0, 11" << endl << "syscall" << endl;
}

void readInt(SymTableptr s, ofstream &mips) {
    mipsCode.emplace_back("li $v0, 5");
    mipsCode.emplace_back("syscall");
    //mips << "li $v0, 5" << endl << "syscall" << endl;
    if (s->level != 1) {
        mipsCode.emplace_back("sw $v0, " + s->offset + "($sp)");
        //mips << "sw $v0, " << s->offset << "($sp)" << endl;
    } else if (s->level == 1) {
        mipsCode.emplace_back("sw $v0, " + s->offset + "($gp)");//局部变量，在栈里
        //mips << "sw $v0, " << s->offset << "($gp)" << endl;
    }
}

void readChar(SymTableptr s, ofstream &mips) {
    mipsCode.emplace_back("li $v0, 12");
    mipsCode.emplace_back("syscall");
    //mips << "li $v0, 12" << endl << "syscall" << endl;
    if (s->level != 1) {
        mipsCode.emplace_back("sw $v0, " + s->offset + "($sp)");
        //mips << "sw $v0, " << s->offset << "($sp)" << endl;
    } else if (s->level == 1) {                         //局部变量，在栈里
        mipsCode.emplace_back("sw $v0, " + s->offset + "($gp)");
        //mips << "sw $v0, " << s->offset << "($gp)" << endl;
    }
}

void printNewline(ofstream &mips) {
    mipsCode.emplace_back("la $a0, newline");
    mipsCode.emplace_back("li $v0, 4");
    mipsCode.emplace_back("syscall");
    //mips << "la $a0, newline" << endl;
    //mips << "li $v0, 4" << endl << "syscall" << endl;
}

bool isInteger(string arg) {
    return isdigit(arg[0]);
}

void calculate(vector<QuadraExp>::iterator iter, ofstream &mips) {
    string arg1_reg;
    string arg2_reg;
    string result_reg = getReg();
    if (iter->op == PLUS_OP) {
        if (isInteger(iter->arg1) && !isInteger(iter->arg2)) {
            arg2_reg = getVarReg(iter->arg2, mips);
            mipsCode.emplace_back("addi " + result_reg + ", " + arg2_reg + ", " + iter->arg1);
            releaseReg(arg2_reg);
        } else if (!isInteger(iter->arg1) && isInteger(iter->arg2)) {
            arg1_reg = getVarReg(iter->arg1, mips);
            mipsCode.emplace_back("addi " + result_reg + ", " + arg1_reg + ", " + iter->arg2);
            releaseReg(arg1_reg);
        } else if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) + stoi(iter->arg2)));
        } else {
            arg1_reg = getVarReg(iter->arg1, mips);
            arg2_reg = getVarReg(iter->arg2, mips);
            mipsCode.emplace_back("add " + result_reg + ", " + arg1_reg + ", " + arg2_reg);
            releaseReg(arg1_reg);
            releaseReg(arg2_reg);
        }
        //mips << "add " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
    } else if (iter->op == MINUS_OP) {
        if (!isInteger(iter->arg1) && isInteger(iter->arg2)) {
            arg1_reg = getVarReg(iter->arg1, mips);
            mipsCode.emplace_back("subi " + result_reg + ", " + arg1_reg + ", " + iter->arg2);
            releaseReg(arg1_reg);
        } else if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) - stoi(iter->arg2)));
        } else {
            arg1_reg = getVarReg(iter->arg1, mips);
            arg2_reg = getVarReg(iter->arg2, mips);
            mipsCode.emplace_back("sub " + result_reg + ", " + arg1_reg + ", " + arg2_reg);
            releaseReg(arg1_reg);
            releaseReg(arg2_reg);
        }
        //mips << "sub " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
    } else if (iter->op == MULT_OP) {
//        int bit_offset;
//        if (isInteger(iter->arg1) && !isInteger(iter->arg2)) {
//            arg2_reg = getVarReg(iter->arg2, mips);
//            if (isPowerOf2(stoi(iter->arg1))) {
//                bit_offset = get_bit_offset(stoi(iter->arg1));
//                mipsCode.emplace_back("sll " + result_reg + ", " + arg2_reg + ", " + to_string(bit_offset));
//                releaseReg(arg2_reg);
//            } else {
//                arg1_reg = getVarReg(iter->arg1, mips);
//                mipsCode.emplace_back("mul " + result_reg + ", " + arg1_reg + ", " + arg2_reg);
//                releaseReg(arg1_reg);
//                releaseReg(arg2_reg);
//            }
//        } else if (!isInteger(iter->arg1) && isInteger(iter->arg2)) {
//            cout << iter->arg1 << endl;
//            arg1_reg = getVarReg(iter->arg1, mips);
//            if (isPowerOf2(stoi(iter->arg2))) {
//                bit_offset = get_bit_offset(stoi(iter->arg2));
//                cout << bit_offset << endl;
//                mipsCode.emplace_back("sll " + result_reg + ", " + arg1_reg + ", " + to_string(bit_offset));
//                releaseReg(arg1_reg);
//            } else {
//                arg2_reg = getVarReg(iter->arg2, mips);
//                mipsCode.emplace_back("mul " + result_reg + ", " + arg1_reg + ", " + arg2_reg);
//                releaseReg(arg1_reg);
//                releaseReg(arg2_reg);
//            }
//        } else if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
//            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) * stoi(iter->arg2)));
//            //mips << "mul " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
//        } else {
        if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) * stoi(iter->arg2)));
            //mips << "mul " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
        } else {
            arg1_reg = getVarReg(iter->arg1, mips);
            arg2_reg = getVarReg(iter->arg2, mips);
            mipsCode.emplace_back("mul " + result_reg + ", " + arg1_reg + ", " + arg2_reg);
            releaseReg(arg1_reg);
            releaseReg(arg2_reg);
        }
    } else if (iter->op == DIV_OP) {
//        int bit_offset;
//        if (!isInteger(iter->arg1) && isInteger(iter->arg2)) {
//            arg1_reg = getVarReg(iter->arg1, mips);
//            if (isPowerOf2(stoi(iter->arg2))) {
//                bit_offset = get_bit_offset(stoi(iter->arg2));
//                mipsCode.emplace_back("srl " + result_reg + ", " + arg1_reg + ", " + to_string(bit_offset));
//                releaseReg(arg1_reg);
//            } else {
//                arg2_reg = getVarReg(iter->arg2, mips);
//                mipsCode.emplace_back("div " + arg1_reg + ", " + arg2_reg);
//                releaseReg(arg1_reg);
//                releaseReg(arg2_reg);
//                mipsCode.emplace_back("mflo " + result_reg);
//            }
//        } else if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
//            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) / stoi(iter->arg2)));
//            //mips << "mul " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
//        } else {
        if (isInteger(iter->arg1) && isInteger(iter->arg2)) {
            mipsCode.emplace_back("li " + result_reg + ", " + to_string(stoi(iter->arg1) / stoi(iter->arg2)));
            //mips << "mul " << result_reg << ", " << arg1_reg << ", " << arg2_reg << endl;
        } else {
            arg1_reg = getVarReg(iter->arg1, mips);
            arg2_reg = getVarReg(iter->arg2, mips);
            mipsCode.emplace_back("div " + arg1_reg + ", " + arg2_reg);
            //mips << "div " << arg1_reg << ", " << arg2_reg << endl;
            releaseReg(arg1_reg);
            releaseReg(arg2_reg);
            mipsCode.emplace_back("mflo " + result_reg);
            //mips << "mflo " << result_reg << endl;
        }
    }
    addToMap(iter->result, result_reg); //!!
    //mipsCode.emplace_back("sw " + result_reg + ", " + addressMap[iter->result] + "($sp)");
    //mips << "sw " << result_reg << ", " << addressMap[iter->result] << "($sp)" << endl;
    //releaseReg(result_reg);
}

void varAssign(ofstream &mips, string arg1, const string &result) {
    SymTableptr result_s = findInSymTable(result, level);
    if (result_s != nullptr) {      // 给全局变量或局部变量赋值
        SymTableptr var_s = findInSymTable(arg1, level);
        if (arg1[0] == '@') {      // 用@t赋值
            string reg = getVarReg(arg1, mips);
            if (result_s->level == 1) {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($gp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($gp)" << endl;
            } else {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($sp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($sp)" << endl;
            }
            releaseReg(reg);
        } else if (var_s != nullptr) {
            string reg = getReg();
            if (isConst(var_s)) {        // 用常量赋值
                mipsCode.emplace_back("lw " + reg + ", " + getConstName(var_s));
                //mips << "lw " << reg << ", " << getConstName(var_s) << endl;
            } else if (var_s->level == 1) {    //用全局变量赋值
                mipsCode.emplace_back("lw " + reg + ", " + var_s->offset + "($gp)");
                //mips << "lw " << reg << ", " << var_s->offset << "($gp)" << endl;
            } else {                    // 用局部变量赋值
                mipsCode.emplace_back("lw " + reg + ", " + var_s->offset + "($sp)");
                //mips << "lw " << reg << ", " << var_s->offset << "($sp)" << endl;
            }
            if (result_s->level == 1) {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($gp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($gp)" << endl;
            } else {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($sp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($sp)" << endl;
            }
            releaseReg(reg);
        } else {                        // 直接用字符或数字
            string reg = getReg();
            mipsCode.emplace_back("li " + reg + ", " + arg1);
            //mips << "li " << reg << ", " << arg1 << endl;
            if (result_s->level == 1) {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($gp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($gp)" << endl;
            } else {
                mipsCode.emplace_back("sw " + reg + ", " + result_s->offset + "($sp)");
                //mips << "sw " << reg << ", " << result_s->offset << "($sp)" << endl;
            }
            releaseReg(reg);
        }
    }
}

string getOffset(const string &result, string arg2, ofstream &mips) {
    SymTableptr result_s = findInSymTable(result, level);   // 全局数组或者局部数组，肯定能找见
    string arg2_reg = getVarReg(std::move(arg2), mips);
    string base_reg = getReg();
    if (result_s->level == 1) {
        mipsCode.emplace_back("addi " + base_reg + ", " + "$gp, " + result_s->offset);
        //mips << "addi " + base_reg + ", " + "$gp, " + result_s->offset << endl;
    } else {
        mipsCode.emplace_back("addi " + base_reg + ", " + "$sp, " + result_s->offset);
        //mips << "addi " + base_reg + ", " + "$sp, " + result_s->offset << endl;
    }
    mipsCode.emplace_back("sll " + arg2_reg + ", " + arg2_reg + ", " + "2");
    mipsCode.emplace_back("add " + arg2_reg + ", " + arg2_reg + ", " + base_reg);
    //mips << "sll " + arg2_reg + ", " + arg2_reg + ", " + "2" << endl;
    //mips << "add " + arg2_reg + ", " + arg2_reg + ", " + base_reg << endl;
    releaseReg(base_reg);
    return arg2_reg;
}

void arrayAssign(vector<QuadraExp>::iterator iter, ofstream &mips) {
    string varReg = getVarReg(iter->arg1, mips);
    string offset_reg = getOffset(iter->result, iter->arg2, mips);
    mipsCode.emplace_back("sw " + varReg + ", " + "0(" + offset_reg + ")");
    //mips << "sw " << varReg + ", " + "0(" + offset_reg + ")" << endl;
    releaseReg(varReg);
    releaseReg(offset_reg);
}

int get_alloc_space(vector<QuadraExp>::iterator iter) {
    int space = 0;
    while (iter->op != FUNC_END) {
        if (iter->op == INT_DEF_INNER || iter->op == CHAR_DEF_INNER || iter->result[0] == '@' ||
            iter->op == INT_PARA || iter->op == CHAR_PARA) {
            space += 4;
        } else if (iter->op == INT_ARRAY_DEF_INNER || iter->op == CHAR_ARRAY_DEF_INNER) {
            space += (4 * stoi(iter->arg1));
        }
        iter++;
    }
    return space + 4; //多给$ra分配一个
}

int alloc_space(vector<QuadraExp>::iterator iter, ofstream &mips) {   //进入函数的时候能先给变量分配空间，常量已经在.data
    int space = get_alloc_space(iter);
    int tmp = 0;
    SymTableptr s;
    do {
        iter++;
    } while (iter->op == CONST_INT_DEF || iter->op == CONST_CHAR_DEF);          // 跳过常量定义
    tmp += 4;
    mipsCode.emplace_back("sw $ra, -4($sp)");
    //mips << "sw $ra, -4($sp)" << endl;
    while (iter->op != FUNC_END) {
        if (iter->op == INT_DEF_INNER || iter->op == CHAR_DEF_INNER || iter->op == INT_PARA || iter->op == CHAR_PARA) {
            s = findInSymTable(iter->result, level);
            tmp += 4;
            s->offset = to_string(space - tmp);
        } else if (iter->op == INT_ARRAY_DEF_INNER || iter->op == CHAR_ARRAY_DEF_INNER) {
            s = findInSymTable(iter->result, level);
            tmp += (4 * stoi(iter->arg1));
            s->offset = to_string(space - tmp);
        } else if (iter->result[0] == '@') {
            tmp += 4;
            addressMap[iter->result] = to_string(space - tmp);
        }
        iter++;
    }
    mipsCode.emplace_back("subi, $sp, $sp, " + to_string(space));
    //mips << "subi, $sp, $sp, " << space << endl;
    return space;
}

void arrayGet(vector<QuadraExp>::iterator iter, ofstream &mips) {   // result肯定是@t
    string reg = getReg();
    string offset_reg = getOffset(iter->arg1, iter->arg2, mips);
    mipsCode.emplace_back("lw " + reg + ", " + "0(" + offset_reg + ")");
    //mipsCode.emplace_back("sw " + reg + ", " + addressMap[iter->result] + "($sp)");
    //mips << "lw " << reg + ", " + "0(" + offset_reg + ")" << endl;
    //mips << "sw " << reg << ", " << addressMap[iter->result] << "($sp)" << endl;
    addToMap(iter->result, reg);    //!!
    releaseReg(offset_reg);
    //releaseReg(reg);
}

void genBranch(vector<QuadraExp>::iterator iter, ofstream &mips) {
    string arg1_reg = getVarReg(iter->arg1, mips);
    string arg2_reg = getVarReg(iter->arg2, mips);
    mipsCode.emplace_back(getOp(iter->op) + " " + arg1_reg + ", " + arg2_reg + ", " + iter->result);
    //mips << getOp(iter->op) + " " + arg1_reg + ", " + arg2_reg + ", " + iter->result << endl;
    releaseReg(arg1_reg);
    releaseReg(arg2_reg);
}

void genText(ofstream &mips) {
    SymTableptr symtableItem;
    mipsCode.emplace_back("jal main");
    mipsCode.emplace_back("li $v0, 10");
    mipsCode.emplace_back("syscall");
    //mips << "jal main" << endl;
    //mips << "li $v0, 10" << endl << "syscall" << endl;
    for (auto iter = quadraExpVec.begin(); iter != quadraExpVec.end(); iter++) {
        if (iter->op == PRINT_STR || iter->op == PRINT_INT || iter->op == PRINT_CHAR) {
            if (!iter->arg1.empty()) {
                mipsCode.emplace_back("la $a0, " + addressMap[iter->arg1]);
                mipsCode.emplace_back("li $v0, 4");
                mipsCode.emplace_back("syscall");
                //mips << "la $a0, " << addressMap[iter->arg1] << endl;
                //mips << "li $v0, 4" << endl << "syscall" << endl;
            }
            if (!iter->arg2.empty()) {
                if (iter->op == PRINT_INT) {
                    printInt(mips, iter->arg2);
                } else if (iter->op == PRINT_CHAR) {
                    printChar(mips, iter->arg2);
                }
            }
            printNewline(mips);
        } else if (iter->op == READ_INT) {
            symtableItem = findInSymTable(iter->arg1, level);
            readInt(symtableItem, mips);
        } else if (iter->op == READ_CHAR) {
            symtableItem = findInSymTable(iter->arg1, level);
            readChar(symtableItem, mips);
        } else if (iter->op == MAIN_FUNC_DEF) {
            level = funcToLevel["main"];
            mipsCode.emplace_back("main:");
            //mips << "main:" << endl;
            local_space = alloc_space(iter, mips);
        } else if (iter->op == ASSIGN_OP) {
            varAssign(mips, iter->arg1, iter->result);
        } else if (iter->op == ARRAY_ASSIGN) {
            arrayAssign(iter, mips);
        } else if (iter->op == PLUS_OP || iter->op == MINUS_OP || iter->op == MULT_OP || iter->op == DIV_OP) {
            calculate(iter, mips);
        } else if (iter->op == LABEL) {
            mipsCode.emplace_back(iter->result + ":");
            //mips << iter->result + ":" << endl;
        } else if (iter->op == JUMP) {
            mipsCode.emplace_back("j " + iter->result);
            //mips << "j " + iter->result << endl;
        } else if (iter->op == ARRAY_GET) {
            arrayGet(iter, mips);
        } else if (iter->op == INT_FUNC_DEF || iter->op == VOID_FUNC_DEF || iter->op == CHAR_FUNC_DEF) {
            level = funcToLevel[iter->result];
            mipsCode.emplace_back(iter->result + ":");
            //mips << iter->result + ":" << endl;
            local_space = alloc_space(iter, mips);
        } else if (iter->op == CALL_FUNC) {     //还没放参数
            int paraCnt = getParaCnt(iter->arg1);
            int tmp = -4 - 4 * paraCnt;
            auto r_iter = para_push.rbegin();
            string reg;
            for (int i = 1; i <= paraCnt; i++) {
                reg = getVarReg(*r_iter, mips);
                mipsCode.emplace_back("sw " + reg + ", " + to_string(tmp) + "($sp)");
                //mips << "sw " << reg << ", " << tmp << "($sp)" << endl;
                tmp += 4;
                r_iter++;
                para_push.pop_back();
                releaseReg(reg);
            }
            mipsCode.emplace_back("jal " + iter->arg1);
            //mips << "jal " + iter->arg1 << endl;
        } else if (iter->op == RETURN) {
            mipsCode.emplace_back("lw $ra, " + to_string(local_space - 4) + "($sp)");
            //mips << "lw $ra, " << local_space - 4 << "($sp)" << endl;
            if (!iter->arg1.empty()) {
                string reg = getVarReg(iter->arg1, mips);
                mipsCode.emplace_back("move, $v0, " + reg);
                //mips << "move, $v0, " << reg << endl;
                releaseReg(reg);
            }
            mipsCode.emplace_back("addi, $sp, $sp, " + to_string(local_space));
            mipsCode.emplace_back("jr $ra");
            //mips << "addi, $sp, $sp, " << local_space << endl;
            //mips << "jr $ra" << endl;
        } else if (iter->op == ble || iter->op == blt || iter->op == bge ||
                   iter->op == bgt || iter->op == bne || iter->op == beq) {
            genBranch(iter, mips);
        } else if (iter->op == PUSH) {
            para_push.emplace_back(iter->arg1);
        } else if (iter->op == GET_RETURN && !iter->result.empty()) {
            mipsCode.emplace_back("sw $v0, " + addressMap[iter->result] + "($sp)");
            add_t_to_mem(iter->result);    //!!
            //mips << "sw $v0" << ", " << addressMap[iter->result] << "($sp)" << endl;
        } else if (iter->op == FUNC_END) {
            mipsCode.emplace_back("lw $ra, " + to_string(local_space - 4) + "($sp)");
            mipsCode.emplace_back("addi, $sp, $sp, " + to_string(local_space));
            mipsCode.emplace_back("jr $ra");
            //mips << "lw $ra, " << local_space - 4 << "($sp)" << endl;
            //mips << "addi, $sp, $sp, " << local_space << endl;
            //mips << "jr $ra" << endl;
        }
    }
}

void genMips_opt() {
    ofstream mips;
    mips.open("mips.txt");
    genData(mips);
    mipsCode.emplace_back(".text");
    //mips << ".text" << endl;
    genGlobalVar(mips);
    genText(mips);
    for (auto &iter:mipsCode) {
        mips << iter << endl;
    }
    mips.close();
}

#endif //GENMIPS_GENMIPS_H
