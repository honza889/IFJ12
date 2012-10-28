#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>

/** 
 * Alokuje prostor pro jeden element typu \a type a vraci pointer na 
 * tento prostor. Vyhazuje OutOfMemory, pokud alokace selze.
 * 
 * Vraceny pointer je nutne uvolnit pomoci free.
 * 
 * Priklad pouziti:
 * \code
 *   MojeSuprStruktura* neco = new( MojeSuprStruktura );
 *   ...prace s neco...
 *   free( neco );
 * \endcode
 */
#define new( type ) ( allocSingle( sizeof( type ), #type ) )

/**
 * Alokuje prostor pro pole o velikosti \a count prvku typu \a type.
 * \see new
 */
#define newArray( type, count ) ( allocSingle( sizeof( type )*(count), #type "[]" ) )

/**
 * Zmeni velikost pole \a ptr na \a count prvku typu \a type. Pri selhani
 * vyhazuje OutOfMemory.
 * 
 * Vraceny pointer je nutne uvolnit pomoci free.
 */
#define resizeArray( ptr, type, count ) ( reallocArray( ptr, sizeof( type ) * (count), #type "[]" ) )


// tyhle fce nepouzivejte
void* allocSingle( size_t size, const char* name );
void* reallocArray( void* ptr, size_t size, const char* name );

#endif
