#include <stdio.h>
#include "rcstring.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
 
 RCString s1 = makeEmptyRCString();
 TEST( RCStringGet(&s1,0) == '\0' )
 TEST( RCStringLength(&s1) == 0 )
 TEST( RCStringEmpty(&s1) )
 
 RCStringAppendChar(&s1,'a');
 RCStringAppendChar(&s1,'b');
 TEST( RCStringGet(&s1,0) == 'a' )
 TEST( RCStringGet(&s1,1) == 'b' )
 TEST( RCStringGet(&s1,2) == '\0' )
 
 RCString s2 = copyRCString(&s1);
 TEST( RCStringGet(&s2,0) == 'a' )
 TEST( RCStringGet(&s2,1) == 'b' )
 TEST( RCStringGet(&s2,2) == '\0' )
 
 RCString s3 = makeRCString("test");
 TEST( RCStringGet(&s3,0) == 't' )
 TEST( RCStringGet(&s3,1) == 'e' )
 TEST( RCStringGet(&s3,2) == 's' )
 TEST( RCStringGet(&s3,3) == 't' )
 TEST( RCStringGet(&s3,4) == '\0' )
 TEST( RCStringLength(&s3) == 4 )
 
 RCStringSet(&s3,2,'X');
 TEST( RCStringGet(&s3,2) == 'X' )
 TEST( RCStringLength(&s3) == 4 )
 
 RCStringSubstring(&s3,1,2);
 TEST( RCStringGet(&s3,0) == 'e' )
 TEST( RCStringGet(&s3,1) == 'X' )
 TEST( RCStringGet(&s3,2) == '\0' )
 TEST( RCStringLength(&s3) == 2 )
 
 
 deleteRCString(&s1);
 deleteRCString(&s2);
 deleteRCString(&s3);
 
END_TEST
