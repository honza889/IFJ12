#ifndef RCSTRING_H
#define RCSTRING_H

typedef struct{
  int references;
  int capacity;
  char string[];
} RCStringBuffer;

typedef struct{
  RCStringBuffer* buffer;
  int offset; //kvuli jednoduchemu substringovani
  int length; //kvuli jednoduchemu zjisteni delky
} RCString;

RCStringBuffer* newRCStringBuffer( int len );
RCString makeRCString( const char* str );
RCString copyRCString( RCString* str );
void deleteRCString( RCString* str );
void makeUniqueRCString( RCString* str );
const char* getReadRCString( RCString* str );

#endif
