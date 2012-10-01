#include <stdio.h>
#include "value.h"
#include "symbols.h"

int getSymbol(char *name, SymbolTable *globalTable, SymbolTable *localTable);

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
 
 return 0;
 
}

