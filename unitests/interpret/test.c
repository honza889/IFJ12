#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"
#include "../test.h"

BEGIN_TEST

  Scanner s;
  FILE* f=fopen("unitests/interpret/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  initScanner(&s,f);
  SymbolTable globalSymbols = newSymbolTable();
  SymbolTable localSymbols = newSymbolTable();
  SyntaxContext sctx = {
      .globalSymbols = &globalSymbols,
      .localSymbols = &localSymbols,
      .functions = NULL
  };
  Function mainFunction;

  try{
    parseProgram(&s, &sctx, &mainFunction);
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
    onAll{
      fprintf(stderr,"Vyjimka v prubehu syntakticke analyzy!\n");
      exit(2);
    }
  }

  fclose(f);

  Context ctx = {
    .globals=initValueTable(globalSymbols.count),
    .locals=initValueTable(localSymbols.count)
  };

  freeSymbolTable(&globalSymbols);
  freeSymbolTable(&localSymbols);

  try
  {

    Value ret;
    ret = evalFunction( &mainFunction, (ExpressionList){NULL,0}, &ctx );
  
    TEST( ret.type == typeNumeric )
    RCString retString = getValueString(&ret);
    RCStringPrint(&retString,stdout);
    deleteRCString(&retString);
    printf("\n\n");

    freeValue( &ret );
    deleteFunction( mainFunction );
  }
  catch
  {
    on( UndefinedVariable, e )
    {
      fprintf( stderr, "Nedefinovana hodnota promenne\n" );
    }
    on( BadArgumentType, e )
    {
      fprintf( stderr, "Chybny typ parametru funkce %s\n", *e );
    }
    onAll{
      fprintf(stderr,"Vyjimka v prubehu interpretace!\n");
      exit(2);
    }
  }

END_TEST
