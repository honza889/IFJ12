/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef VALUE_H
#define VALUE_H


#include <stdbool.h>
#include "global.h"
#include "ast.h"
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "exceptions.h"



/// Popisuje mod semanticke validace kodu
typedef enum
{
    /// Analyzator se pokusi udrzovat si typy promennych
    FULL_VALIDATION,
    /// Analyzator bude kontrolovat pouze operace nad konstantami
    /// Vhodne napr. pro smycky
    PERMISSIVE_VALIDATION
} ValidationMode;

typedef enum    //necht existuji semanticke typy
{
    TYPE_NIL = 0x1,
    TYPE_BOOLEAN = 0x2,
    TYPE_NUMERIC = 0x4,
    TYPE_STRING = 0x8,
    TYPE_ALL = 0xF
} SemanticType;

typedef enum    //necht existuji semanticke binarni operace
{
    BINARYOP_AND,
    BINARYOP_OR,
    BINARYOP_EQUALS,
    BINARYOP_NOTEQUALS,
    BINARYOP_LESS,
    BINARYOP_GREATER,
    BINARYOP_LEQUAL,
    BINARYOP_GEQUAL,
    BINARYOP_ADD,
    BINARYOP_SUBTRACT,
    BINARYOP_MULTIPLY,
    BINARYOP_DIVIDE,
    BINARYOP_POWER,
    BINARYOP_MAXVALUE   //nepouzivat, uzcuje pouze velikost
} SemanticBinaryOperator;

    /*binarni operatory*/
const SemanticBinaryOperator astOperatorConvTable[SBINARYOP_TYPE_MAXVALUE] = {
    [AND]=BINARYOP_AND,
    [OR]=BINARYOP_OR,
    EQUALS=0x21, // atd
    NOTEQUALS=0x22,
    LESS=0x23,
    GREATER=0x24,
    LEQUAL=0x25,
    GEQUAL=0x26,
    ADD=0x31,
    SUBTRACT=0x32,
    MULTIPLY=0x41,
    DIVIDE=0x42,
    POWER=0x51,
};

/// Definuje typ vysledku aplikovani binarniho operatoru, ktery je jako
/// prvni index na typy druheho indexu a tretiho indexu (druhy je levy, treti je pravy)
const SemanticType binaryOperatorTypeTable[OPERATOR_MAXVALUE][4][4] = {
    [BINARYOP_ADD] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_STRING }
    }
};

/** HEEEEEEEEEEEEEYYYYYYYY TADY JSEM SEL SPAT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :] */

typedef struct
{
    SemanticType* types;
    int varCount;
    ValidationMode mode;
} SemCtx;


void validateFunction( Function* f );

void validateStatement( Statement* stmt, SemCtx* ctx );

void validateAssignment( Assignment* assgn, SemCtx* ctx );

void validateIf( Condition* cond, SemCtx* ctx );

void validateSubstring(Substring* substring, SemCtx* ctx );

void validateLoop(Loop* loop, SemCtx* ctx );

void validateCondition(Condition* condition, SemCtx* ctx );

void validateReturn(Return ret, SemCtx* ctx );


SemanticType validateExpression( Expression* expr, SemCtx* ctx );

SemanticType validateVariable( Variable* var, SemCtx* ctx );

SemanticType validateOperator( Operator* op, SemCtx* ctx );

SemanticType getTypeOfBinaryOperator( SemanticBinaryOperator op, SemanticType l, SemanticType r );

SemanticType validateBinaryOp( BinaryOp* op, SemCtx* ctx );

SemanticType validateFunctionCall( FunctionCall* functionCall, SemCtx* ctx );

SemanticType validateConstant( Constatn* constant, SemCtx* ctx );




#endif
