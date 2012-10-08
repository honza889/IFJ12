#ifndef AST_H
#define AST_H

#include "value.h"

typedef struct SExpression Expression;
typedef struct SStatement Statement;
typedef Statement* StatementList;
typedef int Variable; // index do pole proměnných (podle symbols.h)
typedef Expression* ExpressionList;
typedef Value* ValueList;
typedef Value( *BuiltinFunction )( ValueList, int );

typedef struct SFunction
{
	enum{
		USER_DEFINED,
		BUILTIN
	} type;
	
	union{
		struct{
			StatementList statements;
			int statementCount;
			int variableCount;
		} userDefined;
		BuiltinFunction builtin;
	} value;
	
	int paramCount; // zaporne cislo znamena volitelny pocet argumentu
} Function;

typedef Value Constant;

typedef struct
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
} BinaryOp;

typedef struct
{
	enum
	{
		MINUS
	} type;
	
	Expression* operand;
} UnaryOp;
		

typedef struct
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
} Operator;

typedef struct
{
	ExpressionList params;
	int parameterCount;
	Variable function;
} FunctionCall;
	
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

typedef struct
{
	Variable destination;
	Expression source;
} Assignment;

typedef struct
{
	Variable destination;
	Expression source;
	int left;
	int right;
} Substring;

typedef struct
{
	Expression condition;
	StatementList statements;
} Loop;

typedef struct
{
	Expression condition;
	StatementList ifTrue;
	StatementList ifFalse;
} Condition;

typedef Expression Return;

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

typedef struct
{
	Value* globals;
	Value* locals;
} Context;

Value evalFunction( Function* func, ExpressionList params, int parameterCount, Context* context );

#endif
