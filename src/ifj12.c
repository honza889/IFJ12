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
 
 char *s;
 
 setValueString(SYMBOL(1,localTable,globalTable),"zkouska");
 printf("[%s]\n", s = getValueString(SYMBOL(1,localTable,globalTable)) );
 free(s);
 
 setValueString(SYMBOL(-3,localTable,globalTable),"zkouska2");
 printf("[%s]\n", s = getValueString(SYMBOL(-3,localTable,globalTable)) );
 free(s);
 
 freeValueTable(globalTable,&global);
 freeValueTable(localTable,&local);
 
 return 0;
}

