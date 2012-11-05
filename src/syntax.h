#ifndef SYNTAX_H
#define SYNTAX_H

/**
 * Syntaktická analýza
 */

#include "scanner.h"

/**
 * Získá token od scanner(), zkontroluje syntaxi a vrátí funkci semantics()
 * @return Token
 */
Token syntax(FILE *f);

Ast parseProgram( Scanner* s );

#endif
