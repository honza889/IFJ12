#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "ast.h"

int main(int argc, char**argv)
{
    
    /****************************** Inicializace ******************************/
    
    exceptions_init();
    if(argc!=2){
        fprintf( stderr, "Chybný způsob volání interpretru!\nPoužití: %s program.ifj\n\n", argv[0] );
        exit( 13 );
    }
    
    FILE* f=fopen(argv[1],"r");
    if(f==NULL){
        fprintf( stderr, "Program k interpretaci (%s) se nepodařilo otevřít!\n\n", argv[1] );
        exit( 13 );
    }
    
    Scanner s;
    initScanner(&s,f);
    SymbolTable globalSymbols = newSymbolTable(); // strom funkci
    SymbolTable localSymbols = newSymbolTable(); // strom lokalnich promennych mainu
    SyntaxContext syntaxcontext = {
        .globalSymbols = &globalSymbols,
        .localSymbols = &localSymbols,
        .functions = NULL
    };
    Function mainFunction;
    
    /***************************** Překlad do AST *****************************/
    
    try{
        parseProgram(&s, &syntaxcontext, &mainFunction);
    }
    catch{
        on(ScannerError, e){
            scannerErrorPrint(*e);
            fclose( f );
            exit( 1 );
        }
        on(SyntaxError, e){
            syntaxErrorPrint(*e);
            fclose( f );
            exit( 2 );
        }
        on(UnexpectedToken, e){
            UnexpectedTokenPrint(*e);
            fclose( f );
            exit( 2 );
        }
        on(UnexpectedKeyWord, e){
            UnexpectedKeyWordPrint(*e);
            fclose( f );
            exit( 2 );
        }
        on(OutOfMemory, typename){
            fprintf( stderr, "Nezdařila se alokace paměti pro typ '%s'", *typename );
            fclose( f );
            exit( 99 );
        }
        onAll{
            fprintf( stderr, "Nastala neočekávaná vyjímka v průběhu syntaktické analýzy programu!\n" );
            exit( 2 );
        }
    }
    
    /*************************** Uklid po prekladu ****************************/
    
    fclose( f );
    freeSymbolTable( &globalSymbols ); // uvolni obsah, count zustane
    freeSymbolTable( &localSymbols );
    
    /****************************** Interpretace ******************************/
    
    Context context = {
        .globals=initValueTable( globalSymbols.count ),
        .locals=initValueTable( localSymbols.count )
    };
    
    try{
        // Spusteni hlavni funkce programu (bez parametru)
        Value ret = evalFunction( &mainFunction, (ExpressionList){NULL,0}, &context );
        freeValue( &ret );
    }
    catch{
        on( UndefinedVariable, e ){
            fprintf( stderr, "Byla použita proměnná bez definování její hodnoty!\n" );
        }
        on( BadArgumentType, e ){
            fprintf( stderr, "Funkce %s byla zavolána s parametrem chybného typu!\n", *e );
        }
        on(OutOfMemory, typename){
            fprintf( stderr, "Nezdařila se alokace paměti pro typ '%s'", *typename );
            fclose( f );
            exit( 99 );
        }
        onAll{
            fprintf( stderr, "Nastala neočekávaná vyjímka v průběhu vykonávání programu!\n" );
            exit( 2 );
        }
    }
    
    /********************************* Uklid **********************************/
    
    fflush(NULL); // aby vystup programu nemusel cekat na dokonceni uklidu
    
    // Uvolneni vsech funkci
    deleteFunction( mainFunction );
    for(int i=0;i<globalSymbols.count;i++){
        deleteFunction( *context.globals[i].data.function );
    }
    
    // Uvolneni hlavnich tabulek symbolu
    freeValueTable( context.globals, globalSymbols.count );
    freeValueTable( context.locals, localSymbols.count );
    
    return 0;
}

