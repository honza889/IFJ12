#ifndef SCANNER_H
#define SCANNER_H

#include "value.h"
#include "rcstring.h"
#include "exceptions.h"

typedef enum {
    tokId = 0x0001,	/** Typ tokenu: identifikátor (promenna) */
    tokLiteral = 0x0002,	/** Typ tokenu: libovolný literál 
                                (číslo, řetězec, bool, nil) */
    tokOp = 0x0008,	/** Typ tokenu: operátor (+, <, ...) */
    tokKeyW = 0x0010,	/** Typ tokenu: klíčové slovo (if) */
    tokAssign = 0x0020,	/** Typ tokenu: přiřazení (=) */
    tokLParen = 0x0040,	/** Typ tokenu: levá závorka ( */
    tokRParen = 0x0080,	/** Typ tokenu: pravá závorka ) */
    tokLBracket = 0x0100,	/** Typ tokenu: levá hranatá závorka [ */
    tokRBracket = 0x0200,	/** Typ tokenu: pravá hranatá závorka ] */
    tokColon = 0x0400,	/** Typ tokenu: dvojtečka : */
    tokComma = 0x0800,	/** Typ tokenu: čárka , */
    tokEndOfLine = 0x1000,	/** Typ tokenu: konec řádku (\n) */
    tokEndOfFile = 0x2000	/** Typ tokenu: konec souboru */
} TokenType;

typedef enum {		/** Klíčová slova */
      kElse = 0x0001, 
      kEnd = 0x0002,
      kFunction = 0x0004, 
      kIf = 0x0008, 
      kReturn = 0x0010, 
      kWhile = 0x0020, NOTKEYW = -1
} KeyWord;

typedef struct {
    TokenType type;
  union {
    RCString id;	/** Název symbolu */
    Value val;		/** Hodnota zapsaná ve zdrojáku */
    enum {		/** Unárni nebo binární operátor */
      opPlus, opMinus, opMultiple, opDivide, opPower,
      opLT, opGT, opLE, opGE, opNE, opEQ
    } op;
    KeyWord keyw;
  } data;
}  Token;

typedef struct {
    Token current;
    FILE* file;
} Scanner;

/**
 * Inicializuje objekt scanneru.
 */
void initScanner( Scanner* scanner, FILE* file );

/**
 * Vrati posledni nacteny token
 */
Token getTok( Scanner* scanner );

/**
 * Nacte dalsi token
 */
void consumeTok( Scanner* scanner );

/**
 * Zjisti, zda je aktualni token roven typu \a tok.
 * Pokud ano, tak ho zkonzumuje.
 * Pokud ne, haze vyjimku UnexpectedToken.
 */
Token expectTok( Scanner* scanner, TokenType tok );

/**
 * Jako expectTok, ale bez konzumace,
 */
Token testTok( Scanner* scanner, TokenType tok );

/**
 * Zjisti, zda je aktualni token keyword \a keyw.
 * Pokud ne, haze vyjimku UnexpectedKeyWord.
 */
void expectKeyw( Scanner* scanner, KeyWord keyw );

/**
 * ExpectKeyw bez konzumace.
 */
void testKeyw( Scanner* scanner, KeyWord keyw );

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
Token scan(FILE *f);

/**
 * Vrátí text chybové hlášky pro danou vyjímku
 * @param Vyjimka vracena scannerem
 */
void scannerErrorPrint(ScannerErrorException e);

#endif
