// Vestavene funkce
// Zdenek Tretter

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "bif.h"
#include "ast.h"
#include "value.h"
#include "global.h"
#include "exceptions.h"

// Zjisti jestli retezec obsahuje cislo ve spravnem tvaru
bool isNumberInString(char *str)
{
    int i = 0;
    enum
    {
        WHITESPACE,
        NUMBER1,
        POINT,
        NUMBER2,
        EXPONENT,
        SIGN,
        NUMBER3,
        ERROR,
        OK
    }state;

    if(str[i] == ' ' || str[i] == '\t' || str[i] == '\n') state = WHITESPACE;
    else if(str[i] >= '0' && str[i] <= '9') state = NUMBER1;
    else state = ERROR;
    i++;
    while(state != ERROR && state != OK)
    {
        switch(state)
        {
            case WHITESPACE:
                if(str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
                    state = WHITESPACE;
                else if(str[i] >= '0' && str[i] <= '9') state = NUMBER1;
                else state = ERROR;
                break;
            case NUMBER1:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER1;
                else if(str[i] == '.') state = POINT;
                else if(str[i] == 'e') state = EXPONENT;
                else state = ERROR;
                break;
            case POINT:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER2;
                else state = ERROR;
                break;
            case NUMBER2:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER2;
                else if(str[i] == 'e') state = EXPONENT;
                else state = OK;
                break;
            case EXPONENT:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER3;
                else if(str[i] == '+' || str[i] == '-') state = SIGN;
                else state = ERROR;
                break;
            case SIGN:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER3;
                else state = ERROR;
                break;
            case NUMBER3:
                if(str[i] >= '0' && str[i] <= '9') state = NUMBER3;
                else state = OK;
                break;
            default:
                break;
        }
        i++;
    }
    return (state == ERROR) ? false : true;
}

// Nacte retezec ze stdin
Value BIFinput()
{
    int c, i = 0, max = 64;
    char *str = malloc(max * sizeof(char));
    MALLCHECK(str);
    while((c = getchar()) != EOF && c != '\n')
    {
        if(i == max - 1)
        {
            max = max * 2;
            char *restr = realloc(str, max * sizeof(char));
            if(restr == NULL)
            {
                free(str);
                str = NULL;
                MALLCHECK(restr);
            }
            str = restr;
        }
        str[i] = c;
        i++;
    }
    str[i] = '\0';
    return newValueString(str);
}

// Prevede retezec na cislo
Value BIFnumeric(Value object)
{
    switch(object.type)
    {
        case typeUndefined: throw(UndefinedVariable, true);
        case typeFunction:  throw(BadArgumentType, "numeric");
        case typeBoolean:
        case typeNil: throw(InvalidConversion, object);
        case typeNumeric: return object;
        case typeString:
            if(isNumberInString(object.data.string))
            {
                Value number = newValueNumeric(atof(object.data.string));
                if(errno == ERANGE) throw(InvalidConversion, object);
                else return number;
            }
            else throw(InvalidConversion, object);
    }
    exit(99); // Jen pro kompilaci
}

// Vypise to co dostane
Value BIFprint(ValueList param, int count)
{
    for(int i = 0; i < count; i++)
    {
        switch(param[i].type)
        {
            case typeNil:
                printf("Nil");
                break;
            case typeBoolean:
                if(param[i].data.boolean) printf("true");
                else printf("false");
                break;
            case typeNumeric:
                printf("%g", param[i].data.numeric);
                break;
            case typeString:
                printf("%s", param[i].data.string);
                break;
            case typeFunction: throw(BadArgumentType, "print");
            case typeUndefined: throw(UndefinedVariable, true);
        }
    }
    return newValueNil();

}

// Vrati cislo odpovidajici typu co dostala
Value BIFtypeOf(Value object)
{
    switch(object.type)
    {
        case typeNil: return newValueNumeric(0.0);
        case typeBoolean: return newValueNumeric(1.0);
        case typeNumeric: return newValueNumeric(3.0);
        case typeFunction: return newValueNumeric(6.0);
        case typeString: return newValueNumeric(8.0);
        case typeUndefined: throw(UndefinedVariable, true);
    }
    exit(99); // Jen pro kompilaci
}

// Vrati delku retezce
Value BIFlen(Value object)
{
    switch(object.type)
    {
        case typeUndefined: throw(UndefinedVariable, true);
        case typeString: return newValueNumeric((double)strlen(object.data.string));
        default: return newValueNumeric(0.0);
    }
}
