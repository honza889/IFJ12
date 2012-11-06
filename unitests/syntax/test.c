#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  Scanner s;
  FILE* f=fopen("unitests/syntax/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  initScanner(&s,f);
  SymbolTable globalSymbols = newSymbolTable();
  SymbolTable localSymbols = newSymbolTable();
  SyntaxContext ctx = {
        .globalSymbols = &globalSymbols,
        .localSymbols = &localSymbols,
        .functions = NULL,
  };
  Function mainFunction;
  //parseProgram(&s, &ctx, &mainFunction); // zpusobi zacykleni
  Expression ex;parseExpression(&s,&ex,&ctx);
  //TEST(  )
  
  freeSymbolTable(&globalSymbols);
  freeSymbolTable(&localSymbols);
  fclose(f);
END_TEST
