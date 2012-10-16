#include "rcstring.h"
#include "../test.h"

BEGIN_TEST

	{
		RCString str = makeEmptyRCString();
		deleteRCString( &str );
		TEST( str.buffer == NULL );
	}
	{
		RCString str1 = makeEmptyRCString();
		RCString str2 = copyRCString( &str1 );
		RCString str3 = copyRCString( &str1 );
		RCString str4 = copyRCString( &str2 );
		TEST( str1.buffer == str2.buffer );
		TEST( str1.buffer == str3.buffer );
		TEST( str1.buffer == str4.buffer );
		deleteRCString( &str1 );
		TEST( str1.buffer == NULL );
		TEST( str2.buffer == str3.buffer );
		TEST( str2.buffer == str4.buffer );
		deleteRCString( &str2 );
		deleteRCString( &str3 );
		TEST( str2.buffer == NULL );
		TEST( str3.buffer == NULL );
		TEST( str4.buffer != NULL );
		deleteRCString( &str4 );
		TEST( str4.buffer == NULL );
	}
	{
		RCString str = makeRCString( "abcdef" );
		TEST( RCStringGet( &str, 0 ) == 'a' );
		TEST( RCStringGet( &str, 1 ) == 'b' );
		TEST( RCStringGet( &str, 2 ) == 'c' );
		TEST( RCStringGet( &str, 3 ) == 'd' );
		TEST( RCStringGet( &str, 4 ) == 'e' );
		TEST( RCStringGet( &str, 5 ) == 'f' );
		RCStringSubstring( &str, 1, 5 );
		TEST( RCStringGet( &str, 0 ) == 'b' );
		TEST( RCStringGet( &str, 1 ) == 'c' );
		TEST( RCStringGet( &str, 2 ) == 'd' );
		TEST( RCStringGet( &str, 3 ) == 'e' );
		TEST( RCStringGet( &str, 4 ) == 'f' );
		RCStringSubstring( &str, 1, 4 );
		TEST( RCStringGet( &str, 0 ) == 'c' );
		TEST( RCStringGet( &str, 1 ) == 'd' );
		TEST( RCStringGet( &str, 2 ) == 'e' );
		TEST( RCStringGet( &str, 3 ) == 'f' );
		RCStringSubstring( &str, 3, 2 );
		TEST( RCStringGet( &str, 0 ) == 'f' );
		RCStringSubstring( &str, 0, 1024 );
		TEST( RCStringLength( &str) == 1024 );
		TEST( str.buffer->capacity >= 1024 );
		deleteRCString( &str );
	}
	{
		RCString str1 = makeEmptyRCString();
		RCStringAppendChar( &str1, 'a' );
		RCStringAppendChar( &str1, 'b' );
		TEST( RCStringGet( &str1, 0 ) == 'a' );
		TEST( RCStringGet( &str1, 1 ) == 'b' );
		
		RCString str2 = copyRCString( &str1 );
		RCStringSet( &str2, 0, 'c' );
		
		TEST( RCStringGet( &str1, 0 ) == 'a' );
		TEST( RCStringGet( &str1, 1 ) == 'b' );
		TEST( RCStringGet( &str2, 0 ) == 'c' );
		TEST( RCStringGet( &str2, 1 ) == 'b' );
		
		
		deleteRCString( &str1 );
		deleteRCString( &str2 );
	}
	{
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
	}

END_TEST
