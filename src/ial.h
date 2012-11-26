/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef IAL_H_INCLUDED
#define IAL_H_INCLUDED

#include "ast.h"
#include "value.h"
#include "rcstring.h"


/************************** VYHLEDAVANI PODRETEZCE *************************/

/**
*   author: Martin Fryc
*   date: 19.10.2012
*   proj IFJ-find
*
*   vyhledava podretezec v retezci
*   parametr ValueList data, obsahuje retezce (data[0]=retezec,
*   data[1]=hledany podretezec)
*   int count je tedy vzdy 2
*   vraci Value numeric
*   v pripade nalezeni vrati index prvku, kde zacina nalezeny podretezec
*             nenalezeni vrati delku retezce
**/
Value find(ValueList data, int count);


/********************************* RAZENI **********************************/

// Seradi znaky v retezci (vice v ial.c)
Value sort(ValueList data, int count);


/***************************** TABULKA SYMBOLU *****************************/

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

#endif // IAL_H_INCLUDED
