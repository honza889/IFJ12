
#include <stdio.h>
#include "value.h"
#include "symbols.h"
#include "../test.h"

BEGIN_TEST
 
 SymbolTable global = {NULL,0};
 SymbolTable local = {NULL,0};

 RCString promenna0 = makeRCString("promenna0");
 RCString promenna1 = makeRCString("promenna1");
 RCString promenna2 = makeRCString("promenna2");
 RCString promenna3 = makeRCString("promenna3");
 RCString funkce1 = makeRCString("funkce1");
 RCString funkce2 = makeRCString("funkce2");
 RCString funkce3 = makeRCString("funkce3");
 RCString funkce4 = makeRCString("funkce4");
 
 // Prvni cteni (vytvateni stromu)
 TEST( getSymbol(promenna1,&global,&local) == 0 );
 TEST( getSymbol(promenna2,&global,&local) == 1 );
 TEST( getSymbol(promenna3,&global,&local) == 2 );
 TEST( getSymbol(promenna0,&global,&local) == 3 );
 TEST( getSymbol(funkce1,&global,NULL) == -1 );
 TEST( getSymbol(funkce4,&global,NULL) == -2 );
 TEST( getSymbol(funkce2,&global,NULL) == -3 );
 TEST( getSymbol(funkce3,&global,NULL) == -4 );

 // Druhe cteni (cteni ze stromu)
 TEST( getSymbol(promenna1,&global,&local) == 0 );
 TEST( getSymbol(promenna2,&global,&local) == 1 );
 TEST( getSymbol(promenna3,&global,&local) == 2 );
 TEST( getSymbol(promenna0,&global,&local) == 3 );
 TEST( getSymbol(funkce1,&global,NULL) == -1 );
 TEST( getSymbol(funkce4,&global,NULL) == -2 );
 TEST( getSymbol(funkce2,&global,NULL) == -3 );
 TEST( getSymbol(funkce3,&global,NULL) == -4 );

 freeSymbolTable(&global);
 freeSymbolTable(&local);
 
 Value *globalTable = initValueTable(global.count);
 Value *localTable = initValueTable(local.count);
 Context context = (Context){globalTable,localTable};
 
 // Overeni nedefinovaneho stavu promennych po inicializaci
 TEST( symbol(-4,&context)->type == typeUndefined );
 TEST( symbol(-3,&context)->type == typeUndefined );
 TEST( symbol(-2,&context)->type == typeUndefined );
 TEST( symbol(-1,&context)->type == typeUndefined );
 TEST( symbol(0,&context)->type == typeUndefined );
 TEST( symbol(1,&context)->type == typeUndefined );
 TEST( symbol(2,&context)->type == typeUndefined );
 TEST( symbol(3,&context)->type == typeUndefined );
 
 freeValueTable(globalTable,global.count);
 freeValueTable(localTable,local.count);
 
 deleteRCString(&promenna0);
 deleteRCString(&promenna1);
 deleteRCString(&promenna2);
 deleteRCString(&promenna3);
 deleteRCString(&funkce1);
 deleteRCString(&funkce2);
 deleteRCString(&funkce3);
 deleteRCString(&funkce4);
 
END_TEST
