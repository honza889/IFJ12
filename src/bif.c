// Vestavene funkce
// Zdenek Tretter

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "bif.h"
#include "ast.h"
#include "value.h"
#include "exceptions.h"
#include "rcstring.h"

// Nacte retezec ze stdin
Value BIFinput()
{
    RCString str = makeEmptyRCString();
    int c;
    while((c = getchar()) != EOF && c != '\n')
    {
        RCStringAppendChar( &str, c );
    }
    return newValueString(str);
}

// Prevede retezec na cislo
Value BIFnumeric(Value object)
{
    return newValueNumeric(getValueNumeric(&object));
}

// Vypise to co dostane
Value BIFprint(ValueList param, int count)
{
    for(int i = 0; i < count; i++)
    {
        if(param[i].type == typeFunction) throw(BadArgumentType, "print");
        RCString str = getValueString( &param[i] );
        RCStringPrint( &str, stdout );
        deleteRCString( &str );
    }
    return newValueNil();

}

// Vrati cislo odpovidajici typu co dostala
Value BIFtypeOf(Value object)
{
    return newValueNumeric((double)object.type);
}

// Vrati delku retezce
Value BIFlen(Value object)
{
    switch(object.type)
    {
        case typeUndefined: throw(UndefinedVariable, true);
        case typeString: return newValueNumeric((double)RCStringLength(&object.data.string));
        default: return newValueNumeric(0.0);
    }
}
