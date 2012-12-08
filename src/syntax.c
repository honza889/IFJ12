/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <assert.h>
#include <stdbool.h>
#include "global.h"
#include "syntax.h"
#include "exceptions.h"
#include "scanner.h"
#include "ast.h"
#include "alloc.h"
#include "bif.h"
#include "ial.h"
#include "parsexp.h"

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function );
void addStatementToStatementList( StatementList* sl, Statement* statement );
void addExpressionToExpressionList( ExpressionList* sl, Expression* statement );

void fillNameListFromSymbolTree( RCString* nl, Symbol* st )
{
    // rekurzivne to staci, nikdo nebude mit miliony promennych v jedne funkci
    // a pokud bude, tak ma spatnej kod a je to jeho chyba :P
    if( !st ) return;

    nl[ st->index ] = copyRCString( &st->name );		// DE-BUG: 2. kopie
    fillNameListFromSymbolTree( nl, st->lesser );
    fillNameListFromSymbolTree( nl, st->greater );
}

int getFunctionId( SyntaxContext* ctx, RCString* name )
{
    int oldCount = ctx->globalSymbols->count;
    int id = getSymbol( *name, ctx->globalSymbols, NULL );
    bool newFunction = oldCount < ctx->globalSymbols->count;
    if( newFunction )
    {
        ctx->functions = resizeArray( ctx->functions, Value, ctx->globalSymbols->count );
        ctx->functions[ -id-1 ] = newValueUndefined();
    }
    return id;
}

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function )
{
    int id = getFunctionId( ctx, name );

    if( ctx->functions[ -id-1 ].type == typeUndefined )
    {

        ctx->functions[ -id-1 ] = newValueFunction( function );
    }
    else
    {
        throw( MultipleFunctionDefinitions, *name );
    }
}

void addBuiltinToContext( SyntaxContext* ctx, RCString name, BuiltinFunction function, int parameterCount )
{
    Function* funcObj = new( Function );
    funcObj->type = BUILTIN;
    funcObj->value.builtin = function;
    funcObj->paramCount = parameterCount;
    addFunctionToContext( ctx, &name, funcObj );
    deleteRCString(&name);
}

void addStatementToStatementList( StatementList* sl, Statement* statement )
{
    if(sl->item==NULL){
        sl->count=1;
        sl->item = newArray( Statement, sl->count );
        sl->item[0] = *statement;
    }else{
        sl->count++;
        sl->item = resizeArray( sl->item, Statement, sl->count );
        sl->item[sl->count-1] = *statement;
    }
}

void addExpressionToExpressionList( ExpressionList* el, Expression* expression )
{
    if(el->expressions==NULL){
        el->count=1;
        el->expressions = newArray( Statement, el->count );
        el->expressions[0] = *expression;
    }else{
        el->count++;
        el->expressions = resizeArray( el->expressions, Expression, el->count );
        el->expressions[el->count-1] = *expression;
    }
}

void initDefaultSyntaxContext( SyntaxContext* ctx )
{
    ctx->localSymbols = new( SymbolTable );
    *ctx->localSymbols = newSymbolTable();
    ctx->globalSymbols = new( SymbolTable );
    *ctx->globalSymbols = newSymbolTable();
    ctx->functions = NULL;

    addBuiltinToContext( ctx, makeRCString( "input" ), BIFinput, 0 );
    addBuiltinToContext( ctx, makeRCString( "numeric" ), BIFnumeric, 1 );
    addBuiltinToContext( ctx, makeRCString( "print" ), BIFprint, -1 );
    addBuiltinToContext( ctx, makeRCString( "typeOf" ), BIFtypeOf, 1 );
    addBuiltinToContext( ctx, makeRCString( "len" ), BIFlen, 1 );
    addBuiltinToContext( ctx, makeRCString( "find" ), find, 2 );
    addBuiltinToContext( ctx, makeRCString( "sort" ), sort, 1 );
}

void destroyDefaultSyntaxContext( SyntaxContext* ctx )
{
    freeSymbolTable( ctx->globalSymbols );
    freeSymbolTable( ctx->localSymbols );
    free( ctx->globalSymbols );
    free( ctx->localSymbols );
    ctx->globalSymbols = NULL;
    ctx->localSymbols = NULL;
}


void parseProgram( Scanner* s, SyntaxContext* ctx, Function* main )
{

    *main = (Function){
        .type = USER_DEFINED,
        .value.userDefined = {
            .statements = {
                .item = NULL,
                .count = 0 // doplni parseStatement
            },
            .variableCount = 0 // doplni se az po parsovani tady
        },
        .paramCount = 0 // main nikdy nema parametry
    };

    while( getTok( s ).type != tokEndOfFile )
    {
        Token tok = getTok(s);
        if( tok.type == tokKeyW && tok.data.keyw == kFunction )
        {
            parseFunction( s, ctx );
        }
        else
        {
            parseStatement( s, &main->value.userDefined.statements, ctx );
        }
    }
    main->value.userDefined.variableCount = ctx->localSymbols->count;
    main->value.userDefined.variableNames = newArray( RCString, ctx->localSymbols->count );
    fillNameListFromSymbolTree( main->value.userDefined.variableNames, ctx->localSymbols->root );
}

void parseFunctionParameters( Scanner* s, Function* func, SyntaxContext* ctx )
{
    testTok( s, tokId | tokRParen );
    switch( getTok( s ).type )
    {
        case tokId:
        {
            Variable param;
            parseIdentifier( s, &param, ctx ); // tady to chce asi delat neco vic
            func->paramCount++;

            testTok( s, tokComma | tokRParen );

            if( getTok( s ).type == tokComma )
            {
                consumeTok( s );
                parseFunctionParameters( s, func, ctx );
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

// Konzumuje token.
void parseFunction( Scanner* s, SyntaxContext* ctx )
{
    Function* func = new( Function );

    *func = (Function){
        .type = USER_DEFINED,
        .value.userDefined = {
            .statements = {
                .item = NULL,
                .count = 0 // dolplni parseStatement
            },
            .variableCount = 0 // doplni se az po parsovani tady
        },
        .paramCount = 0 // dolplni parseFunctionParameters
    };

    SymbolTable localSymbols = newSymbolTable();

    SymbolTable* oldLocalSymbols = ctx->localSymbols;
    ctx->localSymbols = &localSymbols;

    expectKeyw( s, kFunction );

    testTok( s, tokId );
    RCString funcName = getTok( s ).data.id;
    funcName = copyRCString(&funcName);
    consumeTok( s );

    expectTok( s, tokLParen );
    parseFunctionParameters( s, func, ctx );
    expectTok( s, tokRParen );
    expectTok( s, tokEndOfLine );
    while( ! ( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd ) )
    {
        parseStatement( s, &func->value.userDefined.statements, ctx );
    }
    consumeTok( s );
    expectTok( s, tokEndOfLine );

    func->value.userDefined.variableCount = ctx->localSymbols->count;

    //pro kontrolu shodnosti promenych a funkci
    func->value.userDefined.variableNames = newArray( RCString, ctx->localSymbols->count );
    fillNameListFromSymbolTree( func->value.userDefined.variableNames, ctx->localSymbols->root );

    freeSymbolTable( &localSymbols );

    ctx->localSymbols = oldLocalSymbols;

    addFunctionToContext( ctx, &funcName, func );
    deleteRCString( &funcName );
}

// Konzumuje token.
void parseStatement( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    testTok( s, tokId | tokKeyW | tokEndOfLine );
    if( getTok( s ).type == tokId )
    {
        detectAssignment( s, sl, ctx );
    }
    else if( getTok( s ).type == tokKeyW )
    {
        testKeyw( s, kIf | kWhile | kReturn );
        if( getTok( s ).data.keyw == kIf )
        {
            parseIf( s, sl, ctx );
        }
        else if( getTok( s ).data.keyw == kWhile )
        {
            parseWhile( s, sl, ctx );
        }
        else
        {
            parseReturn( s, sl, ctx );
        }
    }
    else{
        consumeTok(s);
    }
}

// Konzumuje token.
void parseReturn( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt;
    stmt.type = RETURN;

    expectKeyw( s, kReturn );
    parseExpression( s, &stmt.value.ret, ctx );
    expectTok( s, tokEndOfLine );

    addStatementToStatementList( sl, &stmt );
}

void detectAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    assert(getTokN(s, 0).type == tokId);
    expectNextTok(s, tokAssign);
    if (getTokN(s, 2).type == tokLBracket){
      testTokN(s, tokId | tokLiteral, 1);
      parseSubstring(s, sl, ctx);
    }else{
      parseAssignment(s, sl, ctx);
    }
}

// Konzumuje token.
void parseAssignment( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Assignment assgn;
    RCString name = getTok(s).data.id;		// DE-BUG: původ
    assgn.destination = getSymbol(name,NULL, ctx->localSymbols );
    consumeTok(s);
    parseExpression(s, &assgn.source, ctx);
    expectTok(s, tokEndOfLine);

    Statement statement = { .type=ASSIGNMENT, .value.assignment=assgn };
    addStatementToStatementList(sl,&statement);
}

// Konzumuje token.
void parseSubstring( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Substring substr;
    Value valueBuffer;
    RCString name = getTok(s).data.id;
    substr.destination = getSymbol(name,NULL, ctx->localSymbols );
    consumeTok(s);

    if (getTok(s).type == tokLiteral){
        valueBuffer = getTok(s).data.val;
        substr.source = (Expression){ .type=CONSTANT, .value.constant=copyValue(&valueBuffer) };
    }else{
        substr.source = (Expression){ .type=VARIABLE, .value.variable=getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
    }
    consumeTok(s);

    assert(getTok(s).type == tokLBracket);	// V kontextu s detectAssignment().
    consumeTok(s);

    testTok( s, tokId | tokLiteral | tokColon );

    if (getTok(s).type & (tokId | tokLiteral)) {
        if (getTok(s).type == tokLiteral){
            valueBuffer = getTok(s).data.val;
            substr.offset = (Expression){ .type=CONSTANT, .value.constant=copyValue(&valueBuffer) };
        }else{
            substr.offset = (Expression){ .type=VARIABLE, .value.variable=getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
        }
        consumeTok(s);
    }
    else if (getTok(s).type == tokColon) {
        substr.offset = (Expression){ .type=CONSTANT, .value.constant=newValueNumeric(0.0) };
    }

    expectTok(s, tokColon);

    testTok( s, tokId | tokLiteral | tokRBracket );

    if (getTok(s).type & (tokId | tokLiteral)) {
        if (getTok(s).type == tokLiteral){
            valueBuffer = getTok(s).data.val;
            substr.length = (Expression){ .type=CONSTANT, .value.constant=copyValue(&valueBuffer) };
        }else{
            substr.length = (Expression){ .type=VARIABLE, .value.variable = getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
        }
        consumeTok(s);
    }
    else if (getTok(s).type == tokRBracket) {
        substr.length = (Expression){ .type=CONSTANT, .value.constant=newValueUndefined() };
    }

    expectTok(s, tokRBracket);
    expectTok(s, tokEndOfLine);

    Statement statement = { .type=SUBSTRING, .value.substring=substr };
    addStatementToStatementList(sl,&statement);
}

// Konzumuje token.
void parseIf( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt = {.type = CONDITION};

    expectKeyw( s, kIf );
    parseExpression( s, &stmt.value.condition.condition, ctx );
    expectTok( s, tokEndOfLine );
    while( ! (getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kElse) )
    {
        parseStatement( s, &stmt.value.condition.ifTrue, ctx );
    }
    expectKeyw( s, kElse );
    expectTok( s, tokEndOfLine );
    while( ! (getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd) )
    {
        parseStatement( s, &stmt.value.condition.ifFalse, ctx);
    }
    expectKeyw( s, kEnd );
    expectTok( s, tokEndOfLine );
    addStatementToStatementList( sl, &stmt );
}

// Konzumuje token.
void parseWhile( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt = {.type = LOOP};

    expectKeyw( s, kWhile );
    parseExpression( s, &stmt.value.loop.condition, ctx );
    expectTok( s, tokEndOfLine );
    while( ! (getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd) )
    {
        parseStatement( s, &stmt.value.loop.statements, ctx );
    }
    expectKeyw( s, kEnd );
    expectTok( s, tokEndOfLine );
    addStatementToStatementList(sl,&stmt);
}

void parseIdentifier( Scanner* s, Variable* id, SyntaxContext* ctx )
{
    testTok( s, tokId );
    RCString name = getTok( s ).data.id;
    name = copyRCString(&name);
    *id = getSymbol( name, ctx->globalSymbols, ctx->localSymbols );
    if( *id < 0 )
    {
        throw( VariableOverridesFunction, name );
    }
    deleteRCString( &name );
    consumeTok( s );
}

void syntaxErrorPrint(SyntaxErrorException e) {
  switch (e.type) {	// TODO: Dát tam nějaké hlášky!
    case BadTokenInExpression:		fprintf(stderr,"Parse error: Bad token in expression"); break;
    case BadTokenAtBeginOfStatement:	fprintf(stderr,"Parse error: Bad begin of statement"); break;
    case AssignWithoutAssignOperator:	fprintf(stderr,"Parse error: Assign without assign operator"); break;
    case StrangeSyntax:			fprintf(stderr,"Parse error: Your syntax is strange"); break;
    case BinaryOperatorAtBegin:		fprintf(stderr,"Parse error: Binary operator at begin of expression"); break;
    case OperatorAtTheEnd:		fprintf(stderr,"Parse error: Operator at the end of the expression"); break;
    case TwoOperatorsNextToEachOther:	fprintf(stderr,"Parse error: Two operators next to each other"); break;
    case UnterminatedParentheses:	fprintf(stderr,"Parse error: Unclosed parentheses"); break;
    case BadStartOfStatement:		fprintf(stderr,"Parse error: Bad start of statement"); break;
    case BlankExpression:		fprintf(stderr,"Parse error: Blank expression"); break;
  }
  fprintf(stderr," on line %d\n",e.line_num);
}
