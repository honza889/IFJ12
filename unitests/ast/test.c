#include "ast.h"
#include "../test.h"

BEGIN_TEST

	Statement testStatements[] = {
		{
			.type = ASSIGNMENT,
			.value = {
				.assignment = {
					.destination = 0,
					.source = {
						.type = CONSTANT,
						.value = {
							.constant = {
								.type = typeNumeric,
								.data = {
									.numeric = 5.0
								}
							}
						}
					}
				}
			}
		},
		{
			.type = RETURN,
			.value = {
				.ret = {
					.type = VARIABLE,
					.value = {
						.variable = 0
					}
				}
			}
		}
	};

	Function f = {
		.type = USER_DEFINED,
		.value = {
			.userDefined = {
				.statements = testStatements,
				.statementCount = 2,
				.variableCount = 1
			}
		},
		.paramCount = 1
	};
	
	Value glob[1] = { {typeUndefined} };
	Value loc[1] = { {typeUndefined} };
	
	Context ctx = { glob, loc };
	
	Value ret = evalFunction( &f, NULL, 0, &ctx );
	
	char* s = getValueString( &ret );
	STRTEST( s, "5" );
	free( s ); 

END_TEST
