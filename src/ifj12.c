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
 
 freeSymbolTable(&global);
 freeSymbolTable(&local);
 
 Value *globalTable = initValueTable(&global);
 Value *localTable = initValueTable(&local);
 
 char *s;
 setValueString(&localTable[1],"zkouska");
 printf("[%s]\n", s = getValueString(&localTable[1]) );
 free(s);
 
 printf("%s\n", (getValueBoolean(&localTable[1])?"true":"false") );
 
 freeValueTable(globalTable,&global);
 freeValueTable(localTable,&local);
 
 return 0;
}

