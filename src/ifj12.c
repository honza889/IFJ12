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
        fprintf( stderr, "Chybny zpusob volani interpretru!\nPouziti: %s program.ifj\n\n", argv[0] );
        exit( 99 );
    }
    
    FILE* f=fopen(argv[1],"r");
    if(f==NULL){
        fprintf( stderr, "Program k interpretaci (%s) se nepodarilo otevrit!\n\n", argv[1] );
        exit( 99 );
    }
    
    Scanner s;
    SyntaxContext syntaxcontext;
    Function mainFunction;
    
    int exitVal = 0;
    
    try{
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
                exitVal = 1;
                rethrow();
            }
            on(SyntaxError, e){
                syntaxErrorPrint(*e);
                fclose( f );
                exitVal = 2;
                rethrow();
            }
            on(UnexpectedToken, e){
                UnexpectedTokenPrint(*e);
                fclose( f );
                exitVal = 2;
                rethrow();
            }
            on(UnexpectedKeyWord, e){
                UnexpectedKeyWordPrint(*e);
                fclose( f );
                exitVal = 2;
                rethrow();
            }
            on(MultipleFunctionDefinitions, value){
                fprintf( stderr, "Nejednou definovana funkce \""); RCStringPrint(value, stderr); fprintf( stderr, "\"!\n");
                fclose( f );
                exitVal = 5;
                rethrow();
            }
            on(OutOfMemory, typename){
                fprintf( stderr, "Nezdarila se alokace pameti pro typ '%s'", *typename );
                fclose( f );
                exitVal = 99;
                rethrow();
            }
            on( RedefinedParameter, name){
                fprintf( stderr, "Opakovana definice parametru '" );
                RCStringPrint( name, stderr );
                deleteRCString( name );
                fprintf( stderr, "'\n" );
                fclose( f );
                exitVal = 99;
                rethrow();
            }
            onAll{
                fprintf( stderr, "Nastala neocekavana vyjimka v prubehu syntakticke analyzy programu!\n" );
                exitVal = 2;
                rethrow();
            }
        }
    }
    catch{
        onAll{
            deleteFunction( mainFunction );
            freeValueTable( syntaxcontext.functions, syntaxcontext.globalSymbols->count );
            destroyDefaultSyntaxContext( &syntaxcontext );
            exit( exitVal );
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
            fprintf( stderr, "Nevalidni vyraz nalezeny v prubehu sematicke analyzy! (parametr vyjimky: %d)\n", *e );
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
            fprintf( stderr, "Nastala neocekavana vyjimka v prubehu semanticke analyzy!\n" );
            deleteFunction( mainFunction );
            freeValueTable( context.globals, countOfFunctions );
            destroyDefaultSyntaxContext( &syntaxcontext );
            exit( 99 );
        }
    }
    
    /****************************** Interpretace ******************************/

    try{
        // Spusteni hlavni funkce programu (bez parametru)
        Value ret = evalFunction( &mainFunction, (ExpressionList){NULL,0}, &context );
        freeValue( &ret );
    }
    catch{
        on( UndefinedVariable, e ){
            fprintf( stderr, "Byla pouzita promenna bez definovani jeji hodnoty!\n" );
            exitVal = 3;
        }
        on( UndefinedFunction, e ){
            fprintf( stderr, "Byla pouzita nedefinovana funkce!\n" );
            exitVal = 4;
        }
        on( UnexpectedValueType, e ){
            UnexpectedValueTypePrint(*e);
            if ( e->type == CONSTANT )
              exitVal = 5;
            else
              exitVal = 11;
        }
        on( NegativeNumeric, e ){
            fprintf( stderr, "ValueError: Bylo pouzito zaporne cislo tam kde nemelo!\n" );
            if ( e == CONSTANT )
              exitVal = 5;
            else
              exitVal = 11;
        }
        on( DividingByZero, e ){
            fprintf( stderr, "Doslo k pokusu o deleni nulou!\n" );
            exitVal = 10;
        }
        on( VariableIsNotFunction, e ){
            fprintf( stderr, "Pokus pouzit funkci jako promennou!\n" );
            exitVal = 11;
        }
        on( FunctionIsNotVariable, e ){
            fprintf( stderr, "Pokus pouzit promennou jako funkci!\n" );
            exitVal = 11;
        }
        on( IncompatibleComparison, e ){
            fprintf( stderr, "Nekompatibilni porovnavani - rozdilne datove typy porovnavanych hodnot!\n" );
            exitVal = 11;
        }
        on( BadArgumentType, e ){
            fprintf( stderr, "Funkce %s byla zavolana s parametrem chybneho typu!\n", *e );
            exitVal = 11;
        }
        on( InvalidConversion, value ){
            RCString buf = getValueString(value);
            fprintf( stderr, "Nezdaril se prevod hodnoty \""); RCStringPrint(&buf, stderr); fprintf( stderr, "\" na typ numeric!\n");
            exitVal = 12;
        }
        on( IndexOutOfBounds, e ){
            fprintf( stderr, "Index mimo pole!\n" );
            exitVal = 13;
        }
        on( OtherRuntimeErrors, e ){
            fprintf( stderr, "Nastala jina behova chyba!\n" );
            exitVal = 13;
        }
        on(OutOfMemory, typename){
            fprintf( stderr, "Nezdarila se alokace pameti pro typ \"%s\"!\n", *typename );
            exitVal = 99;
        }
        onAll{
            fprintf( stderr, "Nastala neocekavana vyjimka v prubehu vykonavani programu!\n" );
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

