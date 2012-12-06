/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef SEMANTICS_H
#define SEMANTICS_H


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
    TYPE_UNDEFINED = 0x0,
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


typedef enum    //necht existuji semanticke unarni operace
{
    UNARYOP_NOT,
    UNARYOP_MINUS,
    UNARYOP_MAXVALUE   //nepouzivat, uzcuje pouze velikost
} SemanticUnaryOperator;


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

void validateReturn(Return* ret, SemCtx* ctx );


SemanticType validateExpression( Expression* expr, SemCtx* ctx );

SemanticType validateVariable( Variable* var, SemCtx* ctx );

SemanticType validateOperator( Operator* op, SemCtx* ctx );

SemanticType getTypeOfBinaryOperator( SemanticBinaryOperator op, SemanticType l, SemanticType r );

SemanticType validateBinaryOp( BinaryOp* op, SemCtx* ctx );

SemanticType validateFunctionCall( FunctionCall* functionCall, SemCtx* ctx );

SemanticType validateConstant( Constant* constant, SemCtx* ctx );

SemanticType validateUnaryOp( UnaryOp* op, SemCtx* ctx );


#endif
