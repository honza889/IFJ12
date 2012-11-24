#include <stdio.h>
#include "parsexp.h"
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST
  Scanner s;
  FILE* f=fopen("unitests/parsexp/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  initScanner(&s,f);
  SymbolTable globalSymbols = newSymbolTable();
  SymbolTable localSymbols = newSymbolTable();
  SyntaxContext ctx = {
        .globalSymbols = &globalSymbols,
        .localSymbols = &localSymbols,
        .functions = NULL,
  };

  Expression result;
  try{
    parseExpression( &s, &result, &ctx );
  }
  catch{
    on(ScannerError, e){
      scannerErrorPrint(*e);
      fclose( f );
      exit(1);
    }
    on(SyntaxError, e){
      //syntaxErrorPrint(*e);
      fprintf(stderr,"Syntakticka chyba!\n");
      fclose( f );
      exit(2);
    }
    on(UnexpectedToken, e){
      UnexpectedTokenPrint(*e);
      fclose( f );
      exit(2);
    }
    on(UnexpectedKeyWord, e){
      UnexpectedKeyWordPrint(*e);
      fclose( f );
      exit(2);
    }
    on(OutOfMemory, typename){
      fprintf(stderr, "Nebylo mozne alokovat pamet pro typ '%s'", *typename );
      fclose( f );
      exit(99);
    }
  }
  
  freeSymbolTable(&globalSymbols);
  freeSymbolTable(&localSymbols);
  fclose(f);
END_TEST
