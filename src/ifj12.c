/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include "syntax.h"
#include "exceptions.h"
#include "semantics.h"
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


    fclose( f );

    int countOfFunctions = syntaxcontext.globalSymbols->count;

    Context context = {
        .globals=syntaxcontext.functions,
        .locals=NULL
    };

    /*************************** Semanticka analyza ***************************/

    try{
        validateFunction( &mainFunction, &syntaxcontext);
        for(int i=0; i<countOfFunctions; i++){
            if( context.globals[i].type==typeFunction && context.globals[i].data.function->type==USER_DEFINED ){
                validateFunction( context.globals[i].data.function, &syntaxcontext);
            }
        }
    }
    catch{
        on( InvalidExpression, e ){
            fprintf( stderr, "Nevalidní výraz nalezený v průbehu sématické analýzy! (parametr vyjímky: %d)\n", *e );
            deleteFunction( mainFunction );
            freeValueTable( context.globals, countOfFunctions );
            destroyDefaultSyntaxContext( &syntaxcontext );
            exit( 5 );
        }
        on( VariableOverridesFunction, e ){
            fprintf( stderr, "Promenna '"); RCStringPrint(e, stderr); fprintf( stderr, "' se shoduje se jmenem funkce!\n");
            deleteRCString( e );
            deleteFunction( mainFunction );
            freeValueTable( context.globals, countOfFunctions );
            destroyDefaultSyntaxContext( &syntaxcontext );
            exit( 5 );
        }
        onAll{
            fprintf( stderr, "Nastala neočekávaná vyjímka v průběhu sémantické analýzy!\n" );
            deleteFunction( mainFunction );
            freeValueTable( context.globals, countOfFunctions );
            destroyDefaultSyntaxContext( &syntaxcontext );
            exit( 99 );
        }
    }

    /****************************** Interpretace ******************************/

    int exitVal = 0;

    try{
        // Spusteni hlavni funkce programu (bez parametru)
        Value ret = evalFunction( &mainFunction, (ExpressionList){NULL,0}, &context );
        freeValue( &ret );
    }
    catch{
        on( UndefinedVariable, e ){
            fprintf( stderr, "Byla použita proměnná bez definování její hodnoty!\n" );
            exitVal = 3;
        }
        on( UndefinedFunction, e ){
            fprintf( stderr, "Byla použita nedefinovaná funkce!\n" );
            exitVal = 4;
        }
        on( UnexpectedValueType, e ){
            UnexpectedValueTypePrint(*e);
            exitVal = 5;
        }
        on( NegativeNumeric, e ){
            fprintf( stderr, "ValueError: Bylo použito záporné číslo tam kde nemělo!\n" );
            exitVal = 5;
        }
        on( DividingByZero, e ){
            fprintf( stderr, "Došlo k pokusu o dělení nulou!\n" );
            exitVal = 10;
        }
        on( VariableIsNotFunction, e ){
            fprintf( stderr, "Pokus použít funkci jako proměnnou!\n" );
            exitVal = 11;
        }
        on( FunctionIsNotVariable, e ){
            fprintf( stderr, "Pokus použít proměnnou jako funkci!\n" );
            exitVal = 11;
        }
        on( IncompatibleComparison, e ){
            fprintf( stderr, "Nekompatibilní porovnávání - rozdílné datové typy porovnávaných hodnot!\n" );
            exitVal = 11;
        }
        on( BadArgumentType, e ){
            fprintf( stderr, "Funkce %s byla zavolána s parametrem chybného typu!\n", *e );
            exitVal = 11;
        }
        on( InvalidConversion, value ){
            RCString buf = getValueString(value);
            fprintf( stderr, "Nezdařil se převod hodnoty \""); RCStringPrint(&buf, stderr); fprintf( stderr, "\" na typ numeric!\n");
            exitVal = 12;
        }
        on( IndexOutOfBounds, e ){
            fprintf( stderr, "Index mimo pole!\n" );
            exitVal = 13;
        }
        on(OutOfMemory, typename){
            fprintf( stderr, "Nezdařila se alokace paměti pro typ \"%s\"!\n", *typename );
            exitVal = 99;
        }
        onAll{
            fprintf( stderr, "Nastala neočekávaná vyjímka v průběhu vykonávání programu!\n" );
            exitVal = 99;
        }
    }

    /********************************* Uklid **********************************/

    fflush(NULL); // aby vystup programu nemusel cekat na dokonceni uklidu
    


    
    // Uvolneni vsech funkci
    deleteFunction( mainFunction );
    freeValueTable( context.globals, countOfFunctions );
    destroyDefaultSyntaxContext( &syntaxcontext );

    return exitVal;
}

