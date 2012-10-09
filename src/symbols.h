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
 char *name; // Nazev promenne
 int index; // Index Value v poli symbolu (kladny=lokalni tabulka, zaporny=globalni tabulka)
 Symbol *lesser; // Větev - menší prvek
 Symbol *greater; // Větev - větší prvek
};

typedef struct {
 Symbol *root;
 int count;
} SymbolTable;

/** Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho */
int getSymbol(char *name, SymbolTable *globalTable, SymbolTable *localTable);

/** Alokuje a inicializuje tabulku symbolu */
Value* initValueTable(int length);

/** Uvolnuje symboly stromu symbolu (pocet prvku zachova) */
void freeSymbolTable(SymbolTable *st);

/** Uvolnuje Value tabulky symbolu (potrebuje st->count) */
void freeValueTable(Value *table,int length);

static inline Value* symbol(int index,Context* context){
 return (index>=0?&(context->locals)[index]:&(context->globals)[-index-1]);
}

#endif
