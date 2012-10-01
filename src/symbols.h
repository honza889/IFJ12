#ifndef SYMBOLS_H
#define SYMBOLS_H

/**
 * Tabulka symbolu (vyuzivajici vyhledavaci strom)
 */

#include <stdlib.h>
#include "symbols.h"
#include "gentype.h"

/**
 * Symbol (prvek tabulky/stromu symbolu)
 */
typedef struct Symbol Symbol; // Aby mohl obsahovat sam sebe
struct Symbol {
 char *name; // Nazev promenne
 GenType *value; // Obsah promenne
 Symbol *lesser; // Větev - menší prvek
 Symbol *greater; // Větev - větší prvek
};

extern Symbol *symbolTable;

/** Vrati adresu GT symbolu daneho jmena, pokud neexistuje, vytvori */
GenType* setSymbol(char *name);
/** Vrati adresu GT symbolu daneho jmena, pokud neexistuje, vrati NULL */
GenType* getSymbol(char *name);

#endif
