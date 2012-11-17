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
#include "symbols.h"
#include "bif.h"
#include "ial.h"

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
    // BUG: Následující dva zakomentované řádky způsobovaly leak a ani nevím k čemu byli. (Jan Doležal)
//     main->value.userDefined.variableNames = newArray( RCString, ctx->localSymbols->count );
//     fillNameListFromSymbolTree( main->value.userDefined.variableNames, ctx->localSymbols->root );
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
    // BUG: Následující dva zakomentované řádky způsobovaly leak a ani nevím k čemu byli. (Jan Doležal)
//     func->value.userDefined.variableNames = newArray( RCString, ctx->localSymbols->count );
//     fillNameListFromSymbolTree( func->value.userDefined.variableNames, ctx->localSymbols->root );
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
    assgn.destination = getSymbol(name,ctx->globalSymbols, ctx->localSymbols );
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
    substr.destination = getSymbol(name,ctx->globalSymbols, ctx->localSymbols );
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
        substr.offset = (Expression){ .type=CONSTANT, .value.constant=newValueUndefined() };
    }
    
    expectTok(s, tokColon);
    
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

bool compareOperators(Operator op1, Operator op2){
    // unární operátory jsou prioritnější
    if(op2.type==UNARYOP) return true;
    if(op1.type==UNARYOP) return false;
    // jinak je prioritnejší operátor s vyšší hodnotou prvních 4 bitů
    return ( (op1.value.binary.type & 0xF0) > (op2.value.binary.type & 0xF0) );
}

void parseExpression( Scanner* s, Expression* wholeExpression, SyntaxContext* ctx )
{
    // Porad mam dojem, ze tohle lze napsat lepe, ale co se da delat... --Biba
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
                    newExp->value.functionCall.function = getFunctionId( ctx, &current.data.id );
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
                    newExp->value.variable = getSymbol(current.data.id,ctx->globalSymbols,ctx->localSymbols);
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
                newExp->value.constant=copyValue(&current.data.val);
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
                    
                    Operator tmpOp;
                    tmpOp.type = BINARYOP;
                    switch(current.data.op){
                        case opPlus:     tmpOp.value.binary.type = ADD; break;
                        case opMinus:    tmpOp.value.binary.type = SUBTRACT; break;
                        case opMultiple: tmpOp.value.binary.type = MULTIPLY; break;
                        case opDivide:   tmpOp.value.binary.type = DIVIDE; break;
                        case opPower:    tmpOp.value.binary.type = POWER; break;
                        case opEQ:       tmpOp.value.binary.type = EQUALS; break;
                        case opNE:       tmpOp.value.binary.type = NOTEQUALS; break;
                        case opLT:       tmpOp.value.binary.type = LESS; break;
                        case opGT:       tmpOp.value.binary.type = GREATER; break;
                        case opLE:       tmpOp.value.binary.type = LEQUAL; break;
                        case opGE:       tmpOp.value.binary.type = GEQUAL; break;
                    }
                    
                    Expression *parentExp;
                    parentExp = prevExp->parent;
                    // Je-li operace mene prioritni nez predchozi, misto predchozi operace
                    // pujdeme na misto jeste predchodnejsi (parentnejsi) operace
                    while(
                      parentExp!=NULL &&
                      compareOperators(prevExp->parent->value.operator, tmpOp)
                    ){
                      parentExp = parentExp->parent;
                    }
                    // parentExp je rodic newExp (nebo NULL)
                    // prevExp je levy potomek newExp
                    
                    if(parentExp==NULL){ // je novym korenem
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
                      newExp = wholeExpression;
                      newExp->parent = NULL;
                      
                      newExp->type = OPERATOR;
                      newExp->value.operator = tmpOp;
                      newExp->value.operator.value.binary.left = oldWholeExp;
                      newExp->value.operator.value.binary.left->parent = newExp;
                      newExp->value.operator.value.binary.right = NULL;
                      
                    }else{
                      newExp = new(Expression);
                      newExp->parent = parentExp;
                      
                      assert(newExp->parent!=NULL);
                      assert(newExp->parent->type==OPERATOR);
                      
                      if(newExp->parent->value.operator.type==BINARYOP){
                        newExp->parent->value.operator.value.binary.right = newExp;
                      }else{
                        newExp->parent->value.operator.value.unary.operand = newExp;
                      }
                      
                      newExp->type = OPERATOR;
                      newExp->value.operator = tmpOp;
                      newExp->value.operator.value.binary.left = prevExp;
                      newExp->value.operator.value.binary.left->parent = newExp;
                      newExp->value.operator.value.binary.right = NULL;
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
