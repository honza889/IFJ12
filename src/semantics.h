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
    TYPE_UNDEFINED = 0x0,
    TYPE_NIL = 0x1,
    TYPE_BOOLEAN = 0x2,
    TYPE_NUMERIC = 0x4,
    TYPE_STRING = 0x8,
    TYPE_ALL = 0xF
} SemanticType;


    /*binarni operatory*/
const SemanticType ValueToSemanticType[typeString+1] = {
    [typeUndefined] = TYPE_UNDEFINED,
    [typeNil] = TYPE_NIL,
    [typeBoolean] = TYPE_BOOLEAN,
    [typeNumeric] = TYPE_NUMERIC,
    [typeString] = TYPE_STRING
}

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
    [EQUALS]=BINARYOP_EQUALS,
    [NOTEQUALS]=BINARYOP_NOTEQUALS,
    [LESS]=BINARYOP_LESS,
    [GREATER]=BINARYOP_GREATER,
    [LEQUAL]=BINARYOP_LEQUAL,
    [GEQUAL]=BINARYOP_GEQUAL,
    [ADD]=BINARYOP_ADD,
    [SUBTRACT]=BINARYOP_SUBTRACT,
    [MULTIPLY]=BINARYOP_MULTIPLY,
    [DIVIDE]=BINARYOP_DIVIDE,
    [POWER]=BINARYOP_POWER,
};

/// Definuje typ vysledku aplikovani binarniho operatoru, ktery je jako
/// prvni index na typy druheho indexu a tretiho indexu (druhy je levy, treti je pravy)
const SemanticType binaryOperatorTypeTable[BINARYOP_MAXVALUE][4][4] = {
    /*[SemanticType] = {
                                    pravy, treti index
                    TYPE_NIL, TYPE_BOOLEAN, TYPE_NUMERIC, TYPE_STRING
        TYPE_NIL    {   X,          X,           X,            X },
levy    TYPE_BOOLEAN{   X,          X,           X,            X },
druhy   TYPE_NUMERIC{   X,          X,           X,            X },
        TYPE_STRING {   X,          X,           X,            X }
    }*/

    [BINARYOP_AND] = {
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN }
    },
    [BINARYOP_OR] = {
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN }
    },
    [BINARYOP_EQUALS] = {
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN }
    },
    [BINARYOP_NOTEQUALS] = {
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN },
        { TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN, TYPE_BOOLEAN }
    },
    [BINARYOP_LESS] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_BOOLEAN, 0 },
        { 0, 0, 0, TYPE_BOOLEAN }
    },
    [BINARYOP_GREATER] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_BOOLEAN, 0 },
        { 0, 0, 0, TYPE_BOOLEAN }
    },
    [BINARYOP_LEQUAL] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_BOOLEAN, 0 },
        { 0, 0, 0, TYPE_BOOLEAN }
    },
    [BINARYOP_GEQUAL] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_BOOLEAN, 0 },
        { 0, 0, 0, TYPE_BOOLEAN }
    },
    [BINARYOP_ADD] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { TYPE_STRING, TYPE_STRING, TYPE_STRING, TYPE_STRING }
    },
    [BINARYOP_SUBTRACT] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { 0, 0, 0, 0 }
    },
    [BINARYOP_MULTIPLY] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { 0, 0, TYPE_STRING, 0 }
    },
    [BINARYOP_DIVIDE] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { 0, 0, 0, 0 }
    },
    [BINARYOP_POWER] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, TYPE_NUMERIC, 0 },
        { 0, 0, 0, 0 }
    },

};


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

SemanticType validateUnaryOp( UnaryOp* op, SemCtx* ctx );


#endif
