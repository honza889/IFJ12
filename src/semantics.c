/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include "semantics.h"

void validateFunction( Function* f )
{
    if( f->type == USER_DEFINED )   //vestavene funkce validovat nebudeme
    {
        SemanticType types[ f->value.userDefined.variableCount ];   //tabulka pro propojeni promene(vyrazu) a nejakeho datoveho typu (i vice zaroven)
                                                                    //index urcuje assgn->destination

        SemCtx ctx = { types, f->value.userDefined.variableCount, FULL_VALIDATION }; //pom struktura


        /* INICIALIZACE types*/
        for( int i = 0; i < f->paramCount; i++ ){  // zacatek pole vyhrazen pro parametry funkce

            ctx.types[i] = TYPE_ALL;
        }

        for( int i = f->paramCount; i < f->value.userDefined.variableCount ;i++)
        {
            ctx.types[i] = 0;
        }

        /* VYHODNOCENI statements (error == except.)*/
        for( int i = 0; i < f->value.userDefined.statements.count; i++ )
        {
            validateStatement( &f->value.userDefined.statements.item[i], &ctx );
        }
    }
}

void validateStatement( Statement* stmt, SemCtx* ctx )
{
    switch( stmt->type )
    {
        case ASSIGNMENT: validateAssignment( &stmt->value.assignment, ctx );
            break;

        case SUBSTRING: validateSubstring( &stmt->value.substring, ctx );
            break;

        case LOOP: validateLoop( &stmt->value.loop, ctx );
            break;

        case CONDITION: validateCondition( &stmt->value.condition, ctx );
            break;

        case RETURN: validateReturn( &stmt->value.ret, ctx );
            break;
        default :
            break;
    }
}




void validateAssignment( Assignment* assgn, SemCtx* ctx )
{
    if( assgn->destination < 0 ){ // je to globální proměnná

        throw( InvalidAssignment, assgn->destination );

    }else{

        if( ctx->mode == FULL_VALIDATION )
        {
            ctx->types[ assgn->destination ] = validateExpression( &assgn->source, ctx );
        }
        else
        {
            // Pri volnejsi validaci nastavime vsechny pouzite promenne
            // na "cokoliv". To je opet vhodne pro smycky
            ctx->types[ assgn->destination ] = TYPE_ALL;
        }
    }
}


void validateSubstring(Substring* substring, SemCtx* ctx ){

SemanticType result;
    if( substring->destination < 0 ){

        throw( InvalidExpression, substring->destination );

    }else{

        if( ctx->mode == FULL_VALIDATION ){

            if((result = validateExpression( &substring->source, ctx )) == TYPE_STRING){
                ctx->types[ substring->destination ] = result;
            }else{

                throw( InvalidExpression, substring->destination );

            }
        }else{

            ctx->types[ substring->destination ] |= TYPE_STRING;
        }
    }
}


void validateLoop(Loop* loop, SemCtx* ctx ){

    ctx->mode = PERMISSIVE_VALIDATION;

    for( int i = 0; i < loop->statements.count; i++ )
    {
        validateStatement( &loop->statements.item[i], ctx );
    }

    ctx->mode = FULL_VALIDATION;

}

void validateCondition(Condition* condition, SemCtx* ctx ){
    SemanticType typesIf[ctx->varCount];        //potrebujeme dve nove tabulky, pro IF a ELSE
    SemanticType typesElse[ctx->varCount];
    memcpy( typesIf, ctx->types, ctx->varCount * sizeof( SemanticType ) );  //prekopirovani
    memcpy( typesElse, ctx->types, ctx->varCount * sizeof( SemanticType ) );

    SemCtx ctxIf = { typesIf, ctx->varCount, ctx->mode };
    SemCtx ctxElse = { typesElse, ctx->varCount, ctx->mode };

    for( int i = 0; i < condition->ifTrue.count; i++ )
    {
        validateStatement( &condition->ifTrue.item[i], &ctxIf );
    }
    for( int i = 0; i < condition->ifFalse.count; i++ )
    {
        validateStatement( &condition->ifFalse.item[i], &ctxElse );
    }
    for( int i = 0; i < ctx->varCount; i++ )
    {
        ctx->types[i] = typesIf[i] | typesElse[i];  //operace OR, pokud je moznost validace aspon jedne vetve IF ELSE
    }
}

void validateReturn(Return* ret, SemCtx* ctx ){

    validateExpression( ret, ctx );

}

SemanticType validateExpression( Expression* expr, SemCtx* ctx )
{
    switch( expr->type ){

        case VARIABLE: return validateVariable( &expr->value.variable, ctx );
        case OPERATOR: return validateOperator( &expr->value.operator, ctx );
        case FUNCTION_CALL: return validateFunctionCall( &expr->value.functionCall, ctx );
        case CONSTANT: return validateConstant( &expr->value.constant, ctx );
        default:
            break;
    }
    return TYPE_UNDEFINED;    //sem by se to nikdy dostat nemelo!
}


SemanticType validateVariable( Variable* var, SemCtx* ctx )
{
    if( *var < 0 )
    {
        throw( InvalidExpression, 0 );
    }

    if( ctx->mode == FULL_VALIDATION )
    {
        return ctx->types[ *var ];
    }
    else
    {
        return TYPE_ALL;
    }
}

SemanticType validateOperator( Operator* op, SemCtx* ctx )
{
    switch( op->type )
    {
        case BINARYOP: return validateBinaryOp( &op->value.binary, ctx );
        case UNARYOP: return validateUnaryOp( &op->value.unary, ctx );
        default:
            break;
    }
    return TYPE_UNDEFINED;    //sem by se to nikdy dostat nemelo!
}


SemanticType validateFunctionCall( FunctionCall* functionCall, SemCtx* ctx ){

    if( functionCall->function >= 0 ){

        throw( InvalidExpression, functionCall->function );

    }else{
        for( int i = 0; i < functionCall->params.count; i++ ){

            validateExpression( &functionCall->params.expressions[i], ctx );
        }
    }
    return TYPE_ALL;
}

SemanticType validateConstant( Constant* constant, SemCtx* ctx ){

    return ValueToSemanticType[constant->type];  //prevedeme typ na semanticType
}

SemanticType getTypeOfBinaryOperator( SemanticBinaryOperator op, SemanticType l, SemanticType r )
{
    SemanticType res = 0;
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            if( ( ( 1 << i ) & l ) && ( ( 1 << j ) & r ) )
            {
                res |= binaryOperatorTypeTable[op][i][j];
            }
        }
    }
    return res;
}


SemanticType validateBinaryOp( BinaryOp* op, SemCtx* ctx )
{
    SemanticBinaryOperator opType = astOperatorConvTable[op->type];
    SemanticType leftType = validateExpression( op->left, ctx );
    SemanticType rightType = validateExpression( op->right, ctx );
    SemanticType resType = getTypeOfBinaryOperator( opType, leftType, rightType );
    if( resType == 0 )
    {
        // Vysledek operace neni definovany, hodime chybu
        throw( InvalidExpression, 0 );
    }
    return resType;
}


SemanticType validateUnaryOp( UnaryOp* op, SemCtx* ctx )
{
    SemanticUnaryOperator opType = astUnaryOperatorConvTable[op->type];
    SemanticType unaryType = validateExpression( op->operand, ctx );
    SemanticType resType = 0;
    for( int i = 0; i < 4; i++ ){

            if( ( 1 << i ) & unaryType ){
                resType |= unaryOperatorTypeTable[opType][i];
            }
    }

    if( resType == 0 ){

        throw( InvalidExpression, 0 );
    }
    return resType;
}

