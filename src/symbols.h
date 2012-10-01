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

#endif
