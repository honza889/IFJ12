/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <string.h>
#include "rcstring.h"
#include "string.h"
#include "global.h"
#include "exceptions.h"
#include "alloc.h"


RCStringBuffer* newRCStringBuffer( int len )
{
    RCStringBuffer* buf = new( RCStringBuffer );
    try{
        char* string = newArray( char, len );
  
        buf->references = 1;
        buf->capacity = len;
        buf->string = string;
    }catch{
        onAll{ free( buf ); rethrow(); }
    }
    return buf;
}

RCString makeEmptyRCString()
{
  return makeRCString("");
}

RCString makeRCString( const char* str )
{
  int len = strlen(str);
  RCString ret = { newRCStringBuffer( len ), 0, len };
  memcpy( ret.buffer->string, str, len );
  return ret;
}

RCString copyRCString( RCString* str )
{
  str->buffer->references++;
  return *str;
}

void deleteRCString( RCString* str )
{
  str->buffer->references--;
  if( str->buffer->references == 0 )
  {
    free( str->buffer->string );
    free( str->buffer );
  }
  str->buffer=NULL;
  str->length = 0;
  //mozna resetovat i offset a length?
}

void implCopyRCString( RCString* str, int len )
{
  RCStringBuffer* newBuffer = newRCStringBuffer( len );

  memcpy( newBuffer->string, str->buffer->string + str->offset, str->length );

  deleteRCString( str ); //odstranime referenci na buffer, pripadne smazeme buffer

  str->buffer = newBuffer;
  str->offset = 0;
  str->length = len;
}

void ensureUniqueRCString( RCString* str )
{
  if( !RCStringUnique( str ) )
  {
    implCopyRCString( str, str->length );
  } 
}

//BACHA! nejedná se o nulou zakončený string a hlavně to neudrží referenci
const char* RCStringGetBuffer( const RCString* str )
{
  return str->buffer->string + str->offset;
}

char RCStringGet( const RCString* str, int index )
{
  if( index < str->length )
  {
    return str->buffer->string[ index + str->offset ];
  }
  else if( index > str->length )
  {
    throw( IndexOutOfBounds, index );
  }
  return '\0';
}

int RCStringCmp( const RCString* a, const RCString* b )
{
  int lenDiff = RCStringLength( b ) - RCStringLength( a );
  int minLen = minInt( RCStringLength( a ), RCStringLength( b ) );
  int cmp = strncmp( RCStringGetBuffer( a ), RCStringGetBuffer( b ), minLen );
  if( cmp == 0 ) return lenDiff;
  else return cmp;
}

void RCStringPrint( const RCString* str, FILE* stream )
{
  fwrite( RCStringGetBuffer( str ), sizeof(char), RCStringLength( str ), stream );
}

/// Helper pro resizeRCString
static int align( int length )
{
  return ( length + 16 ) & 0xFFFFFFF0;
}

void RCStringResize( RCString* str, int length )
{
  if( str->length < length )
  {
    if( RCStringUnique( str ) )
    {
      // jsme unikátní, nemusíme se starat o ostatní reference
      int alignedLength = align( length + str->offset );
      str->buffer->string = resizeArray( str->buffer->string, char, alignedLength );
      str->buffer->capacity = alignedLength;
    }
    else
    {
      implCopyRCString( str, align(length) ); 
    }
  }
  
  str->length = length;
}

void RCStringSet( RCString* str, int index, char c )
{
  if( index >= str->length )
  {
    RCStringResize( str, index + 1 );
  }
  
  ensureUniqueRCString( str );
  
  str->buffer->string[ index ] = c;
}

void RCStringAppendChar( RCString* str, char c )
{
  RCStringSet( str, str->length, c );
}

void RCStringAppendStr( RCString* str1, const RCString* str2 )
{
    int oldStr1Len = RCStringLength( str1 );
    RCStringResize( str1, oldStr1Len + RCStringLength( str2 ) );
    memcpy( str1->buffer->string + str1->offset + oldStr1Len, 
            str2->buffer->string + str2->offset, 
            RCStringLength( str2 ) * sizeof( char ) );
}
    

void RCStringSubstring( RCString* str, int start, int length )
{
  str->offset += start;
  str->length -= start;
  RCStringResize( str, length );
}

void RCStringOptimize( RCString* str )
{
    if( str->buffer->references == 1 && str->length != 0 )
    {
        str->buffer->string = resizeArray( str->buffer->string, char, str->offset + str->length );
    }
}
