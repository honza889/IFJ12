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
  try{
    parseProgram(&s, &ctx, &mainFunction);
    deleteFunction( mainFunction );
  }
  catch{
    on(ScannerError, e){
      scannerErrorPrint(*e);
      fclose( f );
      exit(1);
    }
    on(SyntaxError, e){
      syntaxErrorPrint(*e);
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
//   Expression ex;parseExpression(&s,&ex,&ctx);
  //TEST(  )
  
  freeSymbolTable(&globalSymbols);
  freeSymbolTable(&localSymbols);
  fclose(f);
END_TEST
