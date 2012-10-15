#include "exceptions.h"
#include "global.h"

#define MAX_TRY_BLOCKS 64

static jmp_buf jumpBuffers[MAX_TRY_BLOCKS];
static int jumpBuffersSize;
static Exception currentException;
static bool currentExceptionHandled;

inline void rethrow()
{
	exceptions_impl_throw( currentException ); 
}

inline void exceptions_init()
{
	jumpBuffersSize = 0;
}

inline jmp_buf* exceptions_pushBuffer()
{
	return &jumpBuffers[jumpBuffersSize++];
}

inline jmp_buf* exceptions_topBuffer()
{
	return ( jumpBuffersSize ? &jumpBuffers[jumpBuffersSize-1] : NULL);
}

inline void exceptions_popBuffer()
{
	jumpBuffersSize--;
}

inline Exception* exceptions_getCurrentException()
{
	return &currentException;
}

inline void exceptions_endCatchBlock()
{
	if( !currentExceptionHandled )
	{
		rethrow();
	}
}

inline void exceptions_setHandled( bool handled )
{
	currentExceptionHandled = handled;
}

inline bool exceptions_isHandled()
{
	return currentExceptionHandled;
}

int exceptions_impl_loopHack;

void exceptions_impl_throw( Exception e )
{
	currentException = e;
	currentExceptionHandled = false;
	if(exceptions_topBuffer()){
		longjmp( *exceptions_topBuffer(), 1 );
	}else{
		ERROR("Exception error: throw without try!");
		exit(99);
	}
}
