// Vestavene funkce
// Zdenek Tretter

#ifndef BIF_H
#define BIF_H

#include "ast.h"

// Nacte retezec ze stdin
// Vraci Value type=typeString, data=ukazatel RCString s nactenym retezecem
Value BIFinput(ValueList param, int count);

// Prevede retezec na cislo
// Pokud dostane nedefinovanou promenou nebo ukazatel na funkci vyvola vyjimku
// Pokud dostane Nil nebo Boolean, vyvola vyjimku (chyba 12)
// Pokud dostane Numeric vrati ho
// Pokud dostane String prevede ho na cislo a pokud je cislo korektni vrati ho,
// jinak vyvola vyjimku (chyba 12)
Value BIFnumeric(ValueList param, int count);

// Vypise co dostane
// @parametr param - pole s parametry
// @parametr count - pocet parametru v poli param
// Pokud dostane nedefinovanou promenou nebo ukazatel na funkci vyvola vyjimku
// Ostatni typy vypise podle zadani
// Vraci Value type=typeNil
Value BIFprint(ValueList param, int count);

// Vrati cislo odpovidajici typu co dostala
// Pokud dostane nedefinovanou promenou vyvola vyjimku
// Pro ostatni typy vraci Value type=typeNumeric, data=cislo odpovidajici typu
Value BIFtypeOf(ValueList param, int count);

// Vrati delku retezce
// Pokud dostane nedefinovanou promenou vyvola vyjimku
// Pro retezec vraci Value type=typeNumeric data=delka retezce
// Pro ostatni typy vraci Value type=typeNumeric, data=0.0
Value BIFlen(ValueList param, int count);

#endif
