
#include<vector>
#include<iostream>

#include "Token.hpp"
#include "Parser.hpp"
#include "Statements.hpp"

// Parser functions

void Parser::die(std::string where, std::string message, Token &token) {
    std::cout << where << " " << message << std::endl;
    token.print();
    std::cout << std::endl;
    std::cout << "\nThe following is a list of tokens that have been identified up to this point.\n";
    tokenizer.printProcessedTokens();
    exit(1);
}

Statements *Parser::statements() {
    // This function is called when we KNOW that we are about to parse
    // a series of assignment statements.

    // This function parses the grammar rules:

    // <statement> -> <statement> { <statement> }
    // <statement> -> <for-statement> | <assign-statement> | <print-statement>
    // <statement> -> Epsilon

    bool inScope;
    Token tok = tokenizer.getToken();
    if(tok.isOpenBrack()){
        inScope = true;
        tok = tokenizer.getToken();
        if(!tok.eol())
            tokenizer.ungetToken();
    }
    else {
        inScope = false;
        tokenizer.ungetToken();
    }
    
    Statements *stmts = new Statements();
    tok = tokenizer.getToken();
    while (tok.isName() || tok.isKeyword()) {
        if(tok.isFor()){
            ForStatement *forStmt = forStatement();
            stmts->addStatement(forStmt);
        }
        else if(tok.isPrint()){
            PrintStatement *printStmt = printStatement();
            stmts->addStatement(printStmt);
        }
        else {
            tokenizer.ungetToken();
            AssignmentStatement *assignStmt = assignStatement();
            stmts->addStatement(assignStmt);
        }
        tok = tokenizer.getToken();
        if(tok.isCloseBrack() && inScope)
            break;
        while(tok.eol())
            tok = tokenizer.getToken();
    }
    if(!inScope)
        tokenizer.ungetToken();
    return stmts;
}

AssignmentStatement *Parser::assignStatement() {
    // Parses the following grammar rule
    // 
    // <assign-stmtement> -> <id> = <arith_expr>

    Token varName = tokenizer.getToken();
    if (!varName.isName())
        die("Parser::assignStatement", "Expected a name token, instead got", varName);

    Token assignOp = tokenizer.getToken();
    if (!assignOp.isAssignmentOperator())
        die("Parser::assignStatement", "Expected an equal sign, instead got", assignOp);

    ExprNode *rightHandSideExpr = rel_expr();
    /*Token tok = tokenizer.getToken();
    if (!tok.isSemiColon())
        die("Parser::assignStatement", "Expected a semicolon, instead got", tok);*/

    return new AssignmentStatement(varName.getName(), rightHandSideExpr);
}

ForStatement *Parser::forStatement(){
    // Parses the following grammar rule
    //
    // <for-statement> -> for ( <assign-statement> ; <rel-expr> ; <assign-statement> ) { <statements> }

    Token tok = tokenizer.getToken();
    if(!tok.isOpenParen())
        die("Parser::forStatement", "Expected an open parenthesis, instead got", tok);

    AssignmentStatement *varInitialize = assignStatement();

    tok = tokenizer.getToken();
    if(!tok.isSemiColon())
        die("Parser::forStatement", "Expected a semicolonm, instead got", tok);
    
    ExprNode *relExpr = rel_expr();

    tok = tokenizer.getToken();
    if(!tok.isSemiColon())
        die("Parser::forStatement", "Expected a semicolonm, instead got", tok);

    AssignmentStatement *varUpdate = assignStatement();

    tok = tokenizer.getToken();
    if(!tok.isCloseParen())
        die("Parser::forStatement", "Expected a close parenthesis, instead got", tok);
    tok = tokenizer.getToken();
    if(!tok.isOpenBrack())
        die("Parser::forStatement", "Expected a open bracket, instead got", tok);
    tokenizer.ungetToken();
    Statements *stmts = statements();

    return new ForStatement(varInitialize, relExpr, varUpdate, stmts);
}

PrintStatement *Parser::printStatement(){
    // Parses the following grammar rule
    //
    // <print-statement> -> print <id>
    Token tok = tokenizer.getToken();
    if(!tok.isName())
        die("Parser::printStatement", "Unexpected token", tok);
    
    std::string varName = tok.getName();

    return new PrintStatement(varName);
}

ExprNode *Parser::rel_expr(){
    // This function parses the grammar rule:

    // <rel-expr> -> <rel-term> {(==, !=) <rel-term>}

    ExprNode *left = rel_term();
    Token tok = tokenizer.getToken();
    while (tok.isEqualOperator() || tok.isNotEqualOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = rel_term();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::rel_term(){
    // This function parses the grammar rules:

    // <rel-term> -> <rel-primary> {(>, >=, <, <=) <rel-primary>}
    // <rel-primary> -> <arith-expr>

    ExprNode *left = arith_expr();
    Token tok = tokenizer.getToken();

    while (tok.isGreaterThanOperator() || tok.isGreaterThanOrEqualOperator() || tok.isLessThanOperator() || tok.isLessThanOrEqualOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = arith_expr();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arith_expr() {
    // This function parses the grammar rules:

    // <arith_expr> -> <term> { <add_op> <arith_term> }
    // <add_op> -> + | -

    // However, it makes the <add_op> left associative.

    ExprNode *left = arith_term();
    Token tok = tokenizer.getToken();
    while (tok.isAdditionOperator() || tok.isSubtractionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = arith_term();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}


ExprNode *Parser::arith_term() {
    // This function parses the grammar rules:

    // <arith_term> -> <primary> { <mult_op> <arith_primary> }
    // <mult_op> -> * | / | %

    // However, the implementation makes the <mult-op> left associate.
    ExprNode *left = arith_primary();
    Token tok = tokenizer.getToken();

    while (tok.isMultiplicationOperator() || tok.isDivisionOperator() || tok.isModuloOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = arith_primary();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arith_primary() {
    // This function parses the grammar rules:

    // <arith_primary> -> [0-9]+
    // <arith_primary> -> [_a-zA-Z]+
    // <arith_primary> -> (<arith_expr>)

    Token tok = tokenizer.getToken();

    if (tok.isWholeNumber() )
        return new WholeNumber(tok);
    else if( tok.isName() )
        return new Variable(tok);
    else if (tok.isOpenParen()) {
        ExprNode *p = arith_expr();
        Token token = tokenizer.getToken();
        if (!token.isCloseParen())
            die("Parser::arith_primary", "Expected close-parenthesis, instead got", token);
        return p;
    }
    die("Parser::arith_primary", "Unexpected token", tok);

    return nullptr;  // Will not reach this statement!
}
