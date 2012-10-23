#ifndef SCANNER_H
#define SCANNER_H

#include "value.h"
#include "rcstring.h"
#include "exceptions.h"

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
    tokRBracket,	/** Typ tokenu: pravá hranatá závorka ] */
    tokColon,	/** Typ tokenu: dvojtečka : */
    tokEndOfFile	/** Typ tokenu: konec souboru */
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
 * Načte lexém, zpracuje a vrátí token.
 * Může vracet tyto vyjímky:
 *    InvalidNumericLiteral	- nepovedl se převest řetězec na číslo
 *    InvalidToken		- špatný token
 *    UnterminatedComment	- neukončený blokový komentář
 *    UnterminatedString	- neukončený řetězec
 *    BadEscSequence		- špatná escape sekvence
 * @param[in]	f	Ukazatel na otevřený soubor.
 * @return Vrací token.
 */
Token scanner(FILE *f);

/**
 * Vrátí text chybové hlášky pro danou vyjímku
 * @param Vyjimka vracena scannerem
 */
void scannerErrorPrint(ScannerErrorException e);

#endif
