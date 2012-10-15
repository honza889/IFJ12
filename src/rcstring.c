#include <string.h>
#include "rcstring.h"
#include "global.h"
#include "exceptions.h"


RCStringBuffer* newRCStringBuffer( int len )
{
  RCStringBuffer* buf = malloc( sizeof( RCStringBuffer ) );
  MALLCHECK(buf);
  char* string = malloc( len * sizeof( char ) );
  MALLCHECK(string);
  buf->references = 1;
  buf->capacity = len;
  buf->string = string;
  return buf;
}

RCString makeRCString( const char* str )
{
  int len = strlen(str);
  return (RCString){ newRCStringBuffer( len ), 0, len };
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

	memcpy( newBuffer->string, str->buffer->string + str->offset, len );

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
  return str->buffer->string;
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

/// Helper pro resizeRCString
static int align( int length )
{
	return ( length + 128 ) & 0xFFFFFF80;
}

void resizeRCString( RCString* str, int length )
{
	if( str->length < length )
	{
		if( RCStringUnique( str ) )
		{
			// jsme unikátní, nemusíme se starat o ostatní reference
			int alignedLength = align( length + str->offset );
			char* newData = realloc( str->buffer->string, align( alignedLength ) );
			MALLCHECK( newData );
			str->buffer->string = newData;
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
		resizeRCString( str, index + 1 );
	}
	
	ensureUniqueRCString( str );
	
	str->buffer->string[ index ] = c;
}

void RCStringAppendChar( RCString* str, char c )
{
	RCStringSet( str, str->length, c );
}

void RCStringSubstring( RCString* str, int start, int length )
{
	str->offset += start;
	str->length -= start;
	resizeRCString( str, length );
}
