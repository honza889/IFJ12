#include <stdio.h>
#include "semantics.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  FILE* f=fopen("unitests/semantics/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");

  printf("\nTento test leakuje, protoze je v rannem stadiu vyvoje.\n");
  printf("Jestli vas to neblokuje, nestezujte si!\n\n");

  SymbolTable globalSymbolTable = {NULL,0};
  Function mainFunc = semantics(0,f,&globalSymbolTable);
  Statement *s = mainFunc.value.userDefined.statements.item;

  TEST( mainFunc.value.userDefined.statements.count == 1 )
  TEST( mainFunc.value.userDefined.statements.item->type == ASSIGNMENT )

  TEST( s->value.assignment.source.type == OPERATOR )

  fclose(f);
  
END_TEST
