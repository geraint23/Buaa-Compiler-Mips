#ifndef GENMIPS_PARSER_H
#define GENMIPS_PARSER_H

#include <set>
#include "error.h"
#include "quadra.h"

vector<pair<Symbol, string>>::iterator now;
set<string> returnFuncSet;
vector<QuadraExp> quadraExpVec;
bool inner = false;

void manySens();

Expr expr();

Factor factor();

Term term();

string useReturnFunc();

void sentences();

Symbol look(int step) {
    auto tmp = now;
    tmp += step;
    return tmp->first;
}

Symbol seeNow() {
    if (vec.empty())
        return UNKNOWN;
    else
        return now->first;
}

void jumpThis() {
    now++;
    lineNow++;
}

bool isReturnFunc() {
    return returnFuncSet.find(now->second) != returnFuncSet.end();
}

void error(int errorLine) {
    cout << "Error line: " << errorLine << endl;
    fprintf(errorFile, "Error line: %d", errorLine);
    jumpThis();
}

string unsignedInteger() {
    if (seeNow() == INTCON) {
//        printFile();
        string tmp = now->second;
        jumpThis();
        return tmp;
//        printInfo("无符号整数");
    } else {
        error_o();
        jumpThis();
        return "error";
    }
}

string integer() {
    string finalInteger;
    Symbol sym = UNKNOWN;
    if (seeNow() == PLUS || seeNow() == MINU) {
//        printFile();
        sym = seeNow();
        jumpThis();
    }
    string aInteger = unsignedInteger();
    if (sym == MINU) {
        finalInteger = "-" + aInteger;
    } else {
        finalInteger = aInteger;
    }
    return finalInteger;
//    printInfo("整数");
}

void intDef() {//<标识符> = <整数>
    QuadraExp q;
    if (seeNow() == IDENFR) {
//        printFile();
        if ((target = getFromSymTable(now->second, level)) == nullptr) {
            add(now->second, type, level, false);
            q.op = CONST_INT_DEF;
            q.result = now->second;
        } else {
            error_b();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == ASSIGN) {
//        printFile();
        jumpThis();
        string aInteger = integer();
        q.arg1 = aInteger;
        q.arg2 = to_string(level);
        quadraExpVec.emplace_back(q);
    } else {
        error(__LINE__);
    }
}

void charDef() {//<标识符> = <字符>
    QuadraExp q;
    if (seeNow() == IDENFR) {
//        printFile();
        if ((target = getFromSymTable(now->second, level)) == nullptr) {
            add(now->second, type, level, false);
            q.op = CONST_CHAR_DEF;
            q.result = now->second;
        } else {
            error_b();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == ASSIGN) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == CHARCON) {
//        printFile();
        if (!charCheck(now->second)) {
            error_a();
        }
        q.arg1 = now->second;
        q.arg2 = to_string(level);
        quadraExpVec.emplace_back(q);
        jumpThis();
    } else {
        error_o();
        jumpThis();
    }
}

void constDef() {
    if (seeNow() == INTTK) {
//        printFile();
        jumpThis();
        type = CONST_INT;
        intDef();
        while (seeNow() == COMMA) {
//            printFile();
            jumpThis();
            intDef();
        }
//        printInfo("常量定义");
    } else if (seeNow() == CHARTK) {
//        printFile();
        jumpThis();
        type = CONST_CHAR;
        charDef();
        while (seeNow() == COMMA) {
//            printFile();
            jumpThis();
            charDef();
        }
//        printInfo("常量定义");
    } else {
        error(__LINE__);
    }
}

void constExp() {
    while (seeNow() == CONSTTK) {
//        printFile();
        jumpThis();
        constDef();
        if (seeNow() == SEMICN) {
//            printFile();
            jumpThis();
        } else {
            error_k();
        }
    }
//    printInfo("常量说明");
}

void subVarDef() {//<标识符>|<标识符>'['<无符号整数>']'
    QuadraExp q;
    if (seeNow() == IDENFR) {
//        printFile();
        if ((target = getFromSymTable(now->second, level)) == nullptr) {
            q.result = now->second;
            if (type == INT && look(1) == LBRACK) {
                add(now->second, INT_ARRAY, level, false);
                if (inner) {
                    q.op = INT_ARRAY_DEF_INNER;
                } else {
                    q.op = INT_ARRAY_DEF_OUTER;
                }
            } else if (type == CHAR && look(1) == LBRACK) {
                add(now->second, CHAR_ARRAY, level, false);
                if (inner) {
                    q.op = CHAR_ARRAY_DEF_INNER;
                } else {
                    q.op = CHAR_ARRAY_DEF_OUTER;
                }
            } else if (type == INT) {
                add(now->second, INT, level, false);
                if (inner) {
                    q.op = INT_DEF_INNER;
                } else {
                    q.op = INT_DEF_OUTER;
                }
            } else if (type == CHAR) {
                add(now->second, CHAR, level, false);
                if (inner) {
                    q.op = CHAR_DEF_INNER;
                } else {
                    q.op = CHAR_DEF_OUTER;
                }
            }
        } else {
            error_b();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == LBRACK) {
//        printFile();
        jumpThis();
        q.arg1 = unsignedInteger();
        if (seeNow() == RBRACK) {
//            printFile();
            jumpThis();
        } else {
            error_m();
        }
    }
    quadraExpVec.emplace_back(q);
}

void varDef() {
    if (seeNow() == INTTK || seeNow() == CHARTK) {
//        printFile();
        if (seeNow() == INTTK) {
            type = INT;
        } else {
            type = CHAR;
        }
        jumpThis();
        subVarDef();
        while (seeNow() == COMMA) {
//            printFile();
            jumpThis();
            subVarDef();
        }
    } else {
        error(__LINE__);
    }
//    printInfo("变量定义");
}

void varExp() {
    while ((seeNow() == INTTK || seeNow() == CHARTK) && (look(2) != LPARENT)) {
        varDef();
        if (seeNow() == SEMICN) {
//            printFile();
            jumpThis();
        } else {
            error_k();
        }
    }
//    printInfo("变量说明");
}

string defHead() {
    QuadraExp q;
    string funcName;
    if (seeNow() == INTTK || seeNow() == CHARTK) {
//        printFile();
        if (seeNow() == INTTK) {
            q.op = INT_FUNC_DEF;
            type = INT_FUNCTION;
        } else {
            q.op = CHAR_FUNC_DEF;
            type = CHAR_FUNCTION;
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == IDENFR) {
//        printFile();
        funcName = now->second;
        if ((target = getFromSymTable(now->second, level)) == nullptr) {
            add(now->second, type, level, false);
            returnFuncSet.insert(now->second);
            q.result = now->second;
            quadraExpVec.emplace_back(q);
        } else {
            error_b();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    return funcName;
//    printInfo("声明头部");
}

void subParaTable() { //<类型标识符><标识符>
    QuadraExp q;
    if (seeNow() == INTTK || seeNow() == CHARTK) {
//        printFile();
        if (seeNow() == INTTK) {
            type = INT;
            q.op = INT_PARA;
        } else {
            type = CHAR;
            q.op = CHAR_PARA;
        }
        jumpThis();
        if (seeNow() == IDENFR) {
//            printFile();
            if ((target = getFromSymTable(now->second, level)) == nullptr) {
                add(now->second, type, level, true);
                q.result = now->second;
                quadraExpVec.emplace_back(q);
            }
            jumpThis();
        } else {
            error(__LINE__);
        }
    }
}

void paraTable() {
    subParaTable();
    while (seeNow() == COMMA) {
//        printFile();
        jumpThis();
        subParaTable();
    }
//    printInfo("参数表");
}

Expr expr() {
    Expr e;
    QuadraExp q;
    Operation op = DEFAULT_OP;
    if (seeNow() == PLUS || seeNow() == MINU) {
//        printFile();
        if (seeNow() == PLUS) {
            op = PLUS_OP;
        } else {
            op = MINUS_OP;
        }
        jumpThis();
    }
    Term t1 = term();
    Term t2;
    e.isChar = t1.isChar;
    e.value = t1.value;
    e.isIden = t1.isIden;
    if (op == PLUS_OP || op == MINUS_OP) {
        q.result = getT();
        q.arg1 = "0";
        q.op = op;
        if (t1.isChar && !t1.isIden) {
            q.arg2 = to_string((int) t1.value[0]);
        } else {
            q.arg2 = t1.value;
        }
        quadraExpVec.emplace_back(q);
        e.value = q.result;
    }
    while (seeNow() == PLUS || seeNow() == MINU) {
        if (seeNow() == PLUS) {
            op = PLUS_OP;
        } else {
            op = MINUS_OP;
        }
//        printFile();
        jumpThis();
        t2 = term();
        q.result = getT();
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg1 = to_string((int) e.value[0]);
        } else {
            q.arg1 = e.value;
        }
        e.isChar = false;
        q.op = op;
        if (t2.isChar && !t2.isIden) {
            q.arg2 = to_string((int) t2.value[0]);
        } else {
            q.arg2 = t2.value;
        }
        quadraExpVec.emplace_back(q);
        e.value = q.result;
    }
    return e;
//    printInfo("表达式");
}

Factor factor() {
    QuadraExp q;
    Factor f{};
    f.isChar = false;
    f.isIden = true;
    if (seeNow() == IDENFR && look(1) != LPARENT && !isReturnFunc()) {
        target = findInSymTable(now->second, level);
        f.value = now->second;
        if (target == nullptr) {
            error_c();
        } else {
            if (target->type == CHAR || target->type == CHAR_ARRAY || target->type == CONST_CHAR) {
                f.isChar = true;
            }
        }
        if (look(1) != LBRACK) {
//            printFile();
            jumpThis();
        } else {
//            printFile();
            jumpThis();
//            printFile();
            jumpThis();
            Expr e = expr();
            q.result = getT();
            if (f.isChar && !f.isIden) {
                q.arg1 = to_string((int) f.value[0]);
            } else {
                q.arg1 = f.value;
            }
            q.op = ARRAY_GET;
            if (e.isChar && !e.isIden && e.value[0] != '@') {
                q.arg2 = to_string((int) e.value[0]);
            } else {
                q.arg2 = e.value;
            }
            f.value = q.result;
            quadraExpVec.emplace_back(q);
            if (e.isChar) {
                error_i();
            }
            if (seeNow() == RBRACK) {
//                printFile();
                jumpThis();
            } else {
                error_m();
            }
        }
    } else if (seeNow() == IDENFR && look(1) == LPARENT && isReturnFunc()) {
        target = findInSymTable(now->second, level);
        if (target == nullptr) {
            error_c();
        } else {
            if (target->type == CHAR_FUNCTION) {
                f.isChar = true;
            }
        }
        f.value = useReturnFunc();
    } else if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
        Expr e = expr();
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            f.value = to_string((int) e.value[0]);
        } else {
            f.value = e.value;
        }
        if (seeNow() == RPARENT) {
//            printFile();
            jumpThis();
        } else {
            error_l();
        }
    } else if (seeNow() == CHARCON) {
        f.isIden = false;
        f.isChar = true;
        f.value = now->second;
        if (!charCheck(now->second)) {
            error_a();
        }
//        printFile();
        jumpThis();
    } else if (seeNow() == INTCON || seeNow() == PLUS || seeNow() == MINU) {
        f.value = integer();
    } else {
        error(__LINE__);//如果调用了无返回值函数会卡死
    }
    return f;
//    printInfo("因子");
}

Term term() {
    Term t{};
    Factor f1 = factor();
    Factor f2;
    t.isChar = f1.isChar;
    t.value = f1.value;
    t.isIden = f1.isIden;
    while (seeNow() == MULT || seeNow() == DIV) {
        QuadraExp q;
//        printFile();
        Operation op;
        if (seeNow() == MULT) {
            op = MULT_OP;
        } else {
            op = DIV_OP;
        }
        jumpThis();
        f2 = factor();
        q.result = getT();
        if (t.isChar && !t.isIden) {
            q.arg1 = to_string((int) t.value[0]);
        } else {
            q.arg1 = t.value;
        }
        t.isChar = false;
        q.op = op;
        if (f2.isChar && !f2.isIden) {
            q.arg2 = to_string((int) f2.value[0]);
        } else {
            q.arg2 = f2.value;
        }
        quadraExpVec.emplace_back(q);
        t.value = q.result;
    }
    return t;
//    printInfo("项");
}

string condition(const string &label) {
    QuadraExp q;
    Expr e1 = expr();
    if (label != "!") {
        q.result = label;
    } else {
        q.result = getLabel();
    }
    q.arg1 = e1.value;
    if (label != "!") {
        q.op = bne;
    } else {
        q.op = beq;
    }
    q.arg2 = "0";
    if (seeNow() == LSS || seeNow() == LEQ || seeNow() == GRE ||
        seeNow() == GEQ || seeNow() == EQL || seeNow() == NEQ) {
        if (seeNow() == LSS) {
            if (label != "!") {
                q.op = blt;
            } else {
                q.op = bge;
            }
        } else if (seeNow() == LEQ) {
            if (label != "!") {
                q.op = ble;
            } else {
                q.op = bgt;
            }
        } else if (seeNow() == GRE) {
            if (label != "!") {
                q.op = bgt;
            } else {
                q.op = ble;
            }
        } else if (seeNow() == GEQ) {
            if (label != "!") {
                q.op = bge;
            } else {
                q.op = blt;
            }
        } else if (seeNow() == EQL) {
            if (label != "!") {
                q.op = beq;
            } else {
                q.op = bne;
            }
        } else if (seeNow() == NEQ) {
            if (label != "!") {
                q.op = bne;
            } else {
                q.op = beq;
            }
        }
        jumpThis();
        Expr e2 = expr();
        q.arg2 = e2.value;
        if (e1.isChar || e2.isChar) {
            error_f();
        }
    } else if (e1.isChar) {
        error_f();
    }
    quadraExpVec.emplace_back(q);
//    printInfo("条件");
    return q.result;
}

string step() {
    return unsignedInteger();
//    printInfo("步长");
}

void varParaTable(SymTableptr func) {  //p是对应的函数.p->paraTable才是参数表开始
    auto para = func->paraTable;
    int realCount = 0;
    QuadraExp q;
    if (seeNow() != RPARENT) {  //肯定有表達式
        realCount++;
        Expr e = expr();
        q.op = PUSH;
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg1 = to_string((int) e.value[0]);
        } else {
            q.arg1 = e.value;
        }
        quadraExpVec.emplace_back(q);
        if (para != nullptr) {
            if (!typeCheck(para, e.isChar)) {
                error_e();
            }
            para = para->paraTable;
        }
        while (seeNow() == COMMA) {
            realCount++;
//            printFile();
            jumpThis();
            e = expr();
            if (e.isChar && !e.isIden && e.value[0] != '@') {
                q.arg1 = to_string((int) e.value[0]);
            } else {
                q.arg1 = e.value;
            }
            quadraExpVec.emplace_back(q);
            if (para != nullptr) {
                if (!typeCheck(para, e.isChar)) {
                    error_e();
                }
                para = para->paraTable;
            }
        }
    }
    if (!paraCountCheck(func, realCount)) {
        error_d();
    }
//    printInfo("值参数表");
}

void returnSen() {
    QuadraExp q;
    if (seeNow() == RETURNTK) {
//        printFile();
        jumpThis();
        if (level_entrance[level]->type == VOID_FUNCTION && seeNow() != SEMICN) {
            error_g();
        } else {
            q.op = RETURN;
        }
    }
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
        Expr e = expr();
        if ((level_entrance[level]->type == INT_FUNCTION && e.isChar) ||
            (level_entrance[level]->type == CHAR_FUNCTION && !e.isChar)) {
            error_h();
        }
        if (seeNow() == RPARENT) {
//            printFile();
            jumpThis();
        } else {
            error_l();
        }
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg1 = to_string((int) e.value[0]);
        } else {
            q.arg1 = e.value;
        }
    }
    quadraExpVec.emplace_back(q);
//    printInfo("返回语句");
}

void printSen() {
//    printFile();
    QuadraExp q;
    jumpThis();
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == STRCON) {
        q.op = PRINT_STR;
        q.arg1 = now->second;
        if (!stringCheck(now->second)) {
            error_a();
        }
//        printFile();
//        printInfo("字符串");
        jumpThis();
        if (seeNow() == COMMA) {
//            printFile();
            jumpThis();
            Expr e = expr();
            if (e.isChar && !e.isIden && e.value[0] != '@') {
                q.arg2 = to_string((int) e.value[0]);
            } else {
                q.arg2 = e.value;
            }
            if (e.isChar) {
                q.op = PRINT_CHAR;
            } else {
                q.op = PRINT_INT;
            }
        }
    } else {
        Expr e = expr();
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg2 = to_string((int) e.value[0]);
        } else {
            q.arg2 = e.value;
        }
        if (e.isChar) {
            q.op = PRINT_CHAR;
        } else {
            q.op = PRINT_INT;
        }
    }
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    quadraExpVec.emplace_back(q);
//    printInfo("写语句");
}

void scanSen() {
//    printFile();
    QuadraExp q;
    jumpThis();
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == IDENFR) {
        target = findInSymTable(now->second, level);
        if (target == nullptr) {
            error_c();
        } else {
            q.arg1 = now->second;
            if (target->type == INT) {
                q.op = READ_INT;
            } else if (target->type == CHAR) {
                q.op = READ_CHAR;
            }
        }
//        printFile();
        jumpThis();
        quadraExpVec.emplace_back(q);
    } else {
        error(__LINE__);
    }
    while (seeNow() == COMMA) {
//        printFile();
        jumpThis();
        if (seeNow() == IDENFR) {
            target = findInSymTable(now->second, level);
            if (target == nullptr) {
                error_c();
            } else {
                q.arg1 = now->second;
                if (target->type == INT) {
                    q.op = READ_INT;
                } else if (target->type == CHAR) {
                    q.op = READ_CHAR;
                }
            }
//            printFile();
            jumpThis();
        }
        quadraExpVec.emplace_back(q);
    }
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
//    printInfo("读语句");
}

void assignSen() {
//    printFile();
    QuadraExp q;
    q.result = now->second;
    target = findInSymTable(now->second, level);
    if (target == nullptr) {
        error_c();
    } else if (target->type == CONST_INT || target->type == CONST_CHAR) {
        error_j();
    }
    jumpThis();
    Expr e;
    if (seeNow() == ASSIGN) {
//        printFile();
        jumpThis();
        e = expr();
        q.op = ASSIGN_OP;
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg1 = to_string((int) e.value[0]);
        } else {
            q.arg1 = e.value;
        }
        quadraExpVec.emplace_back(q);
    } else if (seeNow() == LBRACK) {
//        printFile();
        jumpThis();
        e = expr();
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg2 = to_string((int) e.value[0]);
        } else {
            q.arg2 = e.value;
        }
        if (e.isChar) {
            error_i();
        }
        if (seeNow() == RBRACK) {
//            printFile();
            jumpThis();
        } else {
            error_m();
        }
        if (seeNow() == ASSIGN) {
//            printFile();
            q.op = ARRAY_ASSIGN;
            jumpThis();
        } else {
            error(__LINE__);
        }
        e = expr();
        if (e.isChar && !e.isIden && e.value[0] != '@') {
            q.arg1 = to_string((int) e.value[0]);
        } else {
            q.arg1 = e.value;
        }
        quadraExpVec.emplace_back(q);
    } else {
        error(__LINE__);
    }
//    printInfo("赋值语句");
}

string useFunc() { //<标识符>'('<值参数表>')'
    QuadraExp q;
    Type t = DEFAULT_TYPE;
    if (seeNow() == IDENFR) {
        target = findInSymTable(now->second, level);
        t = target->type;
        if (target == nullptr) {
            error_c();
        } else {
            q.op = CALL_FUNC;
            q.arg1 = now->second;
        }
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    varParaTable(target);   //值参数表的参数都从这个target中来，否则会在因子中因寻找标识符而更新
    if (seeNow() == RPARENT) {
//        printFile();
        quadraExpVec.emplace_back(q);
        q.arg1 = "";
        q.op = GET_RETURN;
        if (t != VOID_FUNCTION) {
            q.result = getT();
        }
        quadraExpVec.emplace_back(q);
        jumpThis();
    } else {
        error_l();
    }
    return q.result;
}

void useNoneReturnFunc() {
    useFunc();
//    printInfo("无返回值函数调用语句");
}

string useReturnFunc() {
    return useFunc();
//    printInfo("有返回值函数调用语句");
}

void forSen() {
//    printFile();
    QuadraExp q;
    jumpThis();
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == IDENFR) {
//        printFile();
        q.result = now->second;
        target = findInSymTable(now->second, level);
        if (target == nullptr) {
            error_c();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == ASSIGN) {
        q.op = ASSIGN_OP;
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    Expr e = expr();
    if (e.isChar && !e.isIden && e.value[0] != '@') {
        q.arg1 = to_string((int) e.value[0]);
    } else {
        q.arg1 = e.value;
    }
    quadraExpVec.emplace_back(q);
    q = QuadraExp();
    q.op = LABEL;
    q.result = getLabel();
    string loop_label = q.result;
    quadraExpVec.emplace_back(q);
    if (seeNow() == SEMICN) {
//        printFile();
        jumpThis();
    } else {
        error_k();
    }
    string out_label = condition("!");
    if (seeNow() == SEMICN) {
//        printFile();
        jumpThis();
    } else {
        error_k();
    }
    string result;
    if (seeNow() == IDENFR) {
        result = now->second;
        target = findInSymTable(now->second, level);
        if (target == nullptr) {
            error_c();
        }
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    Operation assign = DEFAULT_OP;
    if (seeNow() == ASSIGN) {
        assign = ASSIGN_OP;
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == IDENFR) {
        q.arg1 = now->second;
        target = findInSymTable(now->second, level);
        if (target == nullptr) {
            error_c();
        }
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == PLUS || seeNow() == MINU) {
        if (seeNow() == PLUS) {
            q.op = PLUS_OP;
        } else {
            q.op = MINUS_OP;
        }
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    q.arg2 = step();
    q.result = getT();
    string arg1 = q.result;
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    sentences();
    quadraExpVec.emplace_back(q);
    q = QuadraExp();
    q.result = result;
    q.arg1 = arg1;
    q.op = assign;
    quadraExpVec.emplace_back(q);
    q = QuadraExp();
    q.op = JUMP;
    q.result = loop_label;
    quadraExpVec.emplace_back(q);
    q.op = LABEL;
    q.result = out_label;
    quadraExpVec.emplace_back(q);
}

void doWhileSen() {
//    printFile();
    jumpThis();
    QuadraExp q;
    q.op = LABEL;
    q.result = getLabel();
    string label = q.result;
    quadraExpVec.emplace_back(q);
    sentences();
    if (seeNow() == WHILETK) {
//        printFile();
        jumpThis();
    } else {
        error_n();
    }
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    condition(q.result);
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
}

void whileSen() {
//    printFile();
    QuadraExp q;
    q.op = LABEL;
    q.result = getLabel();
    quadraExpVec.emplace_back(q);
    string loop_label = q.result;
    jumpThis();
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    string out_label = condition("!");
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    sentences();
    q.op = JUMP;
    q.result = loop_label;
    quadraExpVec.emplace_back(q);
    q.op = LABEL;
    q.result = out_label;
    quadraExpVec.emplace_back(q);
}

void loopSen() {
    if (seeNow() == WHILETK) {
        whileSen();
    } else if (seeNow() == DOTK) {
        doWhileSen();
    } else if (seeNow() == FORTK) {
        forSen();
    }
//    printInfo("循环语句");
}

void ifElseSen() {
//    printFile();
    QuadraExp q;
    jumpThis();
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    string label1 = condition("!");
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    sentences();
    if (seeNow() == ELSETK) {
        q.op = JUMP;
        q.result = getLabel();
        string label2 = q.result;
        quadraExpVec.emplace_back(q);
        q.op = LABEL;
        q.result = label1;
        quadraExpVec.emplace_back(q);
//        printFile();
        jumpThis();
        sentences();
        q.result = label2;
        quadraExpVec.emplace_back(q);
    } else {
        q.op = LABEL;
        q.result = label1;
        quadraExpVec.emplace_back(q);
    }
//    printInfo("条件语句");
}

void solveSemi() { //统一处理一下最后的分号
    if (seeNow() == SEMICN) {
//        printFile();
        jumpThis();
    } else {
        error_k();
    }
}

void sentences() {
    if (seeNow() == IFTK) {
        ifElseSen();
    } else if (seeNow() == WHILETK || seeNow() == DOTK || seeNow() == FORTK) {
        loopSen();
    } else if (seeNow() == LBRACE) {
//        printFile();
        jumpThis();
        manySens();
        if (seeNow() == RBRACE) {
//            printFile();
            jumpThis();
        } else {
            error(__LINE__);
        }
    } else if (seeNow() == IDENFR && look(1) == LPARENT && isReturnFunc()) {
        useReturnFunc();
        solveSemi();
    } else if (seeNow() == IDENFR && look(1) == LPARENT && !isReturnFunc()) {
        useNoneReturnFunc();
        solveSemi();
    } else if (seeNow() == IDENFR) {//不是函数，标识符开头，必须是赋值语句
        assignSen();
        solveSemi();
    } else if (seeNow() == SCANFTK) {
        scanSen();
        solveSemi();
    } else if (seeNow() == PRINTFTK) {
        printSen();
        solveSemi();
    } else if (seeNow() == RETURNTK) {
        hasReturnSen = true;
        returnSen();
        solveSemi();
    } else if (seeNow() == SEMICN) {//<空>
//        printFile();
        jumpThis();
    }
//    printInfo("语句");
}

void manySens() {
    while (seeNow() != RBRACE) {
        sentences();
    }
//    printInfo("语句列");
}

void complexSen() {
    if (seeNow() == CONSTTK) {
        constExp();
    }
    if (seeNow() == INTTK || seeNow() == CHARTK) {
        varExp();
    }
    manySens();
//    printInfo("复合语句");
}

void funcSolve(const string &funcName) { //'('<参数表>')' '{'<复合语句>'}'
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    levelUp();
    funcToLevel[funcName] = level;
    setEntrance();
    paraTable();//可能有INTTK CHARTK也可能直接是')'
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    if (seeNow() == LBRACE) {
//        printFile();
        jumpThis();

    } else {
        error(__LINE__);
    }
    complexSen();
    if (seeNow() == RBRACE) {
        if (level_entrance[level]->type != VOID_FUNCTION && !hasReturnSen) {
            error_h();
        }
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
}

void hasReturnFunc() { //procedure判断能进来肯定已经是函数定义
    string funcName = defHead();
    funcSolve(funcName);
//    printInfo("有返回值函数定义");
}

void noneReturnFunc() {
    QuadraExp q;
    string funcName;
    if (seeNow() == VOIDTK) {
//        printFile();
        type = VOID_FUNCTION;
        jumpThis();
        q.op = VOID_FUNC_DEF;
    } else {
        error(__LINE__);
    }
    if (seeNow() == IDENFR) {
//        printFile();
        funcName = now->second;
        if ((target = getFromSymTable(now->second, level)) == nullptr) {
            add(now->second, type, level, false);
            q.result = now->second;
            quadraExpVec.emplace_back(q);
        } else {
            error_b();
        }
        jumpThis();
    } else {
        error(__LINE__);
    }
    funcSolve(funcName);
//    printInfo("无返回值函数定义");
}

void mainFunc() {
    QuadraExp q;
    if (seeNow() == VOIDTK) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == MAINTK) {
        add(now->second, VOID_FUNCTION, level, false);
        setEntrance();
//        printFile();
        jumpThis();
        q.op = MAIN_FUNC_DEF;
        quadraExpVec.emplace_back(q);
    } else {
        error(__LINE__);
    }
    if (seeNow() == LPARENT) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    if (seeNow() == RPARENT) {
//        printFile();
        jumpThis();
    } else {
        error_l();
    }
    if (seeNow() == LBRACE) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
    complexSen();
    if (seeNow() == RBRACE) {
//        printFile();
        jumpThis();
    } else {
        error(__LINE__);
    }
//    printInfo("主函数");
}

void procedure() {
    createSymTable();
    if (seeNow() == CONSTTK) {
        constExp();
    }
    if ((seeNow() == INTTK || seeNow() == CHARTK) && (look(2) != LPARENT)) {
        varExp();
    }
    inner = true;
    while ((seeNow() == INTTK || seeNow() == CHARTK || seeNow() == VOIDTK)
           && look(1) != MAINTK && look(2) == LPARENT) {
        QuadraExp q;
        if (seeNow() == INTTK || seeNow() == CHARTK) {
            hasReturnFunc();
        } else {
            noneReturnFunc();
        }
        if (!hasReturnSen) {
            q.op = RETURN;
            quadraExpVec.emplace_back(q);
        }
        resetHasReturnSen();
        levelDown();
        q.op = FUNC_END;
        quadraExpVec.emplace_back(q);
    }
    if (seeNow() == VOIDTK && look(1) == MAINTK && look(2) == LPARENT) {
        QuadraExp q;
        levelUp();
        funcToLevel["main"] = level;
        mainFunc();
        if (!hasReturnSen) {
            q.op = RETURN;
            quadraExpVec.emplace_back(q);
        }
        q.op = FUNC_END;
        quadraExpVec.emplace_back(q);
    } else {
        error(__LINE__);
    }
//    printInfo("程序");
}

void parser() {
    procedure();
    fclose(errorFile);
}

#endif  //GENMIPS_PARSER_H