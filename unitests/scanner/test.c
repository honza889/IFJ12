#include <stdio.h>
#include "scanner.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
  
  FILE* f=fopen("unitests/scanner/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  
  scanner(f);
  
  
  fclose(f);
  
END_TEST
