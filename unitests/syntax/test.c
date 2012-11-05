#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  Scanner s;
  Ast ast;
  FILE* f=fopen("unitests/syntax/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  initScanner(&s,f);
  SymbolTable globalSymbols = newSymbolTable();
  //ast = parseProgram(&s); // zpusobi zacykleni
  
  //TEST(  )
  
  freeSymbolTable(&globalSymbols);
  fclose(f);
END_TEST
