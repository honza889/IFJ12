#ifndef SYNTAX_H
#define SYNTAX_H

/**
 * Syntaktická analýza
 */

#include "scanner.h"
#include "ast.h"

/**
 * Kontext pro syntaktický analyzátor.
 */
typedef struct{
    /// Tabulka lokálních symbolů funkce, ve které se zrovna nalézáme.
    /// Počáteční hodnota bude pravděpodobně tabulka symbolů pro hlavní
    /// funkci.
    SymbolTable* localSymbols;
    /// Tabulka globálních symbolů. V současnosti drží pouze funkce,
    /// jelikož ty jsou jediné globálně přístupné symboly.
    SymbolTable* globalSymbols;
    /// Seznam funkcí (uložených ve Value)
    ValueList functions;
} SyntaxContext;

/**
 * Inicializuje výchozí SyntaxContext obsahující vestavěné funkce
 */
void initDefaultSyntaxContext( SyntaxContext* ctx );

/**
 * Uvolní tabulky symbolů v kontextu \a ctx, vytvořeném pomocí 
 * initDefaultSyntaxContext.
 * 
 * Seznam funkcí nijak neuvolňuje.
 */
void destroyDefaultSyntaxContext( SyntaxContext* ctx );

/**
 * Parsuje program pomocí scanneru \a s. Pouziva kontext \a ctx.
 * Hlavni telo programu ulozi do \a main
 */
void parseProgram( Scanner* s, SyntaxContext* ctx, Function* main );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx identifikátor proměnné.
 * Ten poté uloží do \a id
 */
void parseIdentifier( Scanner* s, Variable* id, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx libovolný výraz.
 * Ten poté uloží do \a expr
 */
void parseExpression( Scanner* s, Expression* expr, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx celou smyčku while. 
 * Tj. včetně opakovací podmínky a těla.
 * Tu poté přidá do seznamu statementů \a sl.
 */
void parseWhile( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/** 
 * Parsuje pomocí scanneru \a s v kontextu \a ctx celou konstrukci 
 * podmínky if. Tj. včetně podmínky, těla, které se provede pokud je 
 * podmínka pravdivá a těla, které se provede, pokud je podmínka 
 * nepravdivá.
 * Celou tuto strukturu poté přidá do seznamu statementů \a sl.
 */
void parseIf( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx return včetně 
 * návratového výrazu. Ten pak přidá do seznamu statementů \a sl.
 */
void parseReturn( Scanner* s, StatementList* sl, SyntaxContext* ctx );

void detectAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx přiřazení výrazu do 
 * proměnné. Toto přiřazení pak jako statement přidá so seznamu
 * statementů \a sl.
 */
void parseAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx přiřazení substringu
 * do proměnné. Toto přiřazení pak přidá jako statement do \a sl.
 */
void parseSubstring( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx libovolný statement.
 * Ten pak přidá do \a sl.
 */
void parseStatement( Scanner* s, StatementList* sl, SyntaxContext* ctx );

/**
 * Parsuje pomocí scanneru \a s v kontextu \a ctx definici funkce. 
 * Tu pak přidá do kontextu \a ctx.
 */
void parseFunction( Scanner* s, SyntaxContext* ctx );

/**
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena syntaktickým analyzátorem.
 */
void syntaxErrorPrint(SyntaxErrorException e);

#endif
