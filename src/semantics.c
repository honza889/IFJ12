/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

typedef enum 
{
    TYPE_NIL = 0x1,
    TYPE_BOOLEAN = 0x2,
    TYPE_NUMERIC = 0x4,
    TYPE_STRING = 0x8,
    TYPE_ALL = 0xF
} SemanticType;

typedef enum
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
    BINARYOP_MAXVALUE
} SemanticBinaryOperator;

/// Popisuje mod semanticke validace kodu
typedef enum
{
    /// Analyzator se pokusi udrzovat si typy promennych
    FULL_VALIDATION,
    /// Analyzator bude kontrolovat pouze operace nad konstantami
    /// Vhodne napr. pro smycky
    PERMISSIVE_VALIDATION
} ValidationMode;

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
    } /// a takhle pro další operátory
};
    
typedef struct 
{
    SemanticType* types;
    int varCount;
    ValidationMode mode;
} SemCtx;

void validateFunction( Function* f )
{
    if( f->type == USER_DEFINED )
    {
        SemanticType types[ f->value.userDefined.variableCount ];
        SemCtx ctx = { types, f->value.userDefined.variableCount, FULL_VALIDATION };
        for( int i = 0; ....do paramCount )
        {
            ctx->types[i] = TYPE_ALL;
        }
        for( int i = paramCount; .... do variableCount )
        {
            ctx->types[i] = 0;
        }
        for( int i = 0; i < f->value.userDefined.statements.count; i++ )
        {
            validateStatement( &f->value.userDefined.statements.item[i], ctx );
        } 
    }
}

void validateStatement( Statement* stmt, SemCtx* ctx )
{
    switch( stmt->type )
    {
        case ASSIGNMENT: validateAssignment( &stmt->value.assignment, ctx ); return;
        //dalsi case....
    }
}

void validateAssignment( Assignment* assgn, SemCtx* ctx )
{
    if( assgn->destination < 0 )
    {
        // je to globální proměnná
        throw( InvalidAssignment, assgn->destination );
    }
    else
    {
        if( ctx->mode == FULL_VALIDATION )
        {
            ctx->types[ assgn->destination ] = validateExpression( assgn->source, ctx );
        }
        else
        {
            // Pri volnejsi validaci nastavime vsechny pouzite promenne
            // na "cokoliv". To je opet vhodne pro smycky
            ctx->types[ assgn->destination ] = TYPE_ALL;
        }
    }
}

SemanticType validateExpression( Expression* expr, SemCtx* ctx )
{
    switch( expr->type )
    {
        case VARIABLE: return validateVariable( &expr->value.variable, ctx );
        case OPERATOR: return validateOperator( &expr->value.operator, ctx );
        // další casy
    }
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
    }
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
}

void validateIf( Condition* cond, SemCtx* ctx )
{
    SemanticType typesIf[ctx->varCount];
    SemanticType typesElse[ctx->varCount];
    memcpy( typesIf, ctx->types, ctx->varCount * sizeof( SemanticType ) );
    memcpy( typesElse, ctx->types, ctx->varCount * sizeof( SemanticType ) );
    
    SemCtx ctxIf = { typesIf, ctx->varCount, ctx->mode };
    SemCtx ctxElse = { typesElse, ctx->varCount, ctx->mode };
    
    for( int i = 0; i < f->ifTrue.count; i++ )
    {
        validateStatement( &cond->ifTrue.item[i], &ctxIf );
    }
    for( int i = 0; i < cond->ifFalse.count; i++ )
    {
        validateStatement( &cond->ifFalse.item[i], &ctxElse );
    }  
    for( int i = 0; i < ctx->varCount; i++ )
    {
        ctx->types[i] = typesIf[i] | typesElse[i];
    }
}
