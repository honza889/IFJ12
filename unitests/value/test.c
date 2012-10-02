
#include <stdio.h>
#include <assert.h>
#include "value.h"
#include "../test.h"

BEGIN_TEST
 
 char *s;
 Value v = {typeUndefined};
 
 setValueNil(&v);
 STRTEST( s=getValueString(&v),"Nil" );free(s);
 TEST( getValueBoolean(&v) == false );
 
 setValueBoolean(&v,true);
 STRTEST( s=getValueString(&v),"true" );free(s);
 TEST( getValueBoolean(&v) == true );

 setValueBoolean(&v,false);
 STRTEST( s=getValueString(&v), "false" );free(s);
 TEST( getValueBoolean(&v) == false );

 setValueNumeric(&v,123456.789012);
 STRTEST( s=getValueString(&v), "123456.789012" ); free( s );
 TEST( getValueBoolean(&v) );
 
 setValueString(&v,"Toto je řetězec znaků");
 STRTEST( s=getValueString(&v), "Toto je řetězec znaků" );free(s);
 TEST( getValueBoolean(&v) == true );

 freeValue(&v);
 
END_TEST
