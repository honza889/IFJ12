#ifndef RCSTRING_H
#define RCSTRING_H

#include <stdbool.h>
#include <stdio.h>

/**
 * Implementace stringů s počítáním referencí.
 * Pár pravidel pro používání:
 * 	  -	Funkce, která má jako parametr RCString by měla na začátku
 * 		zavolat copyRCString a na konci deleteRCString (ekivalent volání
 * 		kopírovacího konstruktoru a destruktoru v C++)
 * 	  - Funkce, která má jako parametr pointer na RCString nemusí dělat
 * 		nic
 * 	  - Pokud voláme funkci, která vrací RCString, tak si jej musíme 
 * 		někam uložit (a následně někdy smazat pomocí deleteRCString)
 * 		a nebo smazat okamžitě (pokud jej hodláme ignorovat)
 */

typedef struct SRCStringBuffer RCStringBuffer;
typedef struct SRCString RCString;

struct SRCString{
  RCStringBuffer* buffer;
  int offset; //kvuli jednoduchemu substringovani
  int length; //kvuli jednoduchemu zjisteni delky
};

struct SRCStringBuffer{
  int references;
  int capacity;
  char* string;
};
/** 
 * Vraci prazdny RCString 
 */
RCString makeEmptyRCString();

/**
 * Vytvori RCString, jehoz obsah je kopii \a str
 */
RCString makeRCString( const char* str );

/** 
 * "Kopirovaci konstruktor" RCStringu, tuto funkci
 * je nutne volat kdykoliv prirazujeme RCString do jineho.
 * 
 * Kazde volani teto funkce musi byt doprovazeno pozdejsim volanim
 * funkce deleteRCString.
 * 
 * Priklad:
 * \code
 *   void funkce( RCString str )
 *   {
 *     str = copyRCString( &str );
 *     ...prace se str...
 *     deleteRCString( &str );
 *   }
 * \endcode
 * Pripadne:
 * \code
 *   void funkce( RCString* str )
 *   {
 *     RCString str2 = copyRCString( str );
 *     ...prace se str2...
 *     deleteRCString( &str2 );
 *   }
 * \endcode
 */
RCString copyRCString( RCString* str );

/**
 * "Destruktor" RCStringu. Veškeré RCStringy je nutné finalizovat
 * touto funkcí.
 * \see copyRCString
 */
void deleteRCString( RCString* str );

/**
 * Zajisti, ze RCString \a str je jedinou referenci na jeho buffer.
 */
void ensureUniqueRCString( RCString* str );

/**
 * Umoznuje prime cteni z vnitrniho bufferu RCStringu \a str
 */
const char* RCStringGetBuffer( const RCString* str );

// Inspekční funkce

/**
 * Zjistuje prazdnost RCStringu \a str
 */
static inline bool RCStringEmpty( const RCString* str )
{
	return str->length == 0;
}

/**
 * Zjistuje, zda \a str je jedina reference na jeho buffer
 */
static inline bool RCStringUnique( const RCString* str )
{
	return str->buffer->references == 1;
}

/**
 * Vraci delku retezce \a str
 */
static inline int RCStringLength( const RCString* str )
{
	return str->length;
}

/**
 * Vraci znak na pozici \a index retezce \a str
 */
char RCStringGet( const RCString* str, int index );

/**
 * Porovnava retezce jako strcmp
 */
int RCStringCmp( const RCString* a, const RCString* b );

/**
 * Co to asi tak dela?
 */
void RCStringPrint( const RCString* str, FILE* stream );

// Mutátory

/**
 * Zmeni velikost stringu \a str na \a length.
 * Pokud je \a length vetsi nez soucasna delka retezce, tak bude
 * retezec prodlouzen, ovsem nove vznikly prostor nebude nicim 
 * inicializovan.
 */
void RCStringResize( RCString* str, int length );

/**
 * Nastavi znak na pozici \a index stringu \a str na \a c
 */
void RCStringSet( RCString* str, int index, char c );

/**
 * Prida znak \a c na konec stringu \a str
 */
void RCStringAppendChar( RCString* str, char c );

/**
 * Prida retezec \a str2 na konec \a str1
 */
void RCStringAppendStr( RCString* str1, const RCString* str2 );

/**
 * Posune zacatek stringu \a str na \a start a zmeni jeho delku 
 * na \a length.
 * 
 * Priklad:
 * \code
 *   RCString s = makeRCString( "Ahoj svete!" );
 *   RCStringSubstring( &s, 5, 4 );
 *   ...s je nyni "svet"...
 * \endcode
 * 
 * Pokud nova delka zpusobi, ze konec retezce by se nachazel
 * az za koncem puvodniho retezce, tak bude retezec zvetsen 
 * pomoci RCStringResize.
 */
void RCStringSubstring( RCString* str, int start, int length );

/**
 * Pokusi se o optimalizaci mista vyuziteho stringem \a str.
 * Optimalizace selze, pokud je string referencovan vice, nez jednou.
 */
void RCStringOptimize( RCString* str );

#endif
