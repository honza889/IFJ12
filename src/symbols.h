#ifndef SYMBOLS_H
#define SYMBOLS_H

/**
 * Tabulka symbolu (vyuzivajici vyhledavaci strom)
 * Jmenum promennych prirazuje index v polich symbolu.
 */

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "symbols.h"
#include "value.h"

/**
 * Symbol (prvek stromu symbolu)
 */
typedef struct Symbol Symbol; // Aby mohl obsahovat sam sebe
struct Symbol {
 RCString name; // Nazev promenne
 int index; // Index Value v poli symbolu (kladny=lokalni tabulka, zaporny=globalni tabulka)
 Symbol *lesser; // Větev - menší prvek
 Symbol *greater; // Větev - větší prvek
};

typedef struct {
 Symbol *root;
 int count;
} SymbolTable;

/** Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho */
int getSymbol(RCString name, SymbolTable *globalTable, SymbolTable *localTable);

/** Uvolnuje symboly stromu symbolu (pocet prvku zachova) */
void freeSymbolTable(SymbolTable *st);

static inline Value* symbol(int index,Context* context){
 return (index>=0?&(context->locals)[index]:&(context->globals)[-index-1]);
}

static inline SymbolTable newSymbolTable(){
	return (SymbolTable){ .root=NULL, .count=0 };
}

#endif
