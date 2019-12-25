#ifndef GENMIPS_QUADRA_H
#define GENMIPS_QUADRA_H

#include <iostream>

using namespace std;

enum Operation {        //中间代码类型
    CONST_INT_DEF,
    CONST_CHAR_DEF,
    INT_DEF_OUTER,
    CHAR_DEF_OUTER,
    INT_ARRAY_DEF_OUTER,
    CHAR_ARRAY_DEF_OUTER,
    INT_FUNC_DEF,
    CHAR_FUNC_DEF,
    INT_PARA,
    CHAR_PARA,
    VOID_FUNC_DEF,
    MAIN_FUNC_DEF,
    ARRAY_GET,
    MULT_OP,
    DIV_OP,
    PLUS_OP,
    MINUS_OP,
    beq, // 相等 ==
    bne, // 不等 !=
    blt, // 小于 <
    ble, // 小于等于 <=
    bgt, // 大于 >
    bge, // 大于等于 >=
    PUSH,
    CALL_FUNC,
    RETURN,
    ASSIGN_OP,
    ARRAY_ASSIGN,
    READ_INT,
    READ_CHAR,
    PRINT_INT,
    PRINT_CHAR,
    PRINT_STR,
    INT_DEF_INNER,
    CHAR_DEF_INNER,
    INT_ARRAY_DEF_INNER,
    CHAR_ARRAY_DEF_INNER,
    LABEL,
    JUMP,
    GET_RETURN,
    FUNC_END,
    DEFAULT_OP
};

class QuadraExp {     //四元式
public:
    string result;
    string arg1;
    Operation op{};
    string arg2;
public:
    QuadraExp() {
        this->arg2 = "";
        this->arg1 = "";
        this->op = DEFAULT_OP;
        this->result = "";
    }
};

class Term {
public:
    string value;
    bool isChar;
    bool isIden;
};

class Expr {
public:
    string value;
    bool isChar;
    bool isIden;
};

class Factor {
public:
    string value;
    bool isChar;
    bool isIden;
};

string getT() {
    static int tCount = 1;
    string t = "@t" + to_string(tCount);
    tCount++;
    return t;
}

string getLabel(){
    static int tCount = 1;
    string t = "GeraintLabel" + to_string(tCount);
    tCount++;
    return t;
}

string getOp(Operation op) {
    switch (op) {
        case 0:
            return "CONST_INT_DEF";
        case 1:
            return "CONST_CHAR_DEF";
        case 2:
            return "INT_DEF_OUTER";
        case 3:
            return "CHAR_DEF_OUTER";
        case 4:
            return "INT_ARRAY_DEF_OUTER";
        case 5:
            return "CHAR_ARRAY_DEF_OUTER";
        case 6:
            return "INT_FUNC_DEF";
        case 7:
            return "CHAR_FUNC_DEF";
        case 8:
            return "INT_PARA";
        case 9:
            return "CHAR_PARA";
        case 10:
            return "VOID_FUNC_DEF";
        case 11:
            return "MAIN_FUNC_DEF";
        case 12:
            return "ARRAY_GET";
        case 13:
            return "*";
        case 14:
            return "/";
        case 15:
            return "+";
        case 16:
            return "-";
        case 17:
            return "beq";
        case 18:
            return "bne";
        case 19:
            return "blt";
        case 20:
            return "ble";
        case 21:
            return "bgt";
        case 22:
            return "bge";
        case 23:
            return "PUSH";
        case 24:
            return "CALL_FUNC";
        case 25:
            return "RETURN";
        case 26:
            return "=";
        case 27:
            return "ARRAY_ASSIGN";
        case 28:
            return "READ_INT";
        case 29:
            return "READ_CHAR";
        case 30:
            return "PRINT_INT";
        case 31:
            return "PRINT_CHAR";
        case 32:
            return "PRINT_STR";
        case 33:
            return "INT_DEF_INNER";
        case 34:
            return "CHAR_DEF_INNER";
        case 35:
            return "INT_ARRAY_DEF_INNER";
        case 36:
            return "CHAR_ARRAY_DEF_INNER";
        case 37:
            return "LABEL";
        case 38:
            return "JUMP";
        case 39:
            return "GET_RETURN";
        case 40:
            return "FUNC_END";
        default:
            return ":=";
    }
}

#endif //GENMIPS_QUADRA_H
