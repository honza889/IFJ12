#include <assert.h>
#include <stdbool.h>
#include "global.h"
#include "syntax.h"
#include "exceptions.h"
#include "scanner.h"
#include "ast.h"
#include "alloc.h"
#include "symbols.h"

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function );
void addStatementToStatementList( StatementList* sl, Statement* statement );
void addExpressionToExpressionList( ExpressionList* sl, Expression* statement );

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function )
{
    // asi to chce jeste trochu predelat symboly...
    int id = getSymbol( *name, ctx->globalSymbols, ctx->globalSymbols );
    if( id < ctx->globalSymbols->count )
    {
        throw( MultipleFunctionDefinitions, copyRCString( name ) );
    }
    else
    {
        ctx->functions = resizeArray( ctx->functions, Value, ctx->globalSymbols->count );
        ctx->functions[ ctx->globalSymbols->count - 1 ] = newValueFunction( function );
    }
}

void addStatementToStatementList( StatementList* sl, Statement* statement )
{
    sl->count++;
    sl->item = resizeArray( sl->item, Statement, sl->count );
    sl->item[sl->count-1] = *statement;
}

void addExpressionToExpressionList( ExpressionList* el, Expression* expression )
{
    el->count++;
    el->expressions = resizeArray( el->expressions, Expression, el->count );
    el->expressions[el->count-1] = *expression;
}

void parseProgram( Scanner* s, SyntaxContext* ctx, Function* main )
{

    *main = (Function){ 
        .type = USER_DEFINED, 
        .value.userDefined = { 
            .statements = {
                .item = NULL,
                .count = 0
            }
        }
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
    Function func = {
        .type = USER_DEFINED,
        .value.userDefined = {
            .statements = {
                .item = NULL,
                .count = 0
            },
            .variableCount = 0
        },
        .paramCount = 0
    };
    
    SymbolTable localSymbols = newSymbolTable();
    
    SyntaxContext newCtx = {
        .globalSymbols = ctx->globalSymbols,
        .localSymbols = &localSymbols,
        .functions = ctx->functions,
    };
    
    expectKeyw( s, kFunction );
    Variable name;
    parseIdentifier( s, &name, &newCtx); // tady to chce taky delat vic
    expectTok( s, tokLParen );
    parseFunctionParameters( s, &func, &newCtx );
    expectTok( s, tokRParen );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd )
    {
        parseStatement( s, &func.value.userDefined.statements, &newCtx );
    }
    consumeTok( s );
    expectTok( s, tokEndOfLine );
    
    freeSymbolTable( &localSymbols );
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
    Assignment ass;
    ass.destination = getSymbol(getTok(s).data.id,ctx->globalSymbols, ctx->localSymbols ); 
    consumeTok(s);
    parseExpression(s, &ass.source, ctx);
    expectTok(s, tokEndOfLine);
    
    Statement* statement = new(Statement);
    *statement = (Statement){ .type=ASSIGNMENT, .value.assignment=ass };
    addStatementToStatementList(sl,statement);
}

// Konzumuje token.
void parseSubstring( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Substring substr;
    substr.destination = getSymbol(getTok(s).data.id,ctx->globalSymbols, ctx->localSymbols );
    consumeTok(s);
    
    if (getTok(s).type == tokLiteral)
        substr.source = (Expression){ .type=CONSTANT, .value.constant=getTok(s).data.val };
    else
        substr.source = (Expression){ .type=VARIABLE, .value.variable=getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
    consumeTok(s);
    
    assert(getTok(s).type == tokLBracket);	// V kontextu s detectAssignment().
    consumeTok(s);
    
    if (getTok(s).type & (tokId | tokLiteral)) {
        if (getTok(s).type == tokLiteral)
            substr.offset = (Expression){ .type=CONSTANT, .value.constant=getTok(s).data.val };
        else
            substr.offset = (Expression){ .type=VARIABLE, .value.variable=getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
        consumeTok(s);
    }
    else if (getTok(s).type == tokColon) {
        substr.offset = (Expression){ .type=CONSTANT, .value.constant=newValueUndefined() };
    }
    
    expectTok(s, tokColon);
    
    if (getTok(s).type & (tokId | tokLiteral)) {
        if (getTok(s).type == tokLiteral)
            substr.length = (Expression){ .type=CONSTANT, .value.constant=getTok(s).data.val };
        else
            substr.length = (Expression){ .type=VARIABLE, .value.variable=getSymbol(getTok(s).data.id,ctx->globalSymbols,ctx->localSymbols) };
        consumeTok(s);
    }
    else if (getTok(s).type == tokRBracket) {
        substr.length = (Expression){ .type=CONSTANT, .value.constant=newValueUndefined() };
    }
    
    expectTok(s, tokRBracket);
    expectTok(s, tokEndOfLine);
    
    Statement* statement = new(Statement);
    *statement = (Statement){ .type=SUBSTRING, .value.substring=substr };
    addStatementToStatementList(sl,statement);
}

// Konzumuje token.
void parseIf( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt;
    stmt.type = CONDITION;
    
    expectKeyw( s, kIf );
    parseExpression( s, &stmt.value.condition.condition, ctx );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kElse )
    {
        parseStatement( s, &stmt.value.condition.ifTrue, ctx );
    }
    expectKeyw( s, kElse );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kElse )
    {
        parseStatement( s, &stmt.value.condition.ifFalse, ctx);
    }
    expectKeyw( s, kElse );
    expectTok( s, tokEndOfLine );
    addStatementToStatementList( sl, &stmt );
}

// Konzumuje token.
void parseWhile( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt;
    stmt.type = LOOP;
    
    expectKeyw( s, kWhile );
    parseExpression( s, &stmt.value.loop.condition, ctx );
    expectTok( s, tokEndOfLine );
    while( getTok( s ).type == tokKeyW && getTok( s ).data.keyw == kEnd )
    {
        parseStatement( s, &stmt.value.loop.statements, ctx );
    }
    expectKeyw( s, kEnd );
    expectTok( s, tokEndOfLine );
}

bool compareOperators(Operator op1, Operator op2){
    // unární operátory jsou prioritnější
    if(op2.type==UNARYOP) return true;
    if(op1.type==UNARYOP) return false;
    // jinak je prioritnejší operátor s vyšší hodnotou prvních 4 bitů
    return ( (op1.value.binary.type & 0xF0) > (op2.value.binary.type & 0xF0) );
}

void parseExpression( Scanner* s, Expression* wholeExpression, SyntaxContext* ctx )
{
    Token current;
    Expression *newExp, *prevExp=NULL, *subExp=NULL;
    enum {
        wasStart,
        wasParentheses,
        wasId,
        wasLiteral,
        wasOperator,
        
    } past=wasStart;
    
    while((current=getTok(s)).type!=tokEndOfFile){
        switch(current.type){
            case tokId:
                printf("ctuId\n");
                if(past==wasParentheses){ // operand je hned za (pravou!) zavorkou - chyba
                  throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                }
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    // pripojit za nejnovejsi operator
                    if(prevExp->value.operator.type==BINARYOP){
                        prevExp->value.operator.value.binary.right = newExp;
                        newExp->parent = prevExp;
                    }else{
                        prevExp->value.operator.value.unary.operand = newExp;
                        newExp->parent = prevExp;
                    }
                    newExp->parent = prevExp;
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                }
                // TODO: az aspon trochu pojede, implementovat volani funkce ctenim budoucnosti
/*
                }else if(past==wasId){ // zavorka volani funkce
                    printf("jeToVolaniFunkce\n");
                    Token previousToken;
                    // priprava seznamu parametru
                    ExpressionList params = {NULL,0};
                    do{
                        consumeTok(s); // zkonzumovat (
                        printf("ctuParametr(%d)\n",params.count);
                        parseExpression(s,subExp,ctx);
                        addExpressionToExpressionList(&params,subExp);
                    }while(getTok(s).type==tokComma);
                    // sestaveni FunctionCall
                    prevExp->type = FUNCTION_CALL;
                    prevExp->value.functionCall = (FunctionCall){
                        .params=params,
                        //.function=getSymbol(pt.data.id,ctx->globalSymbols,NULL) // TODO: kvuli tomuto to bude muset do id
                    };
                    newExp = prevExp; // nevznikla nova expression, jen jsme zmenili VARIABLE na FUNCTION_CALL
*/

                *newExp = (Expression){
                    .type=VARIABLE,
                    .value.variable=getSymbol(current.data.id,ctx->globalSymbols,ctx->localSymbols)
                };
                past = wasId;
            break;
            case tokLiteral:
                printf("ctuLiteral\n");
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    // pripojit za nejnovejsi operator
                    if(prevExp->value.operator.type==BINARYOP){
                        prevExp->value.operator.value.binary.right = newExp;
                        newExp->parent = prevExp;
                    }else{
                        prevExp->value.operator.value.unary.operand = newExp;
                        newExp->parent = prevExp;
                    }
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                }
                *newExp = (Expression){ .type=CONSTANT, .value.constant=current.data.val };
                past = wasLiteral;
            break;
            case tokOp:
                printf("ctuOperator\n");
                if(past==wasStart){ // prvnim tokenem vyrazu?
                    if(current.data.op==opMinus){
                        newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu - unarni minus)
                        *newExp = (Expression){ .type=OPERATOR };
                        newExp->value.operator.type = UNARYOP;
                        newExp->value.operator.value.unary.type = MINUS;
                        newExp->parent = NULL;
                    }else{
                        throw(SyntaxError,((SyntaxErrorException){.type=BinaryOperatorAtBegin}));
                    }
                }else{ // ve vyrazu jiz je promenna/konstanta
                    
                    if(past==wasOperator){ // pred timto operatorem je dalsi operator - nepripustne
                        throw(SyntaxError,((SyntaxErrorException){.type=TwoOperatorsNextToEachOther}));
                    }
                    
                    // Je-li operace mene prioritni nez predchozi, misto predchozi operace
                    // pujdeme na misto jeste predchodnejsi (parentnejsi) operace
                    while(
                      prevExp->parent!=NULL &&
                      compareOperators(prevExp->parent->value.operator, newExp->value.operator)
                    ){
                      printf("lezuVys\n");
                      prevExp = prevExp->parent;
                    }
                    
                    newExp->parent = prevExp->parent;
                    if(prevExp->parent==NULL){
                      printf("je novym korenem\n");
                      subExp = new(Expression);
                      *subExp = *wholeExpression;
                      if(subExp->value.operator.value.binary.left)
                          subExp->value.operator.value.binary.left->parent = subExp;
                      if(subExp->value.operator.value.binary.right)
                          subExp->value.operator.value.binary.right->parent = subExp;
                      prevExp = subExp;
                      newExp = wholeExpression;
                    }else{
                      newExp = new(Expression);
                      if(newExp->parent->value.operator.type==BINARYOP){
                        printf("nahrazuje binarni\n");
                        newExp->parent->value.operator.value.binary.right = newExp;
                      }else{
                        printf("nahrazuje unarni\n");
                        newExp->parent->value.operator.value.unary.operand = newExp;
                      }
                    }
                    prevExp->parent = newExp;
                    newExp->type = OPERATOR;
                    newExp->value.operator.type = BINARYOP;
                    newExp->value.operator.value.binary.left = prevExp;
                    switch(current.data.op){
                        case opPlus:     newExp->value.operator.value.binary.type = ADD; break;
                        case opMinus:    newExp->value.operator.value.binary.type = SUBTRACT; break;
                        case opMultiple: newExp->value.operator.value.binary.type = MULTIPLY; break;
                        case opDivide:   newExp->value.operator.value.binary.type = DIVIDE; break;
                        case opPower:    newExp->value.operator.value.binary.type = POWER; break;
                        case opEQ:       newExp->value.operator.value.binary.type = EQUALS; break;
                        case opNE:       newExp->value.operator.value.binary.type = NOTEQUALS; break;
                        case opLT:       newExp->value.operator.value.binary.type = LESS; break;
                        case opGT:       newExp->value.operator.value.binary.type = GREATER; break;
                        case opLE:       newExp->value.operator.value.binary.type = LEQUAL; break;
                        case opGE:       newExp->value.operator.value.binary.type = GEQUAL; break;
                    }
                }
                past = wasOperator;
            break;
            case tokLParen:
                printf("ctuZavorku\n");
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    prevExp->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
                    newExp->parent = prevExp;
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                }
                parseExpression(s,newExp,ctx);
                if(getTok(s).type!=tokRParen){
                    throw(SyntaxError,((SyntaxErrorException){.type=UnterminatedParentheses}));
                }
            break;
            case tokRParen: case tokComma: case tokEndOfLine:
                printf("konecVyrazu\n");
                if(past==wasOperator){ // na konci vyrazu nesmi byt operator
                  throw(SyntaxError,((SyntaxErrorException){.type=OperatorAtTheEnd}));
                }
                return; // konec parsovani expression, schvalne nekonzumuji!
            break;
            default:
                throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression}));
            break;
        } // endswitch
        prevExp = newExp;
        printf("token(%d)\n",getTokN(s,0).type);
        consumeTok(s);
    } // endwhile
    printf("konecVyrazuAsouboruZaroven\n");
    if(past==wasOperator){ // na konci vyrazu nesmi byt operator
        throw(SyntaxError,((SyntaxErrorException){.type=OperatorAtTheEnd}));
    }
    return; // konec parsovani expression a souboru zaroven, nekonzumuji!
}

void parseIdentifier( Scanner* s, Variable* id, SyntaxContext* ctx )
{
    testTok( s, tokId );
    RCString name = getTok( s ).data.id;
    *id = getSymbol( name, ctx->globalSymbols, ctx->localSymbols );
    deleteRCString( &name );
    consumeTok( s );
}
