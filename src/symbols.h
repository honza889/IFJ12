#ifndef SYMBOLS_H
#define SYMBOLS_H

/**
 * Tabulka symbolu (vyuzivajici vyhledavaci strom)
 * Jmenum promennych prirazuje index v polich symbolu.
 */

#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "rcstring.h"

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

/**
 * Strom symbolu (pro preklad id na index do tabulky symbolu)
 */
typedef struct {
 Symbol *root;
 int count;
} SymbolTable;

/**
 * Prida symbol \a name do \a table.
 * Pokud symbol jiz existuje, vrati false.
 */
bool setNewSymbol( RCString name, SymbolTable *table );

/**
 * Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho
 */
int getSymbol(RCString name, SymbolTable *globalTable, SymbolTable *localTable);

/**
 * Uvolnuje symboly stromu symbolu (pocet prvku zachova)
 */
void freeSymbolTable(SymbolTable *st);

/**
 * Konstruktor tabulky symbolu
 */
static inline SymbolTable newSymbolTable(){
	return (SymbolTable){ .root=NULL, .count=0 };
}

#endif
