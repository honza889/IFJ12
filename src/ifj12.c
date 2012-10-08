#include <stdio.h>
#include "value.h"
#include "symbols.h"

int main(int argc, char**argv)
{
 
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
 
 Value *globalTable = initValueTable(&global);
 Value *localTable = initValueTable(&local);
 Context context = (Context){globalTable,localTable};
 
 char *s;
 
 setValueString(symbol(1,&context),"zkouska");
 printf("[%s]\n", s = getValueString(symbol(1,&context)) );
 free(s);
 
 setValueString(symbol(-3,&context),"zkouska2");
 printf("[%s]\n", s = getValueString(symbol(-3,&context)) );
 free(s);
 
 freeValueTable(globalTable,&global);
 freeValueTable(localTable,&local);
 
 return 0;
}

