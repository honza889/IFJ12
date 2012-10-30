#include <stdio.h>
#include "semantics.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

/**
 * Zarovnání - využívá fce printTreeExpression().
 */
void padding( char ch, int n )
{
  int i;

  for ( i = 0; i < n; i++ )
    putchar ( ch );
}

/**
 * Vytiskne strom výrazu.
 * @param[in] root	Kořen stromu.
 * @param[in] level	Odsazení kořenu zleva (většinou 0).
 */
void printTreeExpression( Expression *root, int level )
{
  if ( root->type != OPERATOR ) {
    padding ( '\t', level );
    if (root->type == CONSTANT && root->value.constant.type == typeNumeric)
      printf ( "%e\n", root->value.constant.data.numeric );
    else
      puts ( "~" );
  }
  else {
    if (root->value.operator.type == BINARYOP) {
      printTreeExpression ( root->value.operator.value.binary.right, level + 1 );
      padding ( '\t', level );
      printf ( "%x\n", root->value.operator.value.binary.type );
      printTreeExpression ( root->value.operator.value.binary.left, level + 1 );
    }
    else {
      padding ( '\t', level );
      puts ( "-" );
    }
  }
}

BEGIN_TEST
  FILE* f=fopen("unitests/semantics/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");

  printf("\nTento test leakuje, protoze Biba jeste neudelal uvolnovani AST :)\n\n");

  SymbolTable globalSymbolTable = newSymbolTable();
  Function mainFunc = semantics(0,f,&globalSymbolTable);

  // compareOperators() jen zletma
  TEST( compareOperators( (Operator){ .type=BINARYOP, .value.binary={ .type=ADD } } , (Operator){ .type=BINARYOP, .value.binary=(BinaryOp){ .type=ADD } } ) == false )
  TEST( compareOperators( (Operator){ .type=BINARYOP, .value.binary={ .type=MULTIPLY } } , (Operator){ .type=BINARYOP, .value.binary=(BinaryOp){ .type=MULTIPLY } } ) == false )
  TEST( compareOperators( (Operator){ .type=BINARYOP, .value.binary={ .type=MULTIPLY } } , (Operator){ .type=BINARYOP, .value.binary=(BinaryOp){ .type=ADD } } ) == false )
  TEST( compareOperators( (Operator){ .type=BINARYOP, .value.binary={ .type=ADD } } , (Operator){ .type=BINARYOP, .value.binary=(BinaryOp){ .type=MULTIPLY } } ) == true )

  TEST( mainFunc.value.userDefined.statements.count >= 4 )

  TEST( mainFunc.value.userDefined.statements.item[0].type == ASSIGNMENT )
  TEST( mainFunc.value.userDefined.statements.item[0].value.assignment.source.type == CONSTANT )

  TEST( mainFunc.value.userDefined.statements.item[1].type == ASSIGNMENT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.type == OPERATOR )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.left->type == CONSTANT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->type == OPERATOR )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->value.operator.value.binary.left->type == CONSTANT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->value.operator.value.binary.right->type == VARIABLE )

  printTreeExpression(&mainFunc.value.userDefined.statements.item[3].value.ret, 0);
  
  freeSymbolTable(&globalSymbolTable);
  fclose(f);

  printf("\nVysledek interpretace:\n");
  Value* glob = NULL;
  Value* loc = NULL;
    
  Context ctx = { glob, loc };
  try
  {
    Value ret = evalFunction( &mainFunc, (ExpressionList){NULL,0}, &ctx );
  
    TEST( ret.type == typeNumeric )
    RCString retString = getValueString(&ret);
    RCStringPrint(&retString,stdout);
    deleteRCString(&retString);
    printf("\n\n");

    freeValue( &ret );
    deleteStatementList( mainFunc.value.userDefined.statements );
  }
  catch
  {
    on( BadArgumentType, e )
    {
      fprintf( stderr, "Chyba v %s.\n", *e );
    }
  }
END_TEST
