#include "global.h"

Symbol *symbolTable = NULL;

void setSymbol(char *name, GenType *value){
  
 Symbol *space = malloc(sizeof(Symbol));
 MALLCHECK(space);
 NEWSTRING(space->name,name);
 space->value = value;
 space->lesser = NULL;
 space->greater = NULL;
 
 // Zde bude pozdeji vyber, kam symbol umistit, a presun/uvolneni dosavadniho obsahu
 symbolTable = space;
 
}

GenType* getSymbol(char *name){
 
 // Zde bude pozdeji vyhledani ve stromu
 return symbolTable->value;
 
}

