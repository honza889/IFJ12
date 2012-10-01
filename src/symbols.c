#include "symbols.h"

#include <string.h>

/** Tabulka symbolu (ukazatel na vrchol vyhledavaciho stromu) */
Symbol *symbolTable = NULL;

/** Zajisti vytvoreni/nalezeni GT a jeho adresu vrati */
GenType* setSymbol(char *name){
 
 // Vyhledani uzlu (uzel=symbolTable/greater/lesser)
 Symbol **destination = &symbolTable;
 while(*destination!=NULL){
  int cmp=strcmp((*destination)->name,name);
  
  if(cmp==0){ // destination name == name
   break; // prepis hodnoty existujiciho symbolu
  }else if(cmp<0){ // destination name < name
   destination = &((*destination)->greater);
  }else{ // destination name > name
   destination = &((*destination)->lesser);
  }
  
 }
 
 // Vytvoreni symbolu, pokud nejde o prepis exitujiciho
 if(*destination==NULL){
  Symbol *newSymbol = malloc(sizeof(Symbol));
  MALLCHECK(newSymbol);
  
  // Vytvoreni kopie nazvu promenne
  int nameSize = (strlen(name)+1)*sizeof(char);
  newSymbol->name=malloc(nameSize);
  MALLCHECK(newSymbol->name);
  memcpy(newSymbol->name,name,nameSize);

  newSymbol->value = NULL;
  newSymbol->lesser = NULL;
  newSymbol->greater = NULL;
  *destination = newSymbol;
 }
 
 // Vytvoreni/nalezeni GT
 if((*destination)->value!=NULL){
  return (*destination)->value;
 }else{
  GenType *newGT = malloc(sizeof(GenType));
  MALLCHECK(newGT);
  newGT->type = typeNil;
  newGT->value = NULL;
  (*destination)->value=newGT;
  return newGT;
 }
 
}

GenType* getSymbol(char *name){
 
 Symbol *destination = symbolTable;
 while(destination!=NULL){
  int cmp=strcmp(destination->name,name);
  
  if(cmp==0){ // destination name == name
   return destination->value; // nalezeno - HURA!
  }else if(cmp<0){ // destination name < name
   destination = destination->greater;
  }else{ // destination name > name
   destination = destination->lesser;
  }
  
 }
 
 return NULL; // nazev nebyl nalezen
}

