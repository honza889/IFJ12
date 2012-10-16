#ifndef SEMANTICS_H
#define SEMANTICS_H

/**
 * Sémantická analýza
 */

#include <stdio.h>
#include "ast.h"

/**
 * Získává od syntax() tokeny a sestaví z nich AST
 * @return AST tabulka
 */
Function semantics(FILE *f);

#endif
