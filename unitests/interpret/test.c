#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  Scanner s;
  Ast ast;
  FILE* f=fopen("unitests/interpret/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  initScanner(&s,f);
  SymbolTable globalSymbols = newSymbolTable();
  //ast = parseProgram(&s); // zpusobi zacykleni
  
  freeSymbolTable(&globalSymbols);
  fclose(f);

  printf("\nInterpretovani:\n");
  Value* glob = NULL;
  Value* loc = NULL;
    
  Context ctx = { glob, loc };
  try
  {
/*
    Value ret;
    ret = evalFunction( &mainFunc, (ExpressionList){NULL,0}, &ctx );
  
    TEST( ret.type == typeNumeric )
    RCString retString = getValueString(&ret);
    RCStringPrint(&retString,stdout);
    deleteRCString(&retString);
    printf("\n\n");

    freeValue( &ret );
    deleteStatementList( mainFunc.value.userDefined.statements );
*/
  }
  catch
  {
    on( BadArgumentType, e )
    {
      fprintf( stderr, "Chyba v %s.\n", *e );
    }
  }
END_TEST
