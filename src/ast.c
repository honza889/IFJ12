#include "ast.h"

#include <stdbool.h>
#include "global.h"

typedef struct
{
	bool returned;
	Value ret;
} ReturnData;

void setVariable( Variable* var, Context* context, Value value )
{
	*SYMBOL(*var,context->locals,context->globals) = value;
}

Value evalConstant( Constant* constant )
{
	return *constant;
}

Value evalVariable( Variable* var, Context* context )
{
	return *SYMBOL(*var,context->locals,context->globals);
}

Value evalUnaryOp( UnaryOp* op, Context* context )
{
	// TODO
	return (Value){ typeUndefined };
}

Value evalBinaryOp( BinaryOp* op, Context* context )
{
	// GIVE ME MY POLYMORPHISM GODDAMIT
	switch( op->type )
	{
		// TODO: všechno
		case ADD:
		case SUBTRACT:
		case MULTIPLY:
		case DIVIDE:
		case POWER:
		case EQUALS:
		case NOTEQUALS:
		case LESS:
		case GREATER:
		case LEQUAL:
		case GEQUAL:
			break;
	}
	return (Value){ typeUndefined };
}

Value evalOperator( Operator* op, Context* context )
{
	switch( op->type )
	{
		case BINARYOP: return evalBinaryOp( &op->value.binary, context );
		case UNARYOP: return evalUnaryOp( &op->value.unary, context );
		default: return (Value){typeUndefined};
	}
}

Value evalFunctionCall( FunctionCall* func, Context* context )
{
	// TODO
	return (Value){ typeUndefined };
}

Value evalExpression( Expression* expr, Context* context )
{
	switch( expr->type )
	{
		case CONSTANT: 
			return evalConstant( &expr->value.constant );
		case VARIABLE:
			return evalVariable( &expr->value.variable, context ); 
		case OPERATOR:
			return evalOperator( &expr->value.operator, context );
		case FUNCTION_CALL:
			return evalFunctionCall( &expr->value.functionCall, context );
		default:
			return (Value){typeUndefined};
	}
}

ReturnData evalAssignment( Assignment* assgn, Context* context )
{
	setVariable( &assgn->destination, context, evalExpression( &assgn->source, context ) );
	return (ReturnData){ false };
}

ReturnData evalSubstring( Substring* substring, Context* context )
{
	// TODO
	return (ReturnData){ false };
}

ReturnData evalLoop( Loop* loop, Context* context )
{
	// TODO
	return (ReturnData){ false };
}

ReturnData evalCondition( Condition* loop, Context* context )
{
	// TODO
	return (ReturnData){ false };
}

ReturnData evalReturn( Return* ret, Context* context )
{
	return (ReturnData){ true, evalExpression( ret, context ) };
}

ReturnData evalStatement( Statement* statement, Context* context )
{
	switch( statement->type )
	{
		case ASSIGNMENT: 
			return evalAssignment( &statement->value.assignment, context );
		case SUBSTRING: 
			return evalSubstring( &statement->value.substring, context );
		case LOOP: 
			return evalLoop( &statement->value.loop, context );
		case CONDITION: 
			return evalCondition( &statement->value.condition, context );
		case RETURN: 
			return evalReturn( &statement->value.ret, context );
		default:
			return (ReturnData){false};
	}
	
}

Value evalFunction( Function* func, ExpressionList params, int parameterCount, Context* context )
{
	int variableCount;
	
	if( func->paramCount < 0 )
	{
		variableCount = parameterCount;
	}
	else
	{
		variableCount = func->paramCount;
		if( parameterCount > func->paramCount )
		{
			parameterCount = func->paramCount;
		}
	}
	
	if( func->type == USER_DEFINED )
	{
		variableCount += func->value.userDefined.variableCount;
	}
	
	Value variables[ variableCount ];
	
	Context nestedContext = { context->globals, variables };
	
	for( int i = 0; i < variableCount; i++ )
	{
		if( i < parameterCount )
		{
			variables[ i ] = evalExpression( &params[ i ], context );
		}
		else if( i < func->paramCount )
		{
			variables[i] = (Value){typeNil};
		}
		else
		{
			variables[i] = (Value){typeUndefined};
		}
	}
	
	if( func->type == BUILTIN )
	{
		return func->value.builtin( variables, parameterCount );
	}
	else
	{
		for( int i = 0; i < func->value.userDefined.statementCount; i++ )
		{
			ReturnData r = evalStatement( &func->value.userDefined.statements[ i ], &nestedContext );
			if( r.returned )
			{
				return r.ret;
			}
		}
	}
	
	return (Value){ typeUndefined };
}
