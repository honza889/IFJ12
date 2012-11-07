#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <setjmp.h>
#include <stdbool.h>
#include "value.h"

typedef enum
{
    /* Lexikalni analyza */
    ScannerError, // exit 1

    /* Syntakticka analyza */
    SyntaxError, // exit 2
    UnexpectedToken,
    UnexpectedKeyWord,
    MultipleFunctionDefinitions,
    
    /* Lexikalni analyza */
    UndefinedVariable, // exit 3
    IncompatibleComparison, // exit 1
    InvalidConversion, // exit 12 (chyba pretypovani na cislo)
    BadArgumentType, // exit 11 (behova chyba nekompatibility typu)
    IndexOutOfBounds, // exit 13
    
    OutOfMemory, // exit 99 (chyba alokace pameti)
} ExceptionType;

/* Parametry vyjimek - protoze musi byt jeden, bool=void */
typedef struct {
  enum {
    InvalidNumericLiteral,
    InvalidToken,
    UndefinedKeyword,
    UnterminatedComment,
    UnterminatedString,
    BadEscSequence
  } type;
  unsigned line_num;
} ScannerErrorException;

/** Vyjimka syntakticke analyzy */
typedef struct {
  enum {
    BadTokenInExpression,
    BadTokenAtBeginOfStatement,
    AssignWithoutAssignOperator,
    StrangeSyntax,
    BinaryOperatorAtBegin,
    OperatorAtTheEnd,
    TwoOperatorsNextToEachOther,
    UnterminatedParentheses,
    BadStartOfStatement,
    BlankExpression,
  } type;
  unsigned line_num;
} SyntaxErrorException;

typedef struct
{
    int expected;
    int got;
    unsigned line_num;
} UnexpectedTokenException;

typedef struct
{
    int expected;
    int got;
    unsigned line_num;
} UnexpectedKeyWordException;

typedef int UndefinedVariableException;
typedef bool IncompatibleComparisonException;
typedef Value InvalidConversionException;
typedef const char* BadArgumentTypeException;
typedef int IndexOutOfBoundsException;
typedef const char* OutOfMemoryException;
typedef RCString MultipleFunctionDefinitionsException;

#define EXCEPTION( name ) name##Exception name##ExceptionValue

typedef struct
{
    ExceptionType type;
    union
    {
        EXCEPTION( ScannerError );
    
        EXCEPTION( UndefinedVariable );
        EXCEPTION( IncompatibleComparison );
        EXCEPTION( InvalidConversion );
        EXCEPTION( BadArgumentType );
        EXCEPTION( IndexOutOfBounds );
        EXCEPTION( SyntaxError );
        EXCEPTION( UnexpectedToken );
        EXCEPTION( UnexpectedKeyWord );
        EXCEPTION( OutOfMemory );
        EXCEPTION( MultipleFunctionDefinitions );
    } value;
} Exception;

#undef EXCEPTION

void exceptions_init();
jmp_buf* exceptions_pushBuffer();
jmp_buf* exceptions_topBuffer();
void exceptions_popBuffer();
void exceptions_endCatchBlock();
void exceptions_setHandled( bool handled );
bool exceptions_isHandled();
Exception* exceptions_getCurrentException();

extern int exceptions_impl_loopHack;
void exceptions_impl_throw( Exception e );

/**
 * Definuje blok, ve kterém může nastat výjimka.
 */
#define try \
    if( !setjmp( *exceptions_pushBuffer() ) ) \
    {

/**
 * Definuje blok handlerů výjimek
 */
#define catch \
        exceptions_popBuffer(); \
    }   \
    else for \
    ( \
        exceptions_popBuffer(), exceptions_impl_loopHack = 1; \
        exceptions_impl_loopHack != 0; \
    exceptions_endCatchBlock(), exceptions_impl_loopHack = 0 \
  )

/**
 * Definuje handler výjimky expected_exception_type, pointer 
 * na hodnotu výjimky je v handleru dostupný pod jménem name. 
 * 
 * Musí se nacházev v bloku catch.
 * 
 * Použití:
 *      on( UndefinedVariable, e )
 *     {
 *          // e je pointer na UndefinedVariableException
 *       printf("%d", *e); // nebo něco takového
 *     }
 */
#define on( expected_exception_type, name) \
  if( exceptions_getCurrentException()->type == expected_exception_type ) \
  for \
  ( \
    exceptions_impl_loopHack = 1; \
    exceptions_impl_loopHack != 0; \
    exceptions_impl_loopHack = 0 \
  ) \
  for \
  ( \
    expected_exception_type##Exception* name = \
    &exceptions_getCurrentException()->value.expected_exception_type##ExceptionValue; \
    exceptions_impl_loopHack != 0; \
    exceptions_setHandled( true ), exceptions_impl_loopHack = 0 \
  )

/**
 * Definuje handler všech druhů výjimek. Je třeba jej dát jako poslední
 * v catch bloku
 */
#define onAll \
  if( !exceptions_isHandled() ) \
  for \
  ( \
    exceptions_impl_loopHack = 1; \
    exceptions_impl_loopHack != 0; \
    exceptions_setHandled( true ), exceptions_impl_loopHack = 0 \
  )

#define throw( exception_type, exception_value ) \
  exceptions_impl_throw( (Exception)\
  { \
    .type = exception_type, \
    .value.exception_type##ExceptionValue = exception_value \
  } )

void rethrow();

/**
 * Provede \a statement, pokud ten vyhodi libovolnou vyjimku, tak se 
 * provede \a handler a vyjimka se znovu vyhodi
 */
#define onAnyException( statement, handler ) \
  try \
  { \
    statement; \
  } \
  catch \
  { \
    onAll \
    { \
      handler; \
      rethrow(); \
    } \
  }

#endif
