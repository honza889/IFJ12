#include <stdio.h>
#include "semantics.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
  FILE* f=fopen("unitests/semantics/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");

  SymbolTable globalSymbolTable = {NULL,0};
  Function mainFunc = semantics(0,f,&globalSymbolTable);

  fclose(f);
  
END_TEST
