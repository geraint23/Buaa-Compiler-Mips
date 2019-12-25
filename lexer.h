#ifndef PARSER_LEXER_H
#define PARSER_LEXER_H

#include <iostream>
#include <vector>
#include <sstream>
#include <cctype>

using namespace std;

enum Symbol {
    IDENFR,
    INTCON,
    CHARCON,
    STRCON,
    CONSTTK,
    INTTK,
    CHARTK,
    VOIDTK,
    MAINTK,
    IFTK,
    ELSETK,
    DOTK,
    WHILETK,
    FORTK,
    SCANFTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    MULT,
    DIV,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,
    UNKNOWN
};

//string symbolToStr(Symbol symbol) {
//    string str;
//    switch (symbol) {
//        case IDENFR:
//            str = "IDENFR";
//            break;
//        case INTCON:
//            str = "INTCON";
//            break;
//        case CHARCON:
//            str = "CHARCON";
//            break;
//        case STRCON:
//            str = "STRCON";
//            break;
//        case CONSTTK:
//            str = "CONSTTK";
//            break;
//        case INTTK:
//            str = "INTTK";
//            break;
//        case CHARTK:
//            str = "CHARTK";
//            break;
//        case VOIDTK:
//            str = "VOIDTK";
//            break;
//        case MAINTK:
//            str = "MAINTK";
//            break;
//        case IFTK:
//            str = "IFTK";
//            break;
//        case ELSETK:
//            str = "ELSETK";
//            break;
//        case DOTK:
//            str = "DOTK";
//            break;
//        case WHILETK:
//            str = "WHILETK";
//            break;
//        case FORTK:
//            str = "FORTK";
//            break;
//        case SCANFTK:
//            str = "SCANFTK";
//            break;
//        case PRINTFTK:
//            str = "PRINTFTK";
//            break;
//        case RETURNTK:
//            str = "RETURNTK";
//            break;
//        case PLUS:
//            str = "PLUS";
//            break;
//        case MINU:
//            str = "MINU";
//            break;
//        case MULT:
//            str = "MULT";
//            break;
//        case DIV:
//            str = "DIV";
//            break;
//        case LSS:
//            str = "LSS";
//            break;
//        case LEQ:
//            str = "LEQ";
//            break;
//        case GRE:
//            str = "GRE";
//            break;
//        case GEQ:
//            str = "GEQ";
//            break;
//        case EQL:
//            str = "EQL";
//            break;
//        case NEQ:
//            str = "NEQ";
//            break;
//        case ASSIGN:
//            str = "ASSIGN";
//            break;
//        case SEMICN:
//            str = "SEMICN";
//            break;
//        case COMMA:
//            str = "COMMA";
//            break;
//        case LPARENT:
//            str = "LPARENT";
//            break;
//        case RPARENT:
//            str = "RPARENT";
//            break;
//        case LBRACK:
//            str = "LBRACK";
//            break;
//        case RBRACK:
//            str = "RBRACK";
//            break;
//        case LBRACE:
//            str = "LBRACE";
//            break;
//        case RBRACE:
//            str = "RBRACE";
//            break;
//        default:
//            str = "Unidentified";
//    }
//    return str;
//}

char ch;
string token;
FILE *in,*errorFile;
Symbol symbol;
vector<pair<Symbol, string>> vec;
vector<int> line;
vector<int>::iterator lineNow;

void init() {
    in = fopen("testfile.txt", "r");
    errorFile = fopen("error.txt", "w");
}

void getSymbol() {
    if (token == "const")
        symbol = CONSTTK;
    else if (token == "int")
        symbol = INTTK;
    else if (token == "char")
        symbol = CHARTK;
    else if (token == "void")
        symbol = VOIDTK;
    else if (token == "main")
        symbol = MAINTK;
    else if (token == "if")
        symbol = IFTK;
    else if (token == "else")
        symbol = ELSETK;
    else if (token == "do")
        symbol = DOTK;
    else if (token == "while")
        symbol = WHILETK;
    else if (token == "scanf")
        symbol = SCANFTK;
    else if (token == "printf")
        symbol = PRINTFTK;
    else if (token == "for")
        symbol = FORTK;
    else if (token == "return")
        symbol = RETURNTK;
    else
        symbol = IDENFR;
}

void clearToken() {
    token = "";
    symbol = UNKNOWN;
    ch = ' ';
}

void catToken() {
    token += ch;
}

int isPlus() {
    return ch == '+';
}

int isMinus() {
    return ch == '-';
}

int isMulti() {
    return ch == '*';
}

int isSquo() {
    return ch == '\'';
}

int isDquo() {
    return ch == '\"';
}

int isDivi() {
    return ch == '/';
}

int isLess() {
    return ch == '<';
}

int isGreater() {
    return ch == '>';
}

int isExc() {
    return ch == '!';
}

int isLetter() {
    return isalpha(ch) || ch == '_';
}

int isDigit() {
    return isdigit(ch);
}

int isComma() {
    return ch == ',';
}

int isSemi() {
    return ch == ';';
}

int isEqu() {
    return ch == '=';
}

int isLpar() {
    return ch == '(';
}

int isRpar() {
    return ch == ')';
}

int isLbrack() {
    return ch == '[';
}

int isRbrack() {
    return ch == ']';
}

int isLbrace() {
    return ch == '{';
}

int isRbrace() {
    return ch == '}';
}

void getChar() {
    ch = char(fgetc(in));
}

void retract() {
    fseek(in, -1, SEEK_CUR);
}

void lexer() {
    int lineCounter = 1;
    init();
    while (true) {
        clearToken();
        getChar();
        while (isspace(ch)) {
            if (ch == '\n') {
                lineCounter++;
            }
            getChar();
        }
        if (isLetter()) {
            while (isDigit() || isLetter()) {
                catToken();
                getChar();
            }
            if (ch != EOF)
                retract();
            getSymbol();
        } else if (isDigit()) {
            while (isDigit()) {
                catToken();
                getChar();
            }
            if (ch != EOF)
                retract();
            symbol = INTCON;
        } else if (isEqu()) {
            symbol = ASSIGN;
            catToken();
            getChar();
            if (isEqu()) {
                symbol = EQL;
                catToken();
            } else {
                if (ch != EOF)
                    retract();
            }
        } else if (isPlus()) {
            symbol = PLUS;
        } else if (isMinus()) {
            symbol = MINU;
        } else if (isMulti()) {
            symbol = MULT;
        } else if (isDivi()) {
            symbol = DIV;
        } else if (isLpar()) {
            symbol = LPARENT;
        } else if (isRpar()) {
            symbol = RPARENT;
        } else if (isComma()) {
            symbol = COMMA;
        } else if (isSemi()) {
            symbol = SEMICN;
        } else if (isLess()) {
            symbol = LSS;
            catToken();
            getChar();
            if (isEqu()) {
                symbol = LEQ;
                catToken();
            } else {
                if (ch != EOF)
                    retract();
            }
        } else if (isGreater()) {
            symbol = GRE;
            catToken();
            getChar();
            if (isEqu()) {
                symbol = GEQ;
                catToken();
            } else {
                if (ch != EOF)
                    retract();
            }
        } else if (isExc()) {
            catToken();
            getChar();
            if (isEqu()) {
                symbol = NEQ;
                catToken();
            } else {
                clearToken();
                if (ch != EOF)
                    retract();
            }
        } else if (isLbrace()) {
            symbol = LBRACE;
        } else if (isRbrace()) {
            symbol = RBRACE;
        } else if (isLbrack()) {
            symbol = LBRACK;
        } else if (isRbrack()) {
            symbol = RBRACK;
        } else if (isSquo()) {
            symbol = CHARCON;
            getChar();
            while (!isSquo()) {
                catToken();
                getChar();
            }
            getChar();
            if (ch != EOF)
                retract();
        } else if (isDquo()) {
            symbol = STRCON;
            getChar();
            while (!isDquo()) {
                if (ch == '\\') {
                    catToken();
                }
                catToken();
                getChar();
            }
            getChar();
            if (ch != EOF)
                retract();
        } else if (ch == EOF) {
            fclose(in);
            lineNow = line.begin();
            return;
        }
        if (token.length()) {
            vec.emplace_back(symbol, token);
            line.emplace_back(lineCounter);
        } else if (!isspace(ch)) {
            stringstream stream;
            stream << ch;
            vec.emplace_back(symbol, stream.str());
            line.emplace_back(lineCounter);
        }
    }
}

#endif //PARSER_LEXER_H
