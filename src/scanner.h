#ifndef SCANNER_H
#define SCANNER_H

#include "value.h"
#include "rcstring.h"

typedef struct {
  enum {
    tokId,	/** Typ tokenu: identifikátor (promenna) */
    tokString,	/** Typ tokenu: řetězec ("hello") */
    tokNum,	/** Typ tokenu: číslo (1.5) */
    tokOp,	/** Typ tokenu: operátor (+, <, ...) */
    tokKeyW,	/** Typ tokenu: klíčové slovo (if) */
    tokAssign,	/** Typ tokenu: přiřazení (=) */
    tokLParen,	/** Typ tokenu: levá závorka ( */
    tokRParen,	/** Typ tokenu: pravá závorka ) */
    tokLBracket,	/** Typ tokenu: levá hranatá závorka [ */
    tokRBracket		/** Typ tokenu: pravá hranatá závorka ] */
  } type;
  union {
    RCString id;	/** Název symbolu */
    RCString string;	/** Řetězec zapsaný ve zdrojáku */
    Value val;		/** Hodnota zapsaná ve zdrojáku */
    enum {		/** Unárni nebo binární operátor */
      opPlus, opMinus, opMultiple, opDivide, opPower,
      opLT, opGT, opLE, opGE, opNE, opEQ
    } op;
    enum {		/** Klíčová slova */
      kElse, kEnd, kFunction, kIf, kReturn, kWhile, NOTKEYW = -1
    } keyw;
  } data;
}  Token;


/**
 * Načte lexém, zpracuje a vrátí token v parametru.
 * @param[in]	f	Ukazatel na otevřený soubor.
 * @param[out]	token	Ukazatel na strukturu Token.
 * @return Vrací chybový kód.
 */
Token scanner(FILE *f);

#endif
