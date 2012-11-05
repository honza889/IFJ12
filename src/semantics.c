#include "semantics.h"
#include "syntax.h"
#include "symbols.h"
#include "ast.h"
#include "exceptions.h"
#include "alloc.h"


/**
 * Přidá položku do StatementListu
 */
void AddToStatementList(StatementList *list, Statement newSt){
    if(list->item==NULL){ // novy seznam
        list->count=1;
        list->item = new( Statement );
    }else{ // pridani do neprazdneho seznamu
        list->count++;
        list->item = resizeArray( list->item, Statement, list->count );
    }
    memcpy(&list->item[list->count-1],&newSt,sizeof(Statement));
}

/**
 * Přidá položku do ExpressionListu
 */
void AddToExpressionList(ExpressionList *list, Expression newExp){
    if(list->expressions==NULL){ // novy seznam
        list->count=1;
        list->expressions = new( Expression );
    }else{ // pridani do neprazdneho seznamu
        list->count++;
        list->expressions = resizeArray( list->expressions, Expression, list->count );
    }
    memcpy(&list->expressions[list->count-1],&newExp,sizeof(Expression));
}


static inline void freeStatementList(StatementList *list){
    free(list->item);
    list->item=NULL;
    list->count=0;
}


/**
 * Porovná prioritu operátorů
 * (Používá se při sestavování Expression - je-li druhý operátor méně prioritní (patří výše), vrátí true)
 * @return Jestli je 1. vice prioritní než 2.
 */
/*
bool compareOperators(Operator op1, Operator op2){
    // unární operátory jsou prioritnější
    if(op2.type==UNARYOP) return true;
    if(op1.type==UNARYOP) return false;
    // jinak je prioritnejší operátor s vyšší hodnotou prvních 4 bitů
    return ( (op1.value.binary.type & 0xF0) > (op2.value.binary.type & 0xF0) );
}
*/

/**
 * Čte Tokeny až sestaví Expression
 */
/*
Expression* semanticOfExpression(FILE *f, SymbolTable *global, SymbolTable *local, Token *lastToken){
    Expression *wholeExpression = NULL;
    Expression *newExp, *oldExp, *subExp, *tmp;
    Token t; // aktualni token
    Token pt = {.type=tokEndOfFile}; // predchazejici token
    while((t=syntax(f)).type!=tokEndOfFile){
        switch(t.type){
            case tokId:
                printf("ctuId\n");
                if(pt.type==tokLParen){ // operand je hned za (pravou!) zavorkou - chyba
                  throw(SyntaxError,((SyntaxErrorException){.type=OperatorAtTheEnd}));
                }
                newExp = new( Expression );
                *newExp = (Expression){ .type=VARIABLE, .value.variable=getSymbol(t.data.id,global,local) };
                if(pt.type==tokEndOfFile){
                    wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(pt.type==tokOp){
                    oldExp->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
                    newExp->parent = oldExp;
                }
            break;
            case tokLiteral:
                printf("ctuLiteral\n");
                newExp = new( Expression );
                *newExp = (Expression){ .type=CONSTANT, .value.constant=t.data.val };
                if(pt.type==tokEndOfFile){
                    wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(pt.type==tokOp){
                    // pripojit za nejnovejsi operator
                    if(oldExp->value.operator.type==BINARYOP){
                        oldExp->value.operator.value.binary.right = newExp;
                        newExp->parent = oldExp;
                    }else{
                        oldExp->value.operator.value.unary.operand = newExp;
                        newExp->parent = oldExp;
                    }
                }
            break;
            case tokOp:
                printf("ctuOperator\n");
                newExp = new( Expression );
                *newExp = (Expression){ .type=OPERATOR };
                if(pt.type==tokEndOfFile){ // prvnim tokenem vyrazu?
                    if(t.data.op==opMinus){
                        newExp->value.operator.type = UNARYOP;
                        newExp->value.operator.value.unary.type = MINUS;
                        wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu - unarni minus)
                        newExp->parent = NULL;
                    }else{
                        throw(SyntaxError,((SyntaxErrorException){.type=BinaryOperatorAtBegin}));
                    }
                }else{ // ve vyrazu jiz je promenna/konstanta
                    
                    if(pt.type==tokOp){ // pred timto operatorem je dalsi operator - nepripustne
                        throw(SyntaxError,((SyntaxErrorException){.type=TwoOperatorsNextToEachOther}));
                    }
                    
                    newExp->value.operator.type = BINARYOP;
                    switch(t.data.op){
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
                    
                    // Je-li operace mene prioritni nez predchozi, misto predchozi operace
                    // pujdeme na misto jeste predchodnejsi (parentnejsi) operace
                    
                    while(
                      oldExp->parent!=NULL &&
                      compareOperators(oldExp->parent->value.operator, newExp->value.operator)
                    ){
                      printf("lezuVys\n");
                      oldExp = oldExp->parent;
                    }
                    
                    newExp->parent = oldExp->parent;
                    if(oldExp->parent==NULL){
                      printf("je novym korenem\n");
                      wholeExpression = newExp;
                    }else{
                      if(newExp->parent->value.operator.type==BINARYOP){
                        printf("nahrazuje binarni\n");
                        newExp->parent->value.operator.value.binary.right = newExp;
                      }else{
                        printf("nahrazuje unarni\n");
                        newExp->parent->value.operator.value.unary.operand = newExp;
                      }
                    }
                    oldExp->parent = newExp;
                    if(newExp->value.operator.type==BINARYOP){
                      newExp->value.operator.value.binary.left = oldExp;
                    }else{
                      throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                    }
                }
            break;
            case tokLParen:
                printf("ctuZavorku\n");
                if(pt.type==tokEndOfFile){
                    newExp = semanticOfExpression(f,global,local,NULL);
                    wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu)
                    newExp->parent = NULL;
                }else if(pt.type==tokOp){
                    newExp = semanticOfExpression(f,global,local,NULL);
                    oldExp->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
                    newExp->parent = oldExp;
                }else if(pt.type==tokId){ // zavorka volani funkce
                    printf("jeToVolaniFunkce\n");
                    Token previousToken;
                    // priprava seznamu parametru
                    ExpressionList params = {NULL,0};
                    do{
                        printf("ctuParametr(%d)\n",params.count);
                        subExp = semanticOfExpression(f,global,local,&previousToken);
                        if(subExp == NULL) break;
                        AddToExpressionList(&params,*subExp);
                    }while(previousToken.type==tokComma);
                    // sestaveni FunctionCall
                    oldExp->type = FUNCTION_CALL;
                    oldExp->value.functionCall = (FunctionCall){
                        .params=params,
                        .function=getSymbol(pt.data.id,global,NULL)
                    };
                    newExp = oldExp; // nevznikla nova expression, jen jsme zmenili VARIABLE na FUNCTION_CALL
                }else{
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
                }
            break;
            case tokRParen: case tokComma: case tokEndOfLine:
                printf("konecVyrazu\n");
                if(pt.type==tokOp){ // na konci vyrazu nesmi byt operator
                  throw(SyntaxError,((SyntaxErrorException){.type=OperatorAtTheEnd}));
                }
                if(lastToken) *lastToken = t;
                return wholeExpression;
            break;
            default:
                throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression}));
        }
        pt = t;
        oldExp = newExp;
    }
    return wholeExpression; // jen pro kompilator, skutecny return je vyse!
}
*/

/**
 * Čte Tokeny až sestaví Function
 * Volána na začátku souboru (pro načtení main) nebo
 * po vstupu do definice funkce (pro načtení této funkce)
 */
/*
Function semantics(int paramCount,FILE *f,SymbolTable *global){
    Token t;
    StatementList list = newStatementList();
    SymbolTable local = newSymbolTable();
    RCString id;
    bool wasntEnd = true;
    
    // Čtení jednotlivých Statementů
    while(wasntEnd && (t=syntax(f)).type!=tokEndOfFile){
        
        switch(t.type){
            // Statement začíná Id - zřejmě id = ...
            case tokId:
                id = t.data.id; // t bude prepsano tokAssignem
                if((t=syntax(f)).type!=tokAssign){ // musi nasledovat operator prirazeni
                    throw(SyntaxError,((SyntaxErrorException){.type=AssignWithoutAssignOperator}));
                }
                printf("ID\n");
                AddToStatementList(&list, (Statement){
                    .type=ASSIGNMENT,
                    .value.assignment={
                        .destination=getSymbol(id,global,&local),
                        .source=*semanticOfExpression(f,global,&local,NULL)
                    }
                });
            break;
            case tokKeyW:
                switch(t.data.keyw){
                    case kEnd:
                        wasntEnd = false; // ukonci nacitani funkce
                    break;
                    case kReturn:
                        printf("RETURN\n");
                        AddToStatementList(&list, (Statement){
                            .type=RETURN,
                            .value.ret=*semanticOfExpression(f,global,&local,NULL)
                        });
                    break;
                    default: ERROR("Neimplementovane klicove slovo!");exit(99); // nevyjikovat, bude odstraneno po implementaci
                }
                
            break;
            case tokEndOfLine: break;
            default:
                throw(SyntaxError,((SyntaxErrorException){.type=BadTokenAtBeginOfStatement}));
        }
        
    }
    
    int variableCount = local.count;
    freeSymbolTable(&local);
    
    return (Function){
        .type=USER_DEFINED,
        .value.userDefined={
            .statements=list,
            .variableCount=variableCount
        },
        .paramCount=paramCount
    };
}
*/

