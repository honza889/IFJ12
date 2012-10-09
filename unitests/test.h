#include <stdio.h>
#include <stdbool.h>

#define TEST( expr ) \
    total++; \
    if( !( expr ) ){ \
        failed++; \
        fprintf(stderr,"%s:%d: %s FAILED\n",__FILE__,__LINE__,#expr); \
    }
    
#define STRTEST( str1, str2 ) \
    total++; \
    { \
        const char* _test_str = (str1); \
        if( strcmp( _test_str, (str2) ) != 0 ){ \
			failed++; \
			fprintf( stderr,"%s:%d: %s FAILED (got \"%s\")\n",__FILE__,__LINE__,#str1,_test_str); \
		} \
	}
#define BEGIN_TEST int main() { int failed = 0; int total = 0;
#define END_TEST \
	if( failed != 0 ) \
	{ \
		fprintf(stderr,"%d out of %d tests failed\n",failed,total); \
	} \
	return failed; }

#define EXCEPT_TEST( exception, expr ) \
	status = 0; total++; \
	try{ expr; } \
	catch{ on( exception, e ){ status = 1; } } \
	if( status != 1 ) \
	{ \
		failed++; \
		fprintf( stderr,"%s:%d: %s doesnt throw %s\n",__FILE__,__LINE__,#exception,#expr); \
	}

