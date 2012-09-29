/**
 * Tabulka symbolu (vyuzivajici vyhledavaci strom)
 */

#include "global.h"

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

void setSymbol(char *name, GenType *value);
GenType* getSymbol(char *name);

