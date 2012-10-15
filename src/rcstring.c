#include <string.h>
#include "rcstring.h"
#include "global.h"

RCStringBuffer* newRCStringBuffer( int len )
{
  RCStringBuffer* buf = malloc( sizeof( RCStringBuffer ) + len * sizeof( char ) );
  MALLCHECK(buf);
  buf->references = 1;
  buf->capacity = len;
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
    free( str->buffer );
  }
  str->buffer=NULL;
  //mozna resetovat i offset a length?
}

void makeUniqueRCString( RCString* str )
{
  int len = str->length;
  RCStringBuffer* newBuffer = newRCStringBuffer( len );

  memcpy( newBuffer->string, str->buffer->string + str->offset, len );

  deleteRCString( str ); //odstranime referenci na buffer, pripadne smazeme buffer

  str->buffer = newBuffer;
  str->offset = 0;
  str->length = len;
  
}

//BACHA! nejedná se o nulou zakončený string a hlavně to neudrží referenci
const char* getReadRCString( RCString* str )
{
  return str->buffer->string;
}
