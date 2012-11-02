#include <assert.h>
#include <stdbool.h>
#include "global.h"
#include "syntax.h"
#include "exceptions.h"
#include "scanner.h"
#include "ast.h"
#include "alloc.h"

Token syntax(FILE *f){
        // TODO: Kontrola syntaxe - pokud selze, vyjimka
        //throw(SyntaxError,15);
        return scan(f);
}

void addFunctionToAst( Ast* ast, Function* function );
void addStatementToStatementList( StatementList* sl, Statement* statement );

void parseIdentifier( Scanner* s, RCString* id );
void parseExpression( Scanner* s, Expression* expr );
void parseWhile( Scanner* s, StatementList* sl );
void parseIf( Scanner* s, StatementList* sl );
void parseReturn( Scanner* s, StatementList* sl );
void detectAssignment( Scanner* s, StatementList* sl );
void parseAssignment( Scanner* s, StatementList* sl );
void parseSubstring( Scanner* s, StatementList* sl );
void parseStatement( Scanner* s, StatementList* sl );
void parseFunction( Scanner* s, Function* func );
void parseFunctionParameters( Scanner* s, Function* func );
Ast parseProgram( Scanner* s );

void addFunctionToAst( Ast* ast, Function* function )
{
    ast->functions.count++;
    if( ast->functions.count >= ast->functions.capacity )
    {
        ast->functions.capacity += 8;
        ast->functions.functions = resizeArray( ast->functions.functions, Function, ast->functions.capacity );
    }
}

void addStatementToStatementList( StatementList* sl, Statement* statement )
{
    sl->count++;
    sl->item = resizeArray( sl->item, Statement, sl->count );
}


Ast parseProgram( Scanner* s )
{
    Ast ast = {
        .functions = {.functions = NULL, .capacity = 0, .count = 0},
        .main = { 
            .type = USER_DEFINED, 
            .value.userDefined = { 
                .statements = {
                    .item = NULL,
                    .count = 0
                }
            }
        }
    };
    
    while( getTok( s ).type != tokEndOfFile )
    {
        Token tok = getTok(s);
        if( tok.type == tokKeyW && tok.data.keyw == kFunction )
        {
            Function func = {
                .type = USER_DEFINED,
                .value.userDefined = {
                    .statements = {
                        .item = NULL,
                        .count = 0
                    }
                }
            };
            parseFunction( s, &func );
            addFunctionToAst( &ast, &func );
        }
        else
        {
            parseStatement( s, &ast.main.value.userDefined.statements );
        }
    }
    
    return ast;
}

void parseFunctionParameters( Scanner* s, Function* func )
{
    testTok( s, tokId | tokRParen );
    switch( getTok( s ).type )
    {
        case tokId: 
        {
            RCString param;
            parseIdentifier( s, &param );
            setNewSymbol( param, &func->symTable );
            func->paramCount++;
            
            testTok( s, tokComma | tokRParen );
            
            if( getTok( s ).type == tokComma )
            {
                consumeTok( s );
                parseFunctionParameters( s, func );
            }
            else
            {
                return;
            }
            break;
        }
        case tokRParen:
            return;
        default: break; // aby si kompilátor nestěžoval
    }
}

void parseFunction( Scanner* s, Function* func )
{
    func->paramCount = 0;
    expectKeyw( s, kFunction );
    parseIdentifier( s, &func->name );
    expectTok( s, tokLParen );
    parseFunctionParameters( s, func );
    expectTok( s, tokRParen );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd )
    {
        parseStatement( s, &func->value.userDefined.statements );
    }
    consumeTok( s );
    expectTok( s, tokEndOfLine );
}

void parseStatement( Scanner* s, StatementList* sl )
{
    testTok( s, tokId | tokKeyW | tokEndOfLine );
    if( getTok( s ).type == tokId )
    {
        detectAssignment( s, sl );
    }
    else if( getTok( s ).type == tokKeyW ) 
    {
        testKeyw( s, kIf | kWhile | kReturn );
        if( getTok( s ).data.keyw == kIf )
        {
            parseIf( s, sl );
        }
        else if( getTok( s ).data.keyw == kWhile )
        {
            parseWhile( s, sl );
        }
        else
        {
            parseReturn( s, sl );
        }
    }
}       

void parseReturn( Scanner* s, StatementList* sl )
{
    // BUG: Na co je StatementList, když se očividně nepoužívá?
    Statement stmt;
    stmt.type = RETURN;
    
    expectKeyw( s, kReturn );
    parseExpression( s, &stmt.value.ret );
    expectTok( s, tokEndOfLine );    
}

void detectAssignment( Scanner* s, StatementList* sl )
{
    assert(getTokN(s, 0).type == tokId);
    expectNextTok(s, tokAssign);
    testTokN(s, tokId, 1);
    if (getTokN(s, 2).type == tokLBracket)
      parseSubstring(s, sl);
    else
      parseAssignment(s, sl);
}

void parseAssignment( Scanner* s, StatementList* sl )
{
    Assignment ass;
//     ass.destination = ; // TODO: Jak se implementuje? Jak index do pole proměnných (podle symbols.h)?
//     ass.source;
//     parseExpression(s, ); // TODO: Jak tam mám naládovat výraz?
}

void parseSubstring( Scanner* s, StatementList* sl )
{
    Substring substr;	// TODO
//     substr.destination = ;
//     substr.source;
//     substr.offset;
//     substr.length;
}

void parseIf( Scanner* s, StatementList* sl )
{
    Statement stmt;
    stmt.type = CONDITION;
    
    expectKeyw( s, kIf );
    parseExpression( s, &stmt.value.condition.condition );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kElse )
    {
        parseStatement( s, &stmt.value.condition.ifTrue );
    }
    expectKeyw( s, kElse );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kElse )
    {
        parseStatement( s, &stmt.value.condition.ifFalse );
    }
    expectKeyw( s, kElse );
    expectTok( s, tokEndOfLine );
    addStatementToStatementList( sl, &stmt );
}

void parseWhile( Scanner* s, StatementList* sl )
{
    Statement stmt;
    stmt.type = LOOP;
    
    expectKeyw( s, kWhile );
    parseExpression( s, &stmt.value.loop.condition );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd )
    {
        parseStatement( s, &stmt.value.loop.statements );
    }
    expectKeyw( s, kEnd );
    expectTok( s, tokEndOfLine );
}

void parseExpression( Scanner* s, Expression* expr )
{
    // TODO
    // je treba pouzit precedencni pravidla pro operatory atd...
}

void parseIdentifier( Scanner* s, RCString* id )
{
    testTok( s, tokId );
    *id = getTok( s ).data.id;
    consumeTok( s );
}
