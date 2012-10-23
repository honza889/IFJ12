#include "semantics.h"
#include "syntax.h"
#include "symbols.h"
#include "ast.h"
#include "exceptions.h"

static inline StatementList newStatementList(){
	return (StatementList){ .item=NULL, .count=0 };
}

void AddToStatementList(StatementList *list, Statement new){
	if(list->item==NULL){ // novy seznam
		list->count=1;
		list->item=malloc(sizeof(Statement));
	}else{ // pridani do neprazdneho seznamu
		list->count++;
		list->item=realloc(list->item,list->count*sizeof(Statement));		
	}
	MALLCHECK(list->item);
	memcpy(&new,&list->item[list->count-1],sizeof(Statement));
}

void freeStatementList(StatementList *list){
	free(list->item);
	list->item=NULL;
	list->count=0;
}

/** Pro ladeni */
void printExpression(Expression e,int odsazeni){
	for(int i=0;i<odsazeni;i++) printf(" ");
	printf("Ex\n");
	if(e.type==OPERATOR){
		if(e.value.operator.type==UNARYOP){
			printExpression(*e.value.operator.value.unary.operand,odsazeni+2);
		}else{
			printExpression(*e.value.operator.value.binary.left,odsazeni+2);
			printExpression(*e.value.operator.value.binary.right,odsazeni+2);
		}
	}
}

/** Sestavi z tokenu Expression */
Expression semanticOfExpression(FILE *f,SymbolTable *global,SymbolTable *local){
	Expression *wholeExpression = NULL;
	Expression *new, *old, *tmp;
	Token t; // aktualni token
	Token pt = {.type=tokEndOfFile}; // predchazejici token
	while((t=syntax(f)).type!=tokEndOfFile){
		switch(t.type){
			case tokId:
				printf("ctuId\n");
				new = malloc(sizeof(Expression));
				*new = (Expression){ .type=VARIABLE, .value.variable=getSymbol("promenna",global,local) };
				if(pt.type==tokEndOfFile){
					wholeExpression = new; // do korene (je prvnim prvkem vyrazu)
					new->parent = NULL;
				}else if(pt.type==tokOp){
					old->value.operator.value.binary.right = new; // pripojit za nejnovejsi operator
					new->parent = old;
				}
			break;
			case tokNum:
				printf("ctuNum\n");

				new = malloc(sizeof(Expression));
				*new = (Expression){ .type=CONSTANT, .value.constant=t.data.val };
				if(pt.type==tokEndOfFile){
					wholeExpression = new; // do korene (je prvnim prvkem vyrazu)
					new->parent = NULL;
				}else if(pt.type==tokOp){
					old->value.operator.value.binary.right = new; // pripojit za nejnovejsi operator
					new->parent = old;
				}
			break;
			case tokOp:
				printf("ctuOperator\n");
				new = malloc(sizeof(Expression));
				*new = (Expression){ .type=OPERATOR };
				switch(t.data.op){
					case opPlus: new->value.operator.value.binary.type = ADD; break;
					case opMinus: new->value.operator.value.binary.type = SUBTRACT; break;
					case opMultiple: new->value.operator.value.binary.type = MULTIPLY; break;
					case opDivide: new->value.operator.value.binary.type = DIVIDE; break;
					default: exit(99);
				}
				if(pt.type==tokEndOfFile){ // prvnim tokenem vyrazu?
					wholeExpression = new; // do korene (je prvnim prvkem vyrazu - unarni minus)
					new->parent = NULL;
				}else{ // ve vyrazu jiz neco je (old je promenna/konstanta)
					new->parent = old->parent;
					old->parent = new;
					new->value.operator.value.binary.left = old;
					new->value.operator.value.binary.right = NULL;
					if(new->parent==NULL){ // je-li prvek novym korenem:
						printf("-Predchozi prvek je koren\n");
						wholeExpression = new;
					}else{ // neni-li korenem
						if(new->parent->type!=OPERATOR) ERROR("Rodicem prvku neni operator!");
						printf("-Predchozi prvek neni koren\n");
						if(new->parent->value.operator.type==BINARYOP){
							printf("-Rodic predchoziho prvku je binarni operator\n");
							new->parent->value.operator.value.binary.right = new;
						}else{
							printf("-Rodic predchoziho prvku je unarni operator\n");
							new->parent->value.operator.value.unary.operand = new;
						}
					}
				}
			break;
			
			default: exit(99); // osetrit
		}
		pt = t;
		old = new;
	}
	return *wholeExpression;
}



/**
 * Funkce načte obsah funkce
 * Volána na začátku souboru (pro načtení main)
 * nebo po vstupu do definice funkce (pro načtení této funkce)
 */
Function semantics(int paramCount,FILE *f,SymbolTable *global){
	Token t;
	StatementList list=newStatementList();
	SymbolTable local = {NULL,0};
	
	// Čtení jednotlivých Statementů
	while((t=syntax(f)).type!=tokEndOfFile){
		
		switch(t.type){
			/* Statement začíná Id - zřejmě id = ... */
			case tokId:
				//RCString id = copyRCString(t.data.id);
				if((t=syntax(f)).type!=tokAssign) exit(99); // osetrit
				AddToStatementList(&list, (Statement){
					.type=ASSIGNMENT,
					.value.assignment={
						.destination=getSymbol("promenna",global,&local), // nazev bude treba prevest na RCString
						.source=semanticOfExpression(f,global,&local)
					}
				});

				Expression test = list.item->value.assignment.source;

			break;
			
			default: exit(99); // osetrit
		}
		
	}
	
	return (Function){
		.type=USER_DEFINED,
		.value.userDefined={
			.statements=list,
			.variableCount=0
		},
		.paramCount=0
	};
}

