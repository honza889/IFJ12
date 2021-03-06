/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

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

// ===========MĚLO BY BÝT VE 'src/value.c' ALE KŘÍŽOVÉ ZÁVISLOSTI TO NEDOVOLÍ============
void PrintValueType(ValueType type)
{
  switch (type) {
    case typeUndefined:	fprintf(stderr,"typeUndefined"); break;
    case typeNil:	fprintf(stderr,"typeNil"); break;
    case typeBoolean:	fprintf(stderr,"typeBoolean"); break;
    case typeNumeric:	fprintf(stderr,"typeNumeric"); break;
    case typeFunction:	fprintf(stderr,"typeFunction"); break;
    case typeString:	fprintf(stderr,"typeString"); break;
  }
}

/**
 * Vytiskne na stderr text chybové hlášky pro danou výjimku.
 * @param[in] e Výjimka vrácena sémantickým analyzátorem.
 */
void UnexpectedValueTypePrint(UnexpectedValueTypeException e)
{	// TODO: Udělat lepší popis chyby.
  fprintf(stderr,"Type error: Predpokladany typ hodnoty je ");
  PrintValueType(e.expected);
  fprintf(stderr,", ale ziskany typ hodnoty je ");
  PrintValueType(e.got);
  fprintf(stderr,".\n");
}
