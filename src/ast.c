/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include "ast.h"

#include <stdbool.h>
#include "global.h"
#include "symbols.h"
#include "value.h"

typedef struct
{
  bool returned;
  Value ret;
} ReturnData;

Value evalUnaryOp( UnaryOp* op, Context* context );
Value evalBinaryOp( BinaryOp* op, Context* context );
Value evalOperator( Operator* op, Context* context );
Value evalFunctionCall( FunctionCall* func, Context* context );
Value evalExpression( Expression* expr, Context* context );

void evalAssignment( Assignment* assgn, Context* context );
void evalSubstring( Substring* substring, Context* context );
ReturnData evalLoop( Loop* loop, Context* context );
ReturnData evalCondition( Condition* loop, Context* context );
ReturnData evalReturn( Return* ret, Context* context );
ReturnData evalStatement( Statement* statement, Context* context );
ReturnData evalStatementList( StatementList sl, Context* context );

Value evalFunction( Function* func, ExpressionList params, Context* context );

// Pozor: value uz musi byt nakopirovana ( drobna optimalizace predavani )
static inline void setVariable( Variable* var, Context* context, Value value )
{
    Value* varPtr = symbol( *var, context );
    freeValue( varPtr );
    *varPtr = value;
}

static inline Value evalConstant( Constant* constant )
{
  return copyValue( constant );
}

static inline Value evalVariable( Variable* var, Context* context )
{
  return copyValue( symbol( *var, context ) );
}


Value evalUnaryOp( UnaryOp* op, Context* context )
{
    Value ret = newValueUndefined();
    Value zero = newValueNumeric( 0.0 );
    Value number = evalExpression( op->operand, context );
    switch( op->type )
    {
        // nejlepsi switch ever
        case MINUS: ret = subtractValue( &zero, &number ); break;
    }
    return ret;
}

Value evalBinaryOp( BinaryOp* op, Context* context )
{
    Value ret = newValueUndefined();
    Value left = evalExpression( op->left, context );
    Value right = evalExpression( op->right, context );
    // GIVE ME MY POLYMORPHISM GODDAMIT
    switch( op->type )
    {
        // TODO
        case ADD: ret = addValue( &left, &right ); break;
        case SUBTRACT: ret = subtractValue( &left, &right ); break;
        case MULTIPLY: ret = multiplyValue( &left, &right ); break;
        case DIVIDE: ret = divideValue( &left, &right ); break;
        case POWER: ret = powerValue( &left, &right ); break;
        case EQUALS: ret = newValueBoolean( equalValue( &left, &right ) ); break;
        case NOTEQUALS: ret = newValueBoolean( notEqualValue( &left, &right ) ); break;
        case LESS: ret = newValueBoolean( lesserValue( &left, &right ) ); break;
        case GREATER: ret = newValueBoolean( greaterValue( &left, &right ) ); break;
        case LEQUAL: ret = newValueBoolean( lesserEqualValue( &left, &right ) ); break;
        case GEQUAL: ret = newValueBoolean( greaterEqualValue( &left, &right ) ); break;
    }
    freeValue( &left );
    freeValue( &right );
    return ret;
}

Value evalOperator( Operator* op, Context* context )
{
  switch( op->type )
  {
    case BINARYOP: return evalBinaryOp( &op->value.binary, context );
    case UNARYOP: return evalUnaryOp( &op->value.unary, context );
    default: return newValueUndefined();
  }
}

Value evalFunctionCall( FunctionCall* func, Context* context )
{
    Value funcVar = evalVariable( &func->function, context );
    Function* funcPtr = getValueFunction( &funcVar );
    freeValue( &funcVar );
    return evalFunction( funcPtr, func->params, context );
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
      return newValueUndefined();
  }
}

inline void evalAssignment( Assignment* assgn, Context* context )
{
    setVariable( &assgn->destination, context, evalExpression( &assgn->source, context ) );
}

void evalSubstring( Substring* substring, Context* context )
{
    Value val = evalExpression( &substring->source, context );
    RCString r = getValueString( testValue( &val, typeString ) );
    freeValue( &val );
    
    int offset,length;
    if(substring->offset.type==CONSTANT && substring->offset.value.constant.type==typeUndefined){
        offset = 0;
    }else{
        Value offsetValue = evalExpression(&substring->offset,context);
        offset = (int)getValueNumeric( testValuePositiveNumeric( &offsetValue ) );
        freeValue(&offsetValue);
    }
    if(substring->length.type==CONSTANT && substring->length.value.constant.type==typeUndefined){
        length = RCStringLength(&r);
    }else{
        Value lengthValue = evalExpression(&substring->length,context);
        length = (int)getValueNumeric( testValuePositiveNumeric( &lengthValue ) );
        freeValue(&lengthValue);
    }
    
    if (offset >= length || length > RCStringLength(&r) || offset < 0) {
        deleteRCString(&r);
        r = makeEmptyRCString();
    }else{
        length -= offset;
        RCStringSubstring( &r, offset, length );
    }
    
    setVariable( &substring->destination, context, newValueString( r ) );
    deleteRCString( &r );
}

ReturnData evalLoop( Loop* loop, Context* context )
{
    Value condition;
    while( condition = evalExpression( &loop->condition, context ), 
           getValueBoolean( &condition ) )
    {
        ReturnData r = evalStatementList( loop->statements, context );
        if( r.returned )
        {
            return r;
        }
    }
    return (ReturnData){ false };
}

ReturnData evalCondition( Condition* condition, Context* context )
{
    Value isTrue = evalExpression( &condition->condition, context );
    ReturnData r;
    if( getValueBoolean( &isTrue ) )
    {
        r = evalStatementList( condition->ifTrue, context );
    }
    else
    {
        r = evalStatementList( condition->ifFalse, context );
    }
    freeValue( &isTrue );
    if( r.returned )
    {
        return r;
    }
    else
    {
        return (ReturnData){ false };
    }
}

inline ReturnData evalReturn( Return* ret, Context* context )
{
  return (ReturnData){ true, evalExpression( ret, context ) };
}

ReturnData evalStatement( Statement* statement, Context* context )
{
    switch( statement->type )
    {
        case ASSIGNMENT: 
            evalAssignment( &statement->value.assignment, context ); break;
        case SUBSTRING: 
            evalSubstring( &statement->value.substring, context ); break;
        case LOOP: 
            return evalLoop( &statement->value.loop, context );
        case CONDITION: 
            return evalCondition( &statement->value.condition, context );
        case RETURN: 
            return evalReturn( &statement->value.ret, context );
    }
    return (ReturnData){false};
}

ReturnData evalStatementList( StatementList sl, Context* context )
{
    for( int i = 0; i < sl.count; i++ )
    {
        ReturnData r = evalStatement( &sl.item[ i ], context );
        if( r.returned )
        {
            return r;
        }
    }
    return (ReturnData){ false };
}

Value evalFunction( Function* func, ExpressionList params, Context* context )
{
    int variableCount;
    
    if( func->paramCount < 0 )
    {
        variableCount = params.count;
    }
    else
    {
        variableCount = func->paramCount;
        if( params.count > func->paramCount )
        {
            params.count = func->paramCount;
        }
    }
    
    if( func->type == USER_DEFINED )
    {
        variableCount += func->value.userDefined.variableCount;
    }
    Value variables[ variableCount ];
    Context nestedContext = { context->globals, variables };
    for( int i = 0; i < variableCount; i++ ) // inicializace lokalni tabulky symbolu
    {
        if( i < params.count ) // predane parametry
        {
            variables[ i ] = evalExpression( &params.expressions[ i ], context );
        }
        else if( i < func->paramCount ) // nepredane parametry
        {
            variables[i]=(Value){.type=typeNil};
        }
        else // lokalni promenne
        {
            variables[i]=(Value){.type=typeUndefined};
        }
    }
    
    if( func->type == BUILTIN )
    {
        Value ret = func->value.builtin( variables, params.count );
        
        for( int i = 0; i < variableCount; i++ )
        {
            freeValue( variables + i );
        }
        
        return ret;
    }
    else
    {
        ReturnData r = evalStatementList( func->value.userDefined.statements, &nestedContext );
        
        for( int i = 0; i < variableCount; i++ )
        {
            freeValue( variables + i );
        }
        
        if( r.returned )
        {
            return r.ret;
        }
    }
    
    return newValueUndefined();
}



void deleteExpression( Expression* expr );

void deleteConstant( Constant* constant )
{
    freeValue( constant );
}

void deleteVariable( Variable* var )
{
}

void deleteUnaryOp( UnaryOp* op )
{
    deleteExpression( op->operand );
    free( op->operand );
}

void deleteBinaryOp( BinaryOp* op )
{
    deleteExpression( op->left );
    deleteExpression( op->right );
    free( op->left );
    free( op->right );
}

void deleteOperator( Operator* op )
{
    switch( op->type )
    {
        case BINARYOP: deleteBinaryOp( &op->value.binary ); break;
        case UNARYOP: deleteUnaryOp( &op->value.unary ); break;
    }
}

void deleteFunctionCall( FunctionCall* func )
{
    for( int i = 0; i < func->params.count; i++ )
    {
        deleteExpression( &func->params.expressions[i] );
    }
    free(func->params.expressions);
    deleteVariable( &func->function );
}

void deleteExpression( Expression* expr )
{
    switch( expr->type )
    {
        case CONSTANT: 
            deleteConstant( &expr->value.constant ); break;
        case VARIABLE:
            deleteVariable( &expr->value.variable ); break;
        case OPERATOR:
            deleteOperator( &expr->value.operator ); break;
        case FUNCTION_CALL:
            deleteFunctionCall( &expr->value.functionCall ); break;
    }
}

static inline void deleteAssignment( Assignment* assgn )
{
    deleteExpression( &assgn->source );
    deleteVariable( &assgn->destination );
}

static inline void deleteSubstring( Substring* substring )
{
    deleteExpression( &substring->offset );
    deleteExpression( &substring->length );
    deleteExpression( &substring->source );
    deleteVariable( &substring->destination );
}

static inline void deleteLoop( Loop* loop )
{
    deleteExpression( &loop->condition );
    deleteStatementList( loop->statements );
}

static inline void deleteCondition( Condition* condition )
{
    deleteExpression( &condition->condition );
    deleteStatementList( condition->ifTrue );
    deleteStatementList( condition->ifFalse );
}

static inline void deleteReturn( Return* ret )
{
    deleteExpression( ret );
}

void deleteStatementList( StatementList sl )
{
    for( int i = 0; i < sl.count; i++ )
    {
        switch( sl.item[i].type )
        {
            case ASSIGNMENT: 
                deleteAssignment( &sl.item[i].value.assignment ); break;
            case SUBSTRING: 
                deleteSubstring( &sl.item[i].value.substring ); break;
            case LOOP: 
                deleteLoop( &sl.item[i].value.loop ); break;
            case CONDITION: 
                deleteCondition( &sl.item[i].value.condition ); break;
            case RETURN: 
                deleteReturn( &sl.item[i].value.ret ); break;
        }
    }
    free( sl.item );
}
