#include <stdio.h>
#include <stdbool.h>
#include "exceptions.h"

#define TEST( expr ) \
    total++; \
    if( !( expr ) ){ \
        failed++; \
        fprintf(stderr,"%s:%d: %s FAILED\n",__FILE__,__LINE__,#expr); \
    }

#define STRTEST( str1, str2 ) \
    total++; \
    { \
        RCString _test_str = (str1); \
        RCString _test_str_2 = makeRCString( str2 ); \
        if( RCStringCmp( &_test_str, &_test_str_2 ) != 0 ){ \
			failed++; \
			fprintf( stderr,"%s:%d: %s FAILED (got \"",__FILE__,__LINE__,#str1); \
			RCStringPrint( &_test_str, stderr ); \
			fprintf( stderr, "\")\n" ); \
		} \
		deleteRCString( &_test_str_2 ); \
	}

#define DBLTEST( dbl1, dbl2 ) \
    total++; \
    { \
        if( (double)dbl1 != (double)dbl2 ){ \
			failed++; \
			fprintf( stderr,"%s:%d: %s == %lf FAILED (got %lf)\n",__FILE__,__LINE__,#dbl1,dbl2,dbl1); \
		} \
	}

#define STRTEST( str1, str2 ) \
    total++; \
    { \
        RCString _test_str = (str1); \
        RCString _test_str_2 = makeRCString( str2 ); \
        if( RCStringCmp( &_test_str, &_test_str_2 ) != 0 ){ \
			failed++; \
			fprintf( stderr,"%s:%d: %s FAILED (got \"",__FILE__,__LINE__,#str1); \
			RCStringPrint( &_test_str, stderr ); \
			fprintf( stderr, "\")\n" ); \
		} \
		deleteRCString( &_test_str_2 ); \
	}

#define BEGIN_TEST \
	int main() { \
		int failed = 0; \
		int total = 0; \
		exceptions_init(); \
		int exc_status = 0; \
		try {
		
#define END_TEST \
		} catch { \
			onAll { \
				fprintf( stderr, "Abort: Unhandled exception (%d)\n", (int)exceptions_getCurrentException()->type ); \
				return -1; \
			} \
		} \
		if( failed != 0 ) \
		{ \
			fprintf(stderr,"%d out of %d tests failed\n",failed,total); \
		} \
		return failed; \
	}

#define EXCEPT_TEST( exception, expr ) \
	exc_status = 0; total++; \
	try{ expr; } \
	catch{ on( exception, e ){ exc_status = 1; } } \
	if( exc_status != 1 ) \
	{ \
		failed++; \
		fprintf( stderr,"%s:%d: %s doesnt throw %s\n",__FILE__,__LINE__,#exception,#expr); \
	}

