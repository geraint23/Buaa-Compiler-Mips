#ifndef ERROR_SYMTABLE_H
#define ERROR_SYMTABLE_H

#define MAXLEVEL 30

#include <iostream>
#include <map>

using namespace std;

typedef enum Type {
    CONST_INT,
    CONST_CHAR,
    INT,
    CHAR,
    INT_ARRAY,
    CHAR_ARRAY,
    INT_FUNCTION,
    CHAR_FUNCTION,
    VOID_FUNCTION,
    DEFAULT_TYPE
} Type;

struct symTable {
    string name;
    Type type{};
    int level{};
    bool isPara{};
    struct symTable *next{};
    struct symTable *paraTable{};
    string offset;
    string value;
};

typedef struct symTable SymTable;
typedef struct symTable *SymTableptr;

void print(SymTableptr);

SymTableptr head, last, target, level_entrance[MAXLEVEL] = {nullptr};
int level = 1;  //当前的level
Type type;      //当前的标识符类型
bool hasReturnSen = false;
map<string, int> funcToLevel;  //根据函数定位函数里参数，变量的level

void createSymTable() {
    head = new SymTable;
    head->next = nullptr;
    head->paraTable = nullptr;
    last = head;
}

void levelUp() {
    static int change = 1;
    level += change;
    change++;
}

void levelDown() {
    level = 1;
}

void setEntrance() {
    level_entrance[level] = last;
}

void resetHasReturnSen() {
    hasReturnSen = false;
}

string getType(Type t) {
    string _;
    switch (t) {
        case 0:
            _ = "const int";
            break;
        case 1:
            _ = "const char";
            break;
        case 2:
            _ = "int";
            break;
        case 3:
            _ = "char";
            break;
        case 4:
            _ = "int array";
            break;
        case 5:
            _ = "char array";
            break;
        case 6:
            _ = "int function";
            break;
        case 7:
            _ = "char function";
            break;
        case 8:
            _ = "void function";
            break;
        default:
            _ = "default";
            break;
    }
    return _;
}

void add(string name, Type t, int lev, bool isPara) {//连一个新块，重置last，last后为空
    auto tmp = new SymTable;
    tmp->name = std::move(name);
    tmp->type = t;
    tmp->level = lev;
    tmp->isPara = isPara;
    tmp->next = nullptr;
    tmp->paraTable = nullptr;
    last->next = tmp;
    if (isPara) {
        last->paraTable = tmp;
    }
    last = tmp;
//    cout << "--------Added to SymbolTable--------" << endl;
//    print(tmp);
}

SymTableptr getFromSymTable(const string &name, int lev) { //根据level找对应的符号表变量，level必须相同，只在局部找
    SymTableptr tmp;
    for (tmp = head->next; tmp != nullptr; tmp = tmp->next) {
        if (name == tmp->name && lev == tmp->level) {
            return tmp;
        }
    }
    return nullptr;
}

SymTableptr findInSymTable(const string &name, int lev) { //判断是否在符号表内定义，在全局和局部都找，优先局部有的变量
    SymTableptr tmp, final = nullptr;
    for (tmp = head->next; tmp != nullptr; tmp = tmp->next) {
        if ((name == tmp->name && tmp->level == 1) || (name == tmp->name && tmp->level == lev)) {
            final = tmp;
        }
    }
    return final;
}

int getParaCnt(const string &funcName) {
    SymTableptr s = findInSymTable(funcName, 1);
    SymTableptr p = s->paraTable;
    int cnt = 0;
    while (p != nullptr) {
        cnt++;
        p = p->paraTable;
    }
    return cnt;
}

void print(SymTableptr tmp) {
    cout << "name:  " << tmp->name << endl;
    cout << "type:  " << getType(tmp->type) << endl;
    cout << "level:  " << tmp->level << endl;
    cout << "isPara:  " << tmp->isPara << endl;
    cout << endl;
}

#endif //ERROR_SYMTABLE_H
