/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef PARSEXP_H
#define PARSEXP_H

#include "scanner.h"
#include "ast.h"

typedef struct{
    enum {
        exp,     // E
        bracket, // <
        term     // $ + - id...
    } type;
    union {
        Expression exp;
        Token term;
    } val;
} ExpItem;

typedef struct{
    ExpItem* array; // ukazatel na pole polozek zasobniku (zacatek, nikoli vrchol!)
    int count; // pocet polozek na zasobniku
    int allocated; // alokovana delka pole polozek (>=count)
    int termIndex; // index nejvrcholovejsiho terminalu (primy index do array)
} ExpStack;

#endif
