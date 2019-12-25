#ifndef GENMIPS_TMPCODE_H
#define GENMIPS_TMPCODE_H

#include <fstream>
#include "parser.h"

void original_tmpCode() {
    ofstream tmpCode;
    tmpCode.open("original_tmpCode.txt");
    tmpCode.setf(std::ios::left);
    for (auto &iter : quadraExpVec) {
        tmpCode.width(20);
        tmpCode << iter.result;
        tmpCode.width(20);
        tmpCode << iter.arg1;
        tmpCode.width(20);
        tmpCode << getOp(iter.op);
        tmpCode.width(20);
        tmpCode << iter.arg2 << endl;
    }
    tmpCode.close();
}

void std_tmpCode() {
    ofstream tmpCode;
    tmpCode.open("16005044_郗哈颉_优化前中间代码.txt");
    for (auto &iter : quadraExpVec) {
        switch (iter.op) {
            case 0:
                tmpCode << "const int " << iter.result << " = " << iter.arg1 << endl;
                break;
            case 1:
                tmpCode << "const char " << iter.result << " = " << "'" + iter.arg1 + "'" << endl;
                break;
            case 2:
                tmpCode << "var int " << iter.result << endl;
                break;
            case 3:
                tmpCode << "var char " << iter.result << endl;
                break;
            case 4:
                tmpCode << "array int " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 5:
                tmpCode << "array char " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 6:
                tmpCode << "int " << iter.result + "()" << endl;
                break;
            case 7:
                tmpCode << "char " << iter.result + "()" << endl;
                break;
            case 8:
                tmpCode << "para int " << iter.result << endl;
                break;
            case 9:
                tmpCode << "para char " << iter.result << endl;
                break;
            case 10:
                tmpCode << "void " << iter.result + "()" << endl;
                break;
            case 11:
                tmpCode << "void main()" << endl;
                break;
            case 12:
                tmpCode << iter.result + " = " << iter.arg1 + "[" + iter.arg2 + "]" << endl;
                break;
            case 13:
                tmpCode << iter.result + " = " << iter.arg1 + " * " + iter.arg2 << endl;
                break;
            case 14:
                tmpCode << iter.result + " = " << iter.arg1 + " / " + iter.arg2 << endl;
                break;
            case 15:
                tmpCode << iter.result + " = " << iter.arg1 + " + " + iter.arg2 << endl;
                break;
            case 16:
                tmpCode << iter.result + " = " << iter.arg1 + " - " + iter.arg2 << endl;
                break;
            case 17:
                tmpCode << iter.arg1 + " == " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 18:
                tmpCode << iter.arg1 + " != " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 19:
                tmpCode << iter.arg1 + " < " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 20:
                tmpCode << iter.arg1 + " <= " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 21:
                tmpCode << iter.arg1 + " > " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 22:
                tmpCode << iter.arg1 + " >= " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 23:
                tmpCode << "push " << iter.arg1 << endl;
                break;
            case 24:
                tmpCode << "call " << iter.arg1 << endl;
                break;
            case 25:
                if (!iter.arg1.empty()) {
                    tmpCode << "return " << iter.arg1 << endl;
                } else {
                    tmpCode << "return" << endl;
                }
                break;
            case 26:
                tmpCode << iter.result + " = " << iter.arg1 << endl;
                break;
            case 27:
                tmpCode << iter.result + "[" + iter.arg2 + "] = " << iter.arg1 << endl;
                break;
            case 28:
                tmpCode << "read integer " << iter.arg1 << endl;
                break;
            case 29:
                tmpCode << "read character " << iter.arg1 << endl;
                break;
            case 30:
                if (!iter.arg1.empty()) {
                    tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                }
                if (!iter.arg2.empty()) {
                    tmpCode << "print integer " << iter.arg2 << endl;
                }
                break;
            case 31:
                if (!iter.arg1.empty()) {
                    tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                }
                if (!iter.arg2.empty()) {
                    tmpCode << "print character " << iter.arg2 << endl;
                }
                break;
            case 32:
                tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                break;
            case 33:
                tmpCode << "var int " << iter.result << endl;
                break;
            case 34:
                tmpCode << "var char " << iter.result << endl;
                break;
            case 35:
                tmpCode << "array int " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 36:
                tmpCode << "array char " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 37:
                tmpCode << "label " << iter.result << endl;
                break;
            case 38:
                tmpCode << "jump " << iter.result << endl;
                break;
            case 39:
                tmpCode << iter.result + " = RET" << endl;
                break;
            case 40:
                tmpCode << "function end" << endl;
                break;
            default:
                continue;
        }
    }
    tmpCode.close();
}

void std_tmpCode2() {
    ofstream tmpCode;
    tmpCode.open("16005044_郗哈颉_优化后中间代码.txt");
    for (auto &iter : quadraExpVec) {
        switch (iter.op) {
            case 0:
                tmpCode << "const int " << iter.result << " = " << iter.arg1 << endl;
                break;
            case 1:
                tmpCode << "const char " << iter.result << " = " << "'" + iter.arg1 + "'" << endl;
                break;
            case 2:
                tmpCode << "var int " << iter.result << endl;
                break;
            case 3:
                tmpCode << "var char " << iter.result << endl;
                break;
            case 4:
                tmpCode << "array int " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 5:
                tmpCode << "array char " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 6:
                tmpCode << "int " << iter.result + "()" << endl;
                break;
            case 7:
                tmpCode << "char " << iter.result + "()" << endl;
                break;
            case 8:
                tmpCode << "para int " << iter.result << endl;
                break;
            case 9:
                tmpCode << "para char " << iter.result << endl;
                break;
            case 10:
                tmpCode << "void " << iter.result + "()" << endl;
                break;
            case 11:
                tmpCode << "void main()" << endl;
                break;
            case 12:
                tmpCode << iter.result + " = " << iter.arg1 + "[" + iter.arg2 + "]" << endl;
                break;
            case 13:
                tmpCode << iter.result + " = " << iter.arg1 + " * " + iter.arg2 << endl;
                break;
            case 14:
                tmpCode << iter.result + " = " << iter.arg1 + " / " + iter.arg2 << endl;
                break;
            case 15:
                tmpCode << iter.result + " = " << iter.arg1 + " + " + iter.arg2 << endl;
                break;
            case 16:
                tmpCode << iter.result + " = " << iter.arg1 + " - " + iter.arg2 << endl;
                break;
            case 17:
                tmpCode << iter.arg1 + " == " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 18:
                tmpCode << iter.arg1 + " != " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 19:
                tmpCode << iter.arg1 + " < " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 20:
                tmpCode << iter.arg1 + " <= " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 21:
                tmpCode << iter.arg1 + " > " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 22:
                tmpCode << iter.arg1 + " >= " + iter.arg2 + " branch " + iter.result << endl;
                break;
            case 23:
                tmpCode << "push " << iter.arg1 << endl;
                break;
            case 24:
                tmpCode << "call " << iter.arg1 << endl;
                break;
            case 25:
                if (!iter.arg1.empty()) {
                    tmpCode << "return " << iter.arg1 << endl;
                } else {
                    tmpCode << "return" << endl;
                }
                break;
            case 26:
                tmpCode << iter.result + " = " << iter.arg1 << endl;
                break;
            case 27:
                tmpCode << iter.result + "[" + iter.arg2 + "] = " << iter.arg1 << endl;
                break;
            case 28:
                tmpCode << "read integer " << iter.arg1 << endl;
                break;
            case 29:
                tmpCode << "read character " << iter.arg1 << endl;
                break;
            case 30:
                if (!iter.arg1.empty()) {
                    tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                }
                if (!iter.arg2.empty()) {
                    tmpCode << "print integer " << iter.arg2 << endl;
                }
                break;
            case 31:
                if (!iter.arg1.empty()) {
                    tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                }
                if (!iter.arg2.empty()) {
                    tmpCode << "print character " << iter.arg2 << endl;
                }
                break;
            case 32:
                tmpCode << "print string " << "\"" + iter.arg1 + "\"" << endl;
                break;
            case 33:
                tmpCode << "var int " << iter.result << endl;
                break;
            case 34:
                tmpCode << "var char " << iter.result << endl;
                break;
            case 35:
                tmpCode << "array int " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 36:
                tmpCode << "array char " << iter.result << "[" + iter.arg1 + "]" << endl;
                break;
            case 37:
                tmpCode << "label " << iter.result << endl;
                break;
            case 38:
                tmpCode << "jump " << iter.result << endl;
                break;
            case 39:
                tmpCode << iter.result + " = RET" << endl;
                break;
            case 40:
                tmpCode << "function end" << endl;
                break;
            default:
                continue;
        }
    }
    tmpCode.close();
}

#endif //GENMIPS_TMPCODE_H
