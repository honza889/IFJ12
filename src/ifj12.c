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
    SyntaxContext syntaxcontext;
    Function mainFunction;
    
    try{
    
        
        initScanner(&s,f);
        initDefaultSyntaxContext(&syntaxcontext);
        
    
        /***************************** Překlad do AST *****************************/
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
        on(MultipleFunctionDefinitions, value){
            fprintf( stderr, "Nejednou definovaná funkce \""); RCStringPrint(value, stderr); fprintf( stderr, "\"!\n");
            fclose( f );
            exit( 5 );
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
    int countOfFunctions = syntaxcontext.globalSymbols->count;
    destroyDefaultSyntaxContext( &syntaxcontext );
    
    /****************************** Interpretace ******************************/
    
    Context context = {
        .globals=syntaxcontext.functions,
        .locals=NULL
    };
    
    try{
        // Spusteni hlavni funkce programu (bez parametru)
        Value ret = evalFunction( &mainFunction, (ExpressionList){NULL,0}, &context );
        freeValue( &ret );
    }
    catch{
        on( UndefinedVariable, e ){
            fprintf( stderr, "Byla použita proměnná bez definování její hodnoty!\n" );
            exit( 3 );
        }
        on( UndefinedFunction, e ){
            fprintf( stderr, "Byla použita nedefinovaná funkce!\n" );
            exit( 4 );
        }
        on( VariableIsNotFunction, e ){
            fprintf( stderr, "Pokus použít funkci jako proměnnou!\n" );
            exit( 11 );
        }
        on( FunctionIsNotVariable, e ){
            fprintf( stderr, "Pokus použít proměnnou jako funkci!\n" );
            exit( 11 );
        }
        on( IncompatibleComparison, e ){
            fprintf( stderr, "Nekompatibilní porovnávání - rozdílné datové typy porovnávaných hodnot!\n" );
            exit( 11 );
        }
        on( BadArgumentType, e ){
            fprintf( stderr, "Funkce %s byla zavolána s parametrem chybného typu!\n", *e );
            exit( 11 );
        }
        on( IndexOutOfBounds, e ){
            fprintf( stderr, "Index mimo pole!\n" );
            exit( 13 );
        }
        on(InvalidConversion, value){
            RCString buf = getValueString(value);
            fprintf( stderr, "Nezdařil se převod hodnoty \""); RCStringPrint(&buf, stderr); fprintf( stderr, "\" na typ numeric!\n");
            exit( 12 );
        }
        on(OutOfMemory, typename){
            fprintf( stderr, "Nezdařila se alokace paměti pro typ \"%s\"!\n", *typename );
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
    freeFunctionsTable( context.globals, countOfFunctions );
    
    return 0;
}

