
#include <stdio.h>
#include <assert.h>
#include "value.h"
#include "symbols.h"
#include "../test.h"

BEGIN_TEST
 
 SymbolTable global = {NULL,0};
 SymbolTable local = {NULL,0};
 
 // Prvni cteni (vytvateni stromu)
 TEST( getSymbol("promenna1",&global,&local) == 0 );
 TEST( getSymbol("promenna2",&global,&local) == 1 );
 TEST( getSymbol("promenna3",&global,&local) == 2 );
 TEST( getSymbol("promenna0",&global,&local) == 3 );
 TEST( getSymbol("funkce1",&global,NULL) == -1 );
 TEST( getSymbol("funkce4",&global,NULL) == -2 );
 TEST( getSymbol("funkce2",&global,NULL) == -3 );
 TEST( getSymbol("funkce3",&global,NULL) == -4 );

 // Druhe cteni (cteni ze stromu)
 TEST( getSymbol("promenna1",&global,&local) == 0 );
 TEST( getSymbol("promenna2",&global,&local) == 1 );
 TEST( getSymbol("promenna3",&global,&local) == 2 );
 TEST( getSymbol("promenna0",&global,&local) == 3 );
 TEST( getSymbol("funkce1",&global,NULL) == -1 );
 TEST( getSymbol("funkce4",&global,NULL) == -2 );
 TEST( getSymbol("funkce2",&global,NULL) == -3 );
 TEST( getSymbol("funkce3",&global,NULL) == -4 );

 freeSymbolTable(&global);
 freeSymbolTable(&local);
 
 Value *globalTable = initValueTable(&global);
 Value *localTable = initValueTable(&local);
 
 // Overeni nedefinovaneho stavu promennych po inicializaci
 TEST( SYMBOL(-4,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(-3,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(-2,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(-1,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(0,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(1,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(2,localTable,globalTable)->type == typeUndefined );
 TEST( SYMBOL(3,localTable,globalTable)->type == typeUndefined );
 
 freeValueTable(globalTable,&global);
 freeValueTable(localTable,&local);
 
END_TEST