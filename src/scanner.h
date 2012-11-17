/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "value.h"
#include "rcstring.h"
#include "exceptions.h"

#define SCAN_BUF_CAP 4

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
    kWhile = 0x0020
} KeyWord;

typedef struct {
    TokenType type;	/** Typ tokenu */
  
    union {
        RCString id;	/** Název symbolu */
        Value val;	/** Hodnota zapsaná ve zdrojáku */
        enum {		/** Unárni nebo binární operátor */
            opPlus, opMinus, opMultiple, opDivide, opPower,
            opLT, opGT, opLE, opGE, opNE, opEQ,
            opAND, opOR, opNOT
        } op;
        KeyWord keyw;	/** Klíčová slova */
    } data;
  
    unsigned line_num;	/** Číslo řádku, na kterém se token nachází (pro výjimky) */
} Token;

typedef struct {
    unsigned first;			/** Index na první prvek bufferu. */
    unsigned count;			/** Počet prvků v bufferu. */
    Token current[SCAN_BUF_CAP];	/** Buffer do kterého se načítají tokeny. */
    FILE* file;				/** Ukazatel na otevřený soubor. */
} Scanner;

/**
 * Inicializuje objekt scanneru.
 */
void initScanner( Scanner* scanner, FILE* file );

/* Následují funkce nad frontou tokenů. */
/*
 * Buffer a fronta je zde to samé pole. Funkce nad frontou berou zepředu
 * a fce nad bufferem můžou přistupovat přímo k prvkům na určitém indexu.
 * Buffer má ve struktuře Scanner proměnné pro práci s ním.
 */
/**
 * Vrátí první token ve frontě.
 */
Token getTok( Scanner* scanner );

/**
 * Zkonzumuje aktuální token a načte další.
 */
void consumeTok( Scanner* scanner );

/**
 * Zjisti, zda je aktualni token roven typu \a tok.
 * Pokud ano, tak ho zkonzumuje. \see consumeTok
 * Pokud ne, haze vyjimku UnexpectedToken.
 */
Token expectTok( Scanner* scanner, TokenType tok );

/**
 * Jako expectTok, ale bez konzumace.
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

/* Následují funkce nad bufferem. */
/**
 * Vrátí token načtený do bufferu na daném indexu
 */
Token getTokN( Scanner* scanner, unsigned index );

/**
 * Zkonzumuje \a N tokenů.
 */
void consumeTokN( Scanner* scanner, unsigned N );

/**
 * Zjisti, zda je následující token roven typu \a tok.
 * Pokud ano, tak ho zkonzumuje.
 * Pokud ne, háže výjimku UnexpectedToken.
 */
void expectNextTok( Scanner* scanner, TokenType tok );

/**
 * Jako expectTokN, ale bez konzumace.
 */
Token testTokN( Scanner* scanner, TokenType tok, unsigned index );

/******************/
/* Hlavní funkce. */
/******************/
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
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena scannerem.
 */
void scannerErrorPrint(ScannerErrorException e);

/**
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena testTok().
 */
void UnexpectedTokenPrint(UnexpectedTokenException e);

/**
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena testKeyw().
 */
void UnexpectedKeyWordPrint(UnexpectedKeyWordException e);

#endif
