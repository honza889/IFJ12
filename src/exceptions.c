#include "exceptions.h"

#define MAX_TRY_BLOCKS 16

static jmp_buf jumpBuffers[MAX_TRY_BLOCKS];
static int jumpBuffersSize;
static Exception currentException;
static bool currentExceptionHandled;

void rethrow()
{
	exceptions_impl_throw( currentException ); 
}

void exceptions_init()
{
	jumpBuffersSize = 0;
}

jmp_buf* exceptions_pushBuffer()
{
	return &jumpBuffers[jumpBuffersSize++];
}

jmp_buf* exceptions_topBuffer()
{
	return &jumpBuffers[jumpBuffersSize-1];
}

void exceptions_popBuffer()
{
	jumpBuffersSize--;
}

Exception* exceptions_getCurrentException()
{
	return &currentException;
}

void exceptions_endCatchBlock()
{
	if( !currentExceptionHandled )
	{
		rethrow();
	}
}

void exceptions_setHandled( bool handled )
{
	currentExceptionHandled = handled;
}

bool exceptions_isHandled()
{
	return currentExceptionHandled;
}

int exceptions_impl_loopHack;

void exceptions_impl_throw( Exception e )
{
	currentException = e;
	currentExceptionHandled = false;
	longjmp( *exceptions_topBuffer(), 1 );
}
