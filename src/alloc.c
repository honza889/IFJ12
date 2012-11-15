/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include "alloc.h"
#include "exceptions.h"

void* allocSingle( size_t size, const char* name )
{
    void* ptr = malloc( size );
    if( !ptr )
    {
        throw( OutOfMemory, name );
    }
    return ptr;
}

void* reallocArray( void* ptr, size_t size, const char* name )
{
    void* newPtr = realloc( ptr, size );
    if( !newPtr )
    {
        free( ptr );
        throw( OutOfMemory, name );
    }
    return newPtr;
}
