#ifndef RCSTRING_H
#define RCSTRING_H

#include <stdbool.h>

/**
 * Implementace stringů s počítáním referencí.
 * Pár pravidel pro používání:
 * 	  -	Funkce, která má jako parametr RCString musí na začátku zavolat
 * 		copyRCString a na konci deleteRCString (ekivalent volání
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

RCString makeEmptyRCString();
RCString makeRCString( const char* str );
RCString copyRCString( RCString* str );
void deleteRCString( RCString* str );

void ensureUniqueRCString( RCString* str );
const char* RCStringGetBuffer( const RCString* str );

// Inspekční funkce

static inline bool RCStringEmpty( const RCString* str )
{
	return str->length == 0;
}

static inline bool RCStringUnique( const RCString* str )
{
	return str->buffer->references == 1;
}

static inline int RCStringLength( const RCString* str )
{
	return str->length;
}

char RCStringGet( const RCString* str, int index );

// Mutátory

void RCStringResize( RCString* str, int capacity );
void RCStringSet( RCString* str, int index, char c );
void RCStringAppendChar( RCString* str, char c );
void RCStringSubstring( RCString* str, int start, int length );
#endif
