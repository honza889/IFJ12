#include <stdio.h>
#include "value.h"
#include "symbols.h"
#include "scanner.h"
#include "exceptions.h"
#include "global.h"

int main(int argc, char**argv)
{
  exceptions_init();
  
  if(argc!=2){
    fprintf(stderr, "Chybné volání interpretru! Použití:\n%s program.fal\n\n", argv[0]);
    exit(13);
  }
  
  FILE* f=fopen(argv[1],"r");
  if(f==NULL){
    fprintf(stderr, "Interpretovaný program se nepodařilo otevřít!\n\n");
    exit(13);
  }
  
  try{
    
    
    
  }catch{
    onAll{
      fprintf(stderr, "\nInterpretace skončila neodchycenou vyjímkou!\n\n");
      exit(99);
    }
  }
  
  fclose(f);
  
  return 0;
}

