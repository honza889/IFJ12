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
