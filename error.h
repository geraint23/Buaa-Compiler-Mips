#ifndef ERROR_ERROR_H
#define ERROR_ERROR_H

#include <iostream>
#include "lexer.h"
#include "symTable.h"

using namespace std;
bool isError = false;

void error_a() {
    isError = true;
    fprintf(errorFile, "%d a\n", *lineNow);
}

void error_b() {
    isError = true;
    fprintf(errorFile, "%d b\n", *lineNow);
}

void error_c() {
    isError = true;
    fprintf(errorFile, "%d c\n", *lineNow);
}

void error_d() {
    isError = true;
    fprintf(errorFile, "%d d\n", *lineNow);
}

void error_e() {
    isError = true;
    fprintf(errorFile, "%d e\n", *lineNow);
}

void error_f() {
    isError = true;
    fprintf(errorFile, "%d f\n", *lineNow);
}

void error_g() {
    isError = true;
    fprintf(errorFile, "%d g\n", *lineNow);
}

void error_h() {
    isError = true;
    fprintf(errorFile, "%d h\n", *lineNow);
}

void error_i() {
    isError = true;
    fprintf(errorFile, "%d i\n", *lineNow);
}

void error_j() {
    isError = true;
    fprintf(errorFile, "%d j\n", *lineNow);
}

void error_k() {
    isError = true;
    lineNow--;
    fprintf(errorFile, "%d k\n", *lineNow);
    lineNow++;
}

void error_l() {
    isError = true;
    lineNow--;
    fprintf(errorFile, "%d l\n", *lineNow);
    lineNow++;
}

void error_m() {
    isError = true;
    lineNow--;
    fprintf(errorFile, "%d m\n", *lineNow);
    lineNow++;
}

void error_n() {
    isError = true;
    lineNow--;
    fprintf(errorFile, "%d n\n", *lineNow);
    lineNow++;
}

void error_o() {
    isError = true;
    fprintf(errorFile, "%d o\n", *lineNow);
}

bool paraCountCheck(SymTableptr p, int realCount) {
    int count = 0;
    p = p->paraTable;
//    cout << "--------ParaTable--------" << endl;
    while (p != nullptr) {
        count++;
//        print(p);
        p = p->paraTable;
    }
//    cout << realCount << endl;
//    cout << count << endl;
//    cout << "-----------" << endl;
    return count == realCount;
}

bool typeCheck(SymTableptr para, bool t) {
    return (t && para->type == CHAR) || (!t && para->type == INT);
}

bool charCheck(string check) {
    if (check.length() != 1) {
        cout << "wrong char length" << endl;
        return false;
    } else {
        char c = check[0];
        return (c == '+' || c == '-' || c == '*' || c == '/' ||
                isdigit(c) || c == '_' || isalpha(c));
    }
}

bool stringCheck(const string &check) { //双引号里面又有双引号会卡死
    for (char i : check) {
        if (!(i >= 32 && i <= 126 && i != 34)) {
            return false;
        }
    }
    return true;
}

#endif //ERROR_ERROR_H