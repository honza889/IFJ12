/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "rcstring.h"
#include "symbols.h"

typedef struct SValue Value;
typedef struct SFunction Function;
typedef int Variable;
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

typedef struct {
    Expression* expressions;
    int count;
} ExpressionList;

typedef Value* ValueList;

typedef Value( *BuiltinFunction )( ValueList, int );

typedef enum {
    typeUndefined=-1,
    typeNil=0,
    typeBoolean=1,
    typeNumeric=3,
    typeFunction=6,
    typeString=8
} ValueType;

/**
 * Hodnota promenne generickeho datoveho typu
 */
struct SValue {
 
    // Aktualni typ
    ValueType type;
 
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
            RCString* variableNames;
            int variableCount;
        } userDefined;
        BuiltinFunction builtin;
    } value;

    int paramCount; // zaporne cislo znamena volitelny pocet argumentu
};

struct SBinaryOp
{
  enum // prvni 4 bity urcuji prioritu operatoru
  {
    AND=0x11,
    OR=0x12,
    EQUALS=0x21,
    NOTEQUALS=0x22,
    LESS=0x23,
    GREATER=0x24,
    LEQUAL=0x25,
    GEQUAL=0x26,
    ADD=0x31,
    SUBTRACT=0x32,
    MULTIPLY=0x41,
    DIVIDE=0x42,
    POWER=0x51
  } type;
  
  Expression* left;
  Expression* right;
};

struct SUnaryOp
{
  enum
  {
    MINUS,
    NOT
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
  Expression offset;
  Expression length;
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

static inline StatementList newStatementList(){
  return (StatementList){ .item=NULL, .count=0 };
}

static inline Value* symbol(int index,Context* context){
 return (index>=0?&(context->locals)[index]:&(context->globals)[-index-1]);
}

Value evalFunction( Function* func, ExpressionList params, Context* context );

void deleteStatementList( StatementList sl );

static void inline deleteFunction( Function func ){
    if(func.type==USER_DEFINED){
        deleteStatementList(func.value.userDefined.statements);
        free( func.value.userDefined.variableNames );
    }
}
#endif

