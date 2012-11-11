#include <assert.h>
#include <stdbool.h>
#include "global.h"
#include "syntax.h"
#include "exceptions.h"
#include "scanner.h"
#include "ast.h"
#include "alloc.h"
#include "symbols.h"
#include "bif.h"

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function );
void addStatementToStatementList( StatementList* sl, Statement* statement );
void addExpressionToExpressionList( ExpressionList* sl, Expression* statement );

void addFunctionToContext( SyntaxContext* ctx, RCString* name, Function* function )
{
    // asi to chce jeste trochu predelat symboly...
    int id = getSymbol( *name, ctx->globalSymbols, ctx->globalSymbols );
    if( id > ctx->globalSymbols->count )
    {
        throw( MultipleFunctionDefinitions, copyRCString( name ) );
    }
    else
    {
        ctx->functions = resizeArray( ctx->functions, Value, ctx->globalSymbols->count );
        ctx->functions[ ctx->globalSymbols->count - 1 ] = newValueFunction( function );
    }
}

void addBuiltinToContext( SyntaxContext* ctx, RCString name, BuiltinFunction function, int parameterCount )
{
    Function* funcObj = new( Function );
    funcObj->type = BUILTIN;
    funcObj->value.builtin = function;
    funcObj->paramCount = parameterCount;
    addFunctionToContext( ctx, &name, funcObj );
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
    addBuiltinToContext( ctx, makeRCString( "find" ), BIFlen, 2 );
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
                .count = 0
            }
        },
        .paramCount = 0
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
    
    Statement statement = { .type=ASSIGNMENT, .value.assignment=ass };
    addStatementToStatementList(sl,&statement);
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
    
    Statement statement = { .type=SUBSTRING, .value.substring=substr };
    addStatementToStatementList(sl,&statement);
}

// Konzumuje token.
void parseIf( Scanner* s, StatementList* sl, SyntaxContext* ctx )
{
    Statement stmt;
    stmt.type = CONDITION;
    
    expectKeyw( s, kIf );
    parseExpression( s, &stmt.value.condition.condition, ctx );	// BUG: Jak to, že projde parseExpression(), když je expression prázdný?
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
    Expression *newExp, *prevExp=NULL, *oldWholeExp=NULL;
    enum {
        wasStart, // zatim nebylo nic, zacatek expression
        wasValue, // byla promenna, literal nebo zavorka (uzavrena)
        wasOperator // byl operator (unarni nebo binarni)
    } past;
    past = wasStart;
    while(true){
        current=getTok(s);
        switch(current.type){
            case tokId:
                if(past==wasValue){ // dve hodnoty hned za sebou?
                  throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=current.line_num}));
                }
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    // pripojit za nejnovejsi operator
                    if(prevExp->value.operator.type==BINARYOP){
                        prevExp->value.operator.value.binary.right = newExp;
                    }else{
                        prevExp->value.operator.value.unary.operand = newExp;
                    }
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=current.line_num}));
                }
                newExp->parent=prevExp;
                
                // id( - Volani funkce
                if(getTokN(s,1).type==tokLParen){
                    newExp->type=FUNCTION_CALL;
                    newExp->value.functionCall.function=getSymbol(current.data.id,ctx->globalSymbols,NULL);
                    newExp->value.functionCall.params = (ExpressionList){NULL,0};
                    consumeTok(s); // zkonzumovat id
                    consumeTok(s); // zkonzumovat '('
                    Expression subExp;
                    TokenType next = getTok(s).type;
                    while(next!=tokRParen){
                        parseExpression(s,&subExp,ctx);
                        addExpressionToExpressionList(&newExp->value.functionCall.params,&subExp);
                        // musi nasledovat ',' nebo ')'
                        next = getTok(s).type;
                        if(next!=tokComma && next!=tokRParen){
                            throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=current.line_num}));
                        }else if(next==tokComma){
                            consumeTok(s); // zkonzumovat ','
                        }
                    }
                // id - Promenna
                }else{
                    newExp->type=VARIABLE;
                    newExp->value.variable=getSymbol(current.data.id,ctx->globalSymbols,ctx->localSymbols);
                }
                past = wasValue;
            break;
            case tokLiteral:
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    // pripojit za nejnovejsi operator
                    if(prevExp->value.operator.type==BINARYOP){
                        prevExp->value.operator.value.binary.right = newExp;
                    }else{
                        prevExp->value.operator.value.unary.operand = newExp;
                    }
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=current.line_num}));
                }
                newExp->type=CONSTANT;
                newExp->value.constant=current.data.val;
                newExp->parent=prevExp;
                past = wasValue;
            break;
            case tokOp:
                if(past==wasStart){ // prvnim tokenem vyrazu?
                    if(current.data.op==opMinus){
                        newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu - unarni minus)
                        *newExp = (Expression){ .type=OPERATOR };
                        newExp->value.operator.type = UNARYOP;
                        newExp->value.operator.value.unary.type = MINUS;
                        newExp->parent = NULL;
                    }else{
                        throw(SyntaxError,((SyntaxErrorException){.type=BinaryOperatorAtBegin, .line_num=current.line_num}));
                    }
                }else{ // ve vyrazu jiz je promenna/konstanta
                    
                    if(past==wasOperator){ // pred timto operatorem je dalsi operator - nepripustne
                        throw(SyntaxError,((SyntaxErrorException){.type=TwoOperatorsNextToEachOther, .line_num=current.line_num}));
                    }
                    
                    // Je-li operace mene prioritni nez predchozi, misto predchozi operace
                    // pujdeme na misto jeste predchodnejsi (parentnejsi) operace
                    while(
                      prevExp->parent!=NULL &&
                      compareOperators(prevExp->parent->value.operator, newExp->value.operator)
                    ){
                      prevExp = prevExp->parent;
                    }
                    
                    newExp->parent = prevExp->parent;
                    if(prevExp->parent==NULL){ // je novym korenem
                      // oldWholeExp je nove umisteni byvaleho korene (wholeExpression)
                      oldWholeExp = new(Expression);
                      *oldWholeExp = *wholeExpression;
                      // pokud byl byvaly koren operator, opravime odkazy z jeho potomku
                      if(oldWholeExp->type==OPERATOR){
                          if(oldWholeExp->value.operator.type==BINARYOP){ // binarni
                              if(oldWholeExp->value.operator.value.binary.left!=NULL){
                                  oldWholeExp->value.operator.value.binary.left->parent = oldWholeExp;
                              }
                              if(oldWholeExp->value.operator.value.binary.right!=NULL){
                                  oldWholeExp->value.operator.value.binary.right->parent = oldWholeExp;
                              }
                          }else{ // unarni
                              oldWholeExp->value.operator.value.unary.operand->parent = oldWholeExp;
                          }
                      }
                      prevExp = oldWholeExp;
                      newExp = wholeExpression;
                    }else{
                      newExp = new(Expression);
                      if(newExp->parent->value.operator.type==BINARYOP){
                        newExp->parent->value.operator.value.binary.right = newExp;
                      }else{
                        newExp->parent->value.operator.value.unary.operand = newExp;
                      }
                    }
                    newExp->type = OPERATOR;
                    newExp->value.operator.type = BINARYOP;
                    newExp->value.operator.value.binary.left = prevExp;
                    if(newExp->value.operator.value.binary.left!=NULL){
                        newExp->value.operator.value.binary.left->parent = newExp;
                    }
                    newExp->value.operator.value.binary.right = NULL;
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
                if(past==wasStart){
                    newExp = wholeExpression; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(past==wasOperator){
                    newExp = new(Expression);
                    prevExp->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
                    newExp->parent = prevExp;
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=current.line_num}));
                }
                consumeTok(s); // odstrani '('
                parseExpression(s,newExp,ctx);
                if(getTok(s).type!=tokRParen){ // byl exp ukoncen ')' ?
                    throw(SyntaxError,((SyntaxErrorException){.type=UnterminatedParentheses, .line_num=current.line_num}));
                }
                past = wasValue;
            break;
            case tokRParen: case tokComma: case tokEndOfLine:
                if(past==wasOperator){ // na konci vyrazu nesmi byt operator
                  throw(SyntaxError,((SyntaxErrorException){.type=OperatorAtTheEnd, .line_num=current.line_num}));
                }
                if(past==wasStart){ // vyraz nesmi byt prazdny
                  throw(SyntaxError,((SyntaxErrorException){.type=BlankExpression, .line_num=current.line_num}));
                }
                return; // konec parsovani expression, nekonzumuji - ukoncujici token zustane
            break;
            default:
                throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression, .line_num=current.line_num}));
            break;
        } // endswitch
        prevExp = newExp;
        consumeTok(s);
    } // endwhile
}

void parseIdentifier( Scanner* s, Variable* id, SyntaxContext* ctx )
{
    testTok( s, tokId );
    RCString name = getTok( s ).data.id;
    *id = getSymbol( name, ctx->globalSymbols, ctx->localSymbols );
    deleteRCString( &name );
    consumeTok( s );
}

void syntaxErrorPrint(SyntaxErrorException e) {
  switch (e.type) {	// TODO: Dát tam nějaké hlášky!
    case BadTokenInExpression:		fprintf(stderr,"Parse error: Bad token in expression"); break;
    case BadTokenAtBeginOfStatement:	fprintf(stderr,"Parse error: Bad begin of statement"); break;
    case AssignWithoutAssignOperator:	fprintf(stderr,"Parse error: Assign without assign operator"); break;
    case StrangeSyntax:			fprintf(stderr,"Parse error: StrangeSyntax"); break;
    case BinaryOperatorAtBegin:		fprintf(stderr,"Parse error: Binary operator at begin of expression"); break;
    case OperatorAtTheEnd:		fprintf(stderr,"Parse error: Operator at the end of the expression"); break;
    case TwoOperatorsNextToEachOther:	fprintf(stderr,"Parse error: Two operators next to each other"); break;
    case UnterminatedParentheses:	fprintf(stderr,"Parse error: Unclosed parentheses"); break;
    case BadStartOfStatement:		fprintf(stderr,"Parse error: Bad start of statement"); break;
    case BlankExpression:		fprintf(stderr,"Parse error: Blank expression"); break;
  }
  fprintf(stderr," on line %d\n",e.line_num);
}
