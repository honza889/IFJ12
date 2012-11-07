#ifndef SYNTAX_H
#define SYNTAX_H

/**
 * Syntaktická analýza
 */

#include "scanner.h"
#include "ast.h"

typedef struct{
    SymbolTable* localSymbols;
    SymbolTable* globalSymbols;
    ValueList functions;
} SyntaxContext;


/**
 * Parsuje program pomocí scanneru \a s. Pouziva kontext \a ctx.
 * Hlavni telo programu ulozi do \a main
 */
void parseProgram( Scanner* s, SyntaxContext* ctx, Function* main );

// TODO: doplnit popisy funkci

void parseIdentifier( Scanner* s, Variable* id, SyntaxContext* ctx );

void parseExpression( Scanner* s, Expression* expr, SyntaxContext* ctx );

void parseWhile( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseIf( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseReturn( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void detectAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseSubstring( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseStatement( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void parseFunction( Scanner* s, SyntaxContext* ctx );

void parseFunctionParameters( Scanner* s, Function* func, SyntaxContext* ctx );

/**
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena syntaktickým analyzátorem.
 */
void syntaxErrorPrint(SyntaxErrorException e);

#endif
