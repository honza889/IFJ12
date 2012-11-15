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
#include <stdbool.h>

#include "bif.h"
#include "ast.h"
#include "value.h"
#include "exceptions.h"
#include "rcstring.h"

// Nacte retezec ze stdin
Value BIFinput(ValueList param, int count)
{
    RCString str = makeEmptyRCString();
    int c;
    while((c = getchar()) != EOF && c != '\n')
    {
        RCStringAppendChar(&str, c);
    }
    return newValueString(str);
}

// Prevede retezec na cislo
Value BIFnumeric(ValueList param, int count)
{
    return newValueNumeric(getValueNumeric(&param[0]));
}

// Vypise to co dostane
Value BIFprint(ValueList param, int count)
{
    for(int i = 0; i < count; i++)
    {
        if(param[i].type == typeFunction) throw(BadArgumentType, "print");
        RCString str = getValueString(&param[i]);
        RCStringPrint(&str, stdout);
        deleteRCString(&str);
    }
    return newValueNil();

}

// Vrati cislo odpovidajici typu co dostala
Value BIFtypeOf(ValueList param, int count)
{
    return newValueNumeric((double)param[0].type);
}

// Vrati delku retezce
Value BIFlen(ValueList param, int count)
{
    switch(param[0].type)
    {
        case typeUndefined: throw(UndefinedVariable, true);
        case typeString: return newValueNumeric((double)RCStringLength(&param[0].data.string));
        default: return newValueNumeric(0.0);
    }
}
