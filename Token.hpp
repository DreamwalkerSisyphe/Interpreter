//
// Created by Ali A. Kooshesh on 1/30/19.
//

#ifndef EXPRINTER_TOKEN_HPP
#define EXPRINTER_TOKEN_HPP
#include<string>

class Token {

public:
    Token();

    bool &eof()  { return _eof; }
    bool &eol()  { return _eol; }

    bool eof() const { return _eof; }
    bool eol() const  { return _eol; }

    bool isOpenParen() const  { return _symbol == '('; }
    bool isCloseParen() const { return _symbol == ')'; }
    bool isOpenBrack() const  { return _symbol == '{'; }
    bool isCloseBrack() const { return _symbol == '}'; }

    void symbol(char c) { _symbol = c; }
    void symbol(std::string s) { _longSymbol = s;}
    char symbol() { return _symbol; }

    bool isSemiColon() const { return _symbol == ';'; }
    bool isAssignmentOperator() const              { return _symbol == '='; }
    bool isMultiplicationOperator() const { return _symbol == '*'; }
    bool isAdditionOperator() const       { return _symbol == '+'; }
    bool isSubtractionOperator() const    { return _symbol == '-'; }
    bool isModuloOperator() const         { return _symbol == '%'; }
    bool isDivisionOperator() const       { return _symbol == '/'; }
    bool isArithmeticOperator() const {
        return isMultiplicationOperator() ||
               isAdditionOperator() ||
               isSubtractionOperator() ||
               isModuloOperator() ||
               isDivisionOperator();
    }
    bool isName() const                   { return ((_name.length() > 0) && (!isKeyword())); }
    std::string getName() const                  { return _name; }
    void setName(std::string n) { _name = n; }

    bool &isWholeNumber() { return _isWholeNumber; }
    bool isWholeNumber() const { return _isWholeNumber; }
    int getWholeNumber() const { return _wholeNumber; }
    void setWholeNumber(int n) {
        _wholeNumber = n;
        isWholeNumber() = true;
    }

    bool isEqualOperator() const { return _longSymbol == "=="; }
    bool isNotEqualOperator() const {return _longSymbol == "!="; }
    bool isGreaterThanOperator() const {return _symbol == '>'; }
    bool isLessThanOperator() const {return _symbol == '<'; }
    bool isGreaterThanOrEqualOperator() const {return _longSymbol == ">="; }
    bool isLessThanOrEqualOperator() const {return _longSymbol == "<="; }
    bool isRelationalOperator() const {
        return isEqualOperator() ||
        isNotEqualOperator() ||
        isGreaterThanOperator() ||
        isLessThanOperator() ||
        isGreaterThanOrEqualOperator() ||
        isLessThanOrEqualOperator();
    }
    
    bool isFor() const { return _name == "for";}
    bool isPrint() const { return _name == "print";}
    bool isKeyword() const {
        return isFor() || isPrint();
    }


    void print() const;

private:
    std::string _name;
    bool _eof, _eol;
    bool _isWholeNumber;
    char _symbol;
    std::string _longSymbol;
    int _wholeNumber;
};

#endif //EXPRINTER_TOKEN_HPP
