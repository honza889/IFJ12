#include <stdio.h>
#include "value.h"
#include "symbols.h"
#include "exceptions.h"

int main(int argc, char**argv)
{
 exceptions_init();
 
 SymbolTable global = {NULL,0};
 SymbolTable local = {NULL,0};
 
 printf("%d\n", getSymbol("promenna",&global,&local) );
 printf("%d\n", getSymbol("funkce",&global,NULL) );
 
 freeSymbolTable(&global);
 freeSymbolTable(&local);
 
 Value *globalTable = initValueTable(global.count);
 Value *localTable = initValueTable(local.count);
 Context context = (Context){globalTable,localTable};
 
 char *s;
 
 setValueString(symbol(0,&context),"obsah proměnné");
 printf("[%s]\n", s = getValueString(symbol(0,&context)) );
 free(s);
 
 setValueString(symbol(-1,&context),"obsah >funkce<");
 printf("[%s]\n", s = getValueString(symbol(-1,&context)) );
 free(s);
 
 freeValueTable(globalTable,global.count);
 freeValueTable(localTable,local.count);
 
 return 0;
}

