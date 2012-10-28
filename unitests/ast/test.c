#include "../test.h"

#include <stdlib.h>
#include "ast.h"
#include "bif.h"
#include "value.h"


BEGIN_TEST

{
    Statement testStatements[] = {
        {
            .type = ASSIGNMENT,
            .value.assignment = {
                .destination = 0,
                .source = {
                    .type = CONSTANT,
                    .value.constant = newValueNumeric( 5.0 )
                }
            }
        },
        {
            .type = RETURN,
            .value.ret = {
                .type = VARIABLE,
                .value.variable = 0
            }
        }
    };
    
    StatementList sl = { testStatements, 2 };

    Function f = {
        .type = USER_DEFINED,
        .value = {
            .userDefined = {
                .statements = sl,
                .variableCount = 1
            }
        },
        .paramCount = 0
    };
    
    Value glob[1] = { {typeUndefined} };
    Value loc[1] = { {typeUndefined} };
    
    Context ctx = { glob, loc };
    
    Value ret = evalFunction( &f, (ExpressionList){NULL,0}, &ctx );
    
    RCString s = getValueString( &ret );
    STRTEST( s, "5" );
    deleteRCString( &s ); 
}
{
    
    // Test smycek, k retezcove promenne s obsahem "neco" pridame ve
    // smycce desetkrat retezec "LOL"
    
    Expression* expr1 = malloc( sizeof( Expression ) );
    Expression* expr2 = malloc( sizeof( Expression ) );
    Expression* expr3 = malloc( sizeof( Expression ) );
    Expression* expr4 = malloc( sizeof( Expression ) );
    Expression* expr5 = malloc( sizeof( Expression ) );
    Expression* expr6 = malloc( sizeof( Expression ) );
    
    
    *expr1 = (Expression){
        .type = VARIABLE,
        .value.variable = 0
    };
    
    *expr2 = (Expression){
        .type = CONSTANT,
        .value.constant = newValueNumeric( 10.0 )
    };
        
    *expr3 = (Expression){
        .type = VARIABLE,
        .value.variable = 1
    };
    
    *expr4 = (Expression){
        .type = CONSTANT,
        .value.constant = newValueCString( "LOL" )
    };
    
    *expr5 = (Expression){
        .type = VARIABLE,
        .value.variable = 0
    };
    
    *expr6 = (Expression){
        .type = CONSTANT,
        .value.constant = newValueNumeric( 1.0 )
    };
    
    
    Statement* whileStmt = malloc( sizeof( Statement ) * 2 );
    whileStmt[0] = (Statement){
        .type = ASSIGNMENT,
        .value.assignment = {
            .destination = 1,
            .source = {
                .type = OPERATOR,
                .value.operator = {
                    .type = BINARYOP,
                    .value.binary = {
                        .left = expr3,
                        .right = expr4,
                        .type = ADD
                    }
                }
            }
        }
    };
    
    whileStmt[1] = (Statement){
        .type = ASSIGNMENT,
        .value.assignment = {
            .destination = 0,
            .source = {
                .type = OPERATOR,
                .value.operator = {
                    .type = BINARYOP,
                    .value.binary = {
                        .left = expr5,
                        .right = expr6,
                        .type = ADD
                    }
                }
            }
        }
    };
    
    Statement* testStatements = malloc( sizeof(Statement) * 4 );
    testStatements[0] = (Statement){
        .type = ASSIGNMENT,
        .value.assignment = {
            .destination = 0,
            .source = {
                .type = CONSTANT,
                .value.constant = newValueNumeric( 0.0 )
            }
        }
    };
    testStatements[1] = (Statement){
        .type = ASSIGNMENT,
        .value.assignment = {
            .destination = 1,
            .source = {
                .type = CONSTANT,
                .value.constant = newValueCString( "neco" )
            }
        }
    };
    testStatements[2] = (Statement){
        .type = LOOP,
        .value.loop = {
            .condition = {
                .type = OPERATOR,
                .value.operator = {
                    .type = BINARYOP,
                    .value.binary = {
                        .left = expr1,
                        .right = expr2,
                        .type = LESS
                    }
                }
            },
            .statements = {
                .count = 2,
                .item = whileStmt
            }
        }
    };
        
    testStatements[3] = (Statement){
        .type = RETURN,
        .value.ret = {
            .type = VARIABLE,
            .value.variable = 1
        }
    };
    
    Value* glob = NULL;   
    Value* loc = NULL;
    
    Context ctx = { glob, loc };
    
    Function f = {
        .type = USER_DEFINED,
        .value = {
            .userDefined = {
                .statements = {
                    .item = testStatements,
                    .count = 4
                },
                .variableCount = 2
            }
        },
        .paramCount = 0
    };
    
    Value ret = evalFunction( &f, (ExpressionList){NULL,0}, &ctx );
    RCString retStr = getValueString( &ret );
    STRTEST( retStr, "necoLOLLOLLOLLOLLOLLOLLOLLOLLOLLOL" );
    deleteStatementList( f.value.userDefined.statements );
}

END_TEST
