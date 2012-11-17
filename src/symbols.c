/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <string.h>
#include "symbols.h"

/**
 * Vyhledani uzlu (uzel=root/greater/lesser)
 * Vstupem ukazatel na ukazatel na vrchol stromu (Symbol**)
 * Vystupem ukazatel na uzel pro symbol (Symbol**)
 */
Symbol** searchSymbol(Symbol **destination, RCString name, bool *exist){
 while(*destination!=NULL){
  int cmp=RCStringCmp(&(*destination)->name,&name);
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
 * Vytvoreni uzlu v zadane tabulce
 */
bool setNewSymbol( RCString name, SymbolTable *table )
{
 bool exist=false;
 Symbol **destination; // ukazatel na promennou v budoucim rodici
 
 // Prohledame zda uz v tabulce je
 destination = searchSymbol(&(table->root),name,&exist);
 
 // Vytvoreni symbolu, pokud nejde o prepis exitujiciho
 if(!exist){
  // destination nyni ukazuje na NULL ukazatel (volny uzel)
  
  Symbol *newSymbol = malloc(sizeof(Symbol));
  MALLCHECK(newSymbol);
  
  // Kopie nazvu promenne
  newSymbol->name=copyRCString(&name);
  
  newSymbol->lesser = NULL;
  newSymbol->greater = NULL;
  newSymbol->index = table->count++;
  *destination = newSymbol;
  
 }
 return !exist; // true pokud byl vytvoren, false pokud uz existoval
}


/**
 * Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho
 * @return index (zaporny=globalTable, kladny=localTable)
 */
int getSymbol(RCString name, SymbolTable *globalTable, SymbolTable *localTable){
 
 bool inLocal=false;
 bool exist=false;
 int index;
 Symbol **destination; // ukazatel na promennou v budoucim rodici
 
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
  newSymbol->name=copyRCString(&name);		// DE-BUG: 1. kopie
  
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

/**
 * Uvolni symbol (vcetne obsahu)
 */
void freeSymbol(Symbol *s){
 deleteRCString(&s->name);
 if(s->lesser!=NULL) freeSymbol(s->lesser);
 if(s->greater!=NULL) freeSymbol(s->greater);
 free(s);
}

/**
 * Uvolni strom symbolu
 */
void freeSymbolTable(SymbolTable *st){
 if(st->root!=NULL) freeSymbol(st->root);
}

