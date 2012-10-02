#include "symbols.h"

#include <string.h>

/**
 * Vyhledani uzlu (uzel=root/greater/lesser)
 * Vstupem ukazatel na ukazatel na vrchol stromu (Symbol**)
 * Vystupem ukazatel na uzel pro symbol (Symbol**)
 */
Symbol** searchSymbol(Symbol **destination, char *name, bool *exist){
 while(*destination!=NULL){
  int cmp=strcmp((*destination)->name,name);
  if(cmp==0){ // destination name == name
   *exist=true; // symbol nalezen
   break;
  }else if(cmp<0){ // destination name < name
   destination = &((*destination)->greater);
  }else{ // destination name > name
   destination = &((*destination)->lesser);
  }
 }
 return destination;
}


/**
 * Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho
 * @return index (zaporny=globalTable, kladny=localTable)
 */
int getSymbol(char *name, SymbolTable *globalTable, SymbolTable *localTable){
 
 bool inLocal=false;
 bool exist=false;
 int index;
 Symbol **destination;
 SymbolTable *table;
 
 // Nejprve bude prohledana globalni tabulka
 destination = searchSymbol(&(globalTable->root),name,&exist);
 
 // Mame-li lokalni tabulku, a symbol v minule nebyl, prohledame lokalni
 if(localTable!=NULL && !exist){
  destination = searchSymbol(&(localTable->root),name,&exist);
  inLocal=true;
 }
 
 // Vytvoreni symbolu, pokud nejde o prepis exitujiciho
 if(!exist){
  
  // destination nyni ukazuje na NULL ukazatel (volny uzel)
  
  Symbol *newSymbol = malloc(sizeof(Symbol));
  MALLCHECK(newSymbol);
  
  // Kopie nazvu promenne
  int nameSize = (strlen(name)+1)*sizeof(char);
  newSymbol->name=malloc(nameSize);
  MALLCHECK(newSymbol->name);
  memcpy(newSymbol->name,name,nameSize);
  
  // Ziskani a nasledne navyseni indexu
  index=(inLocal?localTable:globalTable)->count++;
  
  newSymbol->lesser = NULL;
  newSymbol->greater = NULL;
  newSymbol->index = index;
  *destination = newSymbol;
  
 }else{ // Jde o prepis stavajiciho
  
  // destination nyni ukazuje na ukazatel na stejnojmeny prvek
  index = (*destination)->index;
  
 }
 
 // prevod fyzicke adresy na logickou
 return (inLocal?index:-index-1);
}

Value* initValueTable(SymbolTable *st){
 Value *table = malloc(st->count*sizeof(Value));
 for(int i=0;i<st->count;i++){
  table[i].type=typeUndefined;
 }
 return table;
}

void freeSymbol(Symbol *s){
 free(s->name);
 if(s->lesser!=NULL) freeSymbol(s->lesser);
 if(s->greater!=NULL) freeSymbol(s->greater);
 free(s);
}

void freeSymbolTable(SymbolTable *st){
 if(st->root!=NULL) freeSymbol(st->root);
}

void freeValueTable(Value *table,SymbolTable *st){
 for(int i=0;i<st->count;i++){
  if(table[i].type==typeString){
   free(table[i].data.string);
  }
 }
 free(table);
}

