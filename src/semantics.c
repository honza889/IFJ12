#include "semantics.h"
#include "syntax.h"
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

Function semantics(FILE *f){
	Token t;
	StatementList list=newStatementList();
	
	
	while((t=syntax(f)).type!=tokEndOfFile){
		printf("cteni\n");
	}
	
	AddToStatementList(&list, (Statement){.type=ASSIGNMENT});
	
	
	
	freeStatementList(&list);
	return (Function){
		.type=USER_DEFINED,
		.value.userDefined={
			.statements=list,
			.variableCount=0
		},
		.paramCount=0
	};
}

