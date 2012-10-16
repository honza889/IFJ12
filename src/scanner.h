#ifndef SCANNER_H
#define SCANNER_H

#include "value.h"
#include "rcstring.h"

typedef struct {
  enum {
    tokId, /** Typ tokenu: identifikátor (promenna) */
    tokNum, /** Typ tokenu: číslo (1.5) */
    tokOp, /** Typ tokenu: operátor (+) */
    tokKeyW /** Typ tokenu: klíčové slovo (if) */
  } type;
  union {
    RCString id; /** Název symbolu */
    Value val; /** Hodnota zapsaná ve zdrojáku */
    Operator op; /** Unárni nebo binární operátor */
    enum { /** Klíčová slova */
      kElse, kEnd, kFunction, kIf, kReturn, kWhile
    } keyw;
  } data;
}  Token;

/**
 * Načte jeden token
 * @return Token
 */
Token scanner(FILE *f);

#endif
