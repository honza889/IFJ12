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
 * Uvolní StatementList
 */
void freeStatementList(StatementList *list){
	free(list->item);
	list->item=NULL;
	list->count=0;
}

/**
 * Čte Tokeny až sestaví Expression
 */
Expression* semanticOfExpression(FILE *f, SymbolTable *global, SymbolTable *local, Token *lastToken){
	Expression *wholeExpression = NULL;
	Expression *newExp, *old, *tmp;
	Token t; // aktualni token
	Token pt = {.type=tokEndOfFile}; // predchazejici token
	while((t=syntax(f)).type!=tokEndOfFile){
		switch(t.type){
			case tokId:
				printf("ctuId\n");
				newExp = new( Expression );
				*newExp = (Expression){ .type=VARIABLE, .value.variable=getSymbol(t.data.id,global,local) };
				if(pt.type==tokEndOfFile){
					wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu)
					newExp->parent = NULL;
				}else if(pt.type==tokOp){
					old->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
					newExp->parent = old;
				}
			break;
			case tokNum:
				printf("ctuNum\n");
				newExp = new( Expression );
				*newExp = (Expression){ .type=CONSTANT, .value.constant=t.data.val };
				if(pt.type==tokEndOfFile){
					wholeExpression = newExp; // do korene (je prvnim prvkem vyrazu)
					newExp->parent = NULL;
				}else if(pt.type==tokOp){
					old->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
					newExp->parent = old;
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
					newExp->parent = old->parent;
					old->parent = newExp;
					newExp->value.operator.value.binary.left = old;
					newExp->value.operator.value.binary.right = NULL;
					if(newExp->parent==NULL){ // je-li exitujici promenna/konstanta korenem
						wholeExpression = newExp; // bude novy operator novym korenem
					}else{ // neni-li korenem
						if(newExp->parent->type!=OPERATOR){
							ERROR("Interni chyba interpretru pri sestavovani AST: Rodicem prvku ve vyrazu neni operator ani NULL!");
							exit(99);
						}
						
						// zde by jeste musim udelat hop nahoru, je-li novy operator mensi priority nez stary
						if(newExp->parent->value.operator.type==BINARYOP){
							newExp->parent->value.operator.value.binary.right = newExp;
						}else{
							newExp->parent->value.operator.value.unary.operand = newExp;
						}
						
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
					old->value.operator.value.binary.right = newExp; // pripojit za nejnovejsi operator
					newExp->parent = old;
				}else if(pt.type==tokId){ // zavorka volani funkce
					// čeká na standardizaci ExpressionListu
					//FunctionCall call={.params={NULL,0}, .function=getSymbol(pt.data.id,global,NULL) };
					Token previousToken;
					int paramsCount=0;
					do{
						newExp = semanticOfExpression(f,global,local,&previousToken);
						if(newExp == NULL) break;
						printf("ctuParametr\n");
						paramsCount++;
					}while(previousToken.type==tokComma);
					printf("docteno=%d\n",paramsCount);
					
				}else{
					throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax}));
				}
			break;
			case tokRParen: case tokComma: case tokEndOfLine:
				printf("konecVyrazu\n");
				if(lastToken) *lastToken = t;
				return wholeExpression;
			break;
			default:
				throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression}));
		}
		pt = t;
		old = newExp;
	}
	return wholeExpression;
}

/**
 * Čte Tokeny až sestaví Function
 * Volána na začátku souboru (pro načtení main) nebo
 * po vstupu do definice funkce (pro načtení této funkce)
 */
Function semantics(int paramCount,FILE *f,SymbolTable *global){
	Token t;
	StatementList list = newStatementList();
	SymbolTable local = newSymbolTable();
	RCString id;
	bool wasntEnd = true;
	
	// Čtení jednotlivých Statementů
	while(wasntEnd && (t=syntax(f)).type!=tokEndOfFile){
		
		switch(t.type){
			/* Statement začíná Id - zřejmě id = ... */
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

