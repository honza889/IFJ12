#include <stdio.h>
#include "semantics.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  FILE* f=fopen("unitests/semantics/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");

  printf("\nTento test leakuje, protoze Biba jeste neudelal uvolnovani AST :)\n\n");

  SymbolTable globalSymbolTable = newSymbolTable();
  Function mainFunc = semantics(0,f,&globalSymbolTable);

  TEST( mainFunc.value.userDefined.statements.count == 4 )

  TEST( mainFunc.value.userDefined.statements.item[0].type == ASSIGNMENT )
  TEST( mainFunc.value.userDefined.statements.item[0].value.assignment.source.type == CONSTANT )

  TEST( mainFunc.value.userDefined.statements.item[1].type == ASSIGNMENT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.type == OPERATOR )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.left->type == CONSTANT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->type == OPERATOR )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->value.operator.value.binary.left->type == CONSTANT )
  TEST( mainFunc.value.userDefined.statements.item[1].value.assignment.source.value.operator.value.binary.right->value.operator.value.binary.right->type == VARIABLE )

  freeSymbolTable(&globalSymbolTable);
  fclose(f);

  printf("\nVysledek interpretace:\n");
  Value* glob = NULL;
  Value* loc = NULL;
    
  Context ctx = { glob, loc };
  try
  {
    Value ret = evalFunction( &mainFunc, NULL, 0, &ctx );
  
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
