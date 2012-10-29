#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "rcstring.h"
#include "symbols.h"

typedef struct SValue Value;
typedef struct SFunction Function;
typedef struct SAst Ast;

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
    Function* functions;
    int count; /// kolik je pouzitych polozek ve \a functions
    int capacity; /// kolik je alokovano pro \a functions
} FunctionList;

typedef struct {
	Statement* item;
	int count;
} StatementList;

typedef struct {
    Expression* expressions;
    int count;
} ExpressionList;
typedef Value* ValueList;

typedef Value( *BuiltinFunction )( ValueList, int );

Value evalFunction( Function* func, ExpressionList params, Context* context );
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
    RCString name;
    SymbolTable symTable;
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

	Expression *parent;
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

struct SAst
{
    Function main;
    FunctionList functions;
};


static inline StatementList newStatementList(){
	return (StatementList){ .item=NULL, .count=0 };
}

static inline Value* symbol(int index,Context* context){
 return (index>=0?&(context->locals)[index]:&(context->globals)[-index-1]);
}
#endif
