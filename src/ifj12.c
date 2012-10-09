#include <stdio.h>
#include "value.h"
#include "symbols.h"
#include "exceptions.h"

int main(int argc, char**argv)
{
 exceptions_init();
 
 SymbolTable global = {NULL,0};
 SymbolTable local = {NULL,0};
 
 printf("%d\n", getSymbol("test",&global,&local) );
 printf("%d\n", getSymbol("test",&global,&local) );
 printf("%d\n", getSymbol("test2",&global,&local) );
 printf("%d\n", getSymbol("test3",&global,&local) );
 printf("%d\n", getSymbol("funkce1",&global,NULL) );
 printf("%d\n", getSymbol("funkce2",&global,NULL) );
 printf("%d\n", getSymbol("funkce1",&global,NULL) );
 printf("%d\n", getSymbol("globalVar",&global,NULL) ); // pro test makra SYMBOL
 
 freeSymbolTable(&global);
 freeSymbolTable(&local);
 
 Value *globalTable = initValueTable(global.count);
 Value *localTable = initValueTable(local.count);
 Context context = (Context){globalTable,localTable};
 
 char *s;
 
 setValueString(symbol(1,&context),"zkouska");
 printf("[%s]\n", s = getValueString(symbol(1,&context)) );
 free(s);
 
 setValueString(symbol(-3,&context),"zkouska2");
 printf("[%s]\n", s = getValueString(symbol(-3,&context)) );
 free(s);
 
 freeValueTable(globalTable,global.count);
 freeValueTable(localTable,local.count);
 
 return 0;
}

