#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "rcstring.h"

typedef struct SValue Value;
typedef struct SFunction Function;

typedef int Variable; // index do pole proměnných (podle symbols.h)
typedef Value Constant;
typedef struct SBinaryOp BinaryOp;
typedef struct SUnaryOp UnaryOp;
typedef struct SOperator Operator;
typedef struct SFunctionCall FunctionCall;
typedef struct SExpression Expression;

typedef struct SAssignment Assignment;
typedef struct SSubstring Substring;
typedef struct SLoop Loop;
typedef struct SCondition Condition;
typedef Expression Return;

typedef struct SStatement Statement;

typedef struct SContext Context;

typedef struct {
	Statement* item;
	int count;
} StatementList;

typedef Expression* ExpressionList;
typedef Value* ValueList;

typedef Value( *BuiltinFunction )( ValueList, int );

Value evalFunction( Function* func, ExpressionList params, int parameterCount, Context* context );
void deleteStatementList( StatementList func );

/**
 * Hodnota promenne generickeho datoveho typu
 */
struct SValue {
 
    // Aktualni typ
    enum {
        typeUndefined=-1,
        typeNil=0,
        typeBoolean=1,
        typeNumeric=3,
        typeFunction=6,
        typeString=8
    } type;
 
    // Hodnota
    union {
        bool boolean;
        double numeric;
        Function *function;
        RCString string;
    } data;
};

struct SFunction
{
	enum{
		USER_DEFINED,
		BUILTIN
	} type;
	
	union{
		struct{
			StatementList statements;
			int variableCount;
		} userDefined;
		BuiltinFunction builtin;
	} value;
	
	int paramCount; // zaporne cislo znamena volitelny pocet argumentu
};

struct SBinaryOp
{
	enum
	{
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		/* ROW ROW FIGHT THE */ POWER,
		EQUALS,
		NOTEQUALS,
		LESS,
		GREATER,
		LEQUAL,
		GEQUAL
	} type;
	
	Expression* left;
	Expression* right;
};

struct SUnaryOp
{
	enum
	{
		MINUS
	} type;
	
	Expression* operand;
};
		

struct SOperator
{
	enum
	{
		BINARYOP,
		UNARYOP
	} type;
	
	union
	{
		BinaryOp binary;
		UnaryOp unary;
	} value;
};

struct SFunctionCall
{
	ExpressionList params;
	int parameterCount;
	Variable function;
};
	
struct SExpression
{
	enum
	{
		CONSTANT,
		VARIABLE,
		OPERATOR,
		FUNCTION_CALL
	} type;
	
	union
	{
		Constant constant;
		Variable variable;
		Operator operator;
		FunctionCall functionCall;
	} value;
};

struct SAssignment
{
	Variable destination;
	Expression source;
};

struct SSubstring
{
	Variable destination;
	Expression source;
	int offset;
	int length;
};

struct SLoop
{
	Expression condition;
	StatementList statements;
};

struct SCondition
{
	Expression condition;
	StatementList ifTrue;
	StatementList ifFalse;
};

struct SStatement
{
	enum
	{
		ASSIGNMENT,
		SUBSTRING,
		LOOP,
		CONDITION,
		RETURN
	} type;
	
	union
	{
		Assignment assignment;
		Substring substring;
		Loop loop;
		Condition condition;
		Return ret;
	} value;
};

struct SContext
{
	Value* globals;
	Value* locals;
};

#endif
