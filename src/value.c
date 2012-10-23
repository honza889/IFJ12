#include "value.h"

#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include "global.h"
#include "exceptions.h"

#define BUFFERSIZE 128

/**
 * Ulozit hodnotu Undefined
 */
void setValueUndefined(Value *object){
 freeValue(object);
 object->type=typeUndefined;
}


/**
 * Ulozit hodnotu Nil
 */
void setValueNil(Value *object){
 freeValue(object);
 object->type=typeNil;
}

/**
 * Ulozit hodnotu Bool
 */
void setValueBoolean(Value *object, bool value){
 freeValue(object);
 object->type=typeBoolean;
 object->data.boolean=value;
}

/**
 * Ulozit do Value numeric (desetinne cislo)
 */
void setValueNumeric(Value *object, double value){
 freeValue(object);
 object->type=typeNumeric;
 object->data.numeric=value;
}

/**
 * Ulozit do Value (ukazatel na) funkci
 */
void setValueFunction(Value *object, Function *value){
 freeValue(object);
 object->type=typeFunction;
 object->data.function=value;
}

/**
 * Ulozit do Value retezec
 */
void setValueString(Value *object, RCString str){
	str = copyRCString( &str );
	freeValue( object );
	object->type=typeString;
	object->data.string = copyRCString( &str );
	deleteRCString( &str );
}

void setValueCString(Value *object, const char* str)
{
	freeValue( object );
	object->type=typeString;
	object->data.string = makeRCString( str );
}

/**
 * Dostat z Value string (bez ohledu na to co obsahuje)
 */
RCString getValueString(Value *object){
 switch(object->type){

  case typeUndefined:
   throw(UndefinedVariable,true);
  break;
  
  case typeNil:
   return makeRCString("Nil");
  break;
  
  case typeBoolean:
   if(object->data.boolean==true){
    return makeRCString("true");
   }else{
    return makeRCString("false");
   }
  break;

  case typeNumeric:
   {
	char* output=malloc(BUFFERSIZE*sizeof(char));
	MALLCHECK(output);
	if( snprintf(output,BUFFERSIZE,"%g",object->data.numeric) >= BUFFERSIZE ){
		ERROR("Preteceni bufferu - necekane dlouhe cislo!");
	}
	RCString ret = makeRCString( output );
	free( output );
	return ret;
   }
  break;
  
  case typeFunction:
    throw(BadArgumentType, "functionToString");
  break;
  
  case typeString:
   return copyRCString( &object->data.string );
  break;
  
 }
 ERROR("getValueString: Neni implementovano pro dany typ!");
 exit(99);
}

/**
 * Dostat z Value logickou hodnotu (napr. pri pouziti ve vyrazu)
 */
bool getValueBoolean(Value *object){
 switch(object->type){
  
  case typeUndefined:
   throw(UndefinedVariable,true);
  break;
  
  case typeNil:
   return false;
  break;
  
  case typeBoolean:
   return object->data.boolean;
  break;
  
  case typeNumeric:
   if(object->data.numeric==0.0){
    return false; // nulova hodnota ciselne promenne
   }else{
    return true; // nenulova hodnota ciselne promenne
   }
  break;
  
  case typeFunction:
   throw(BadArgumentType,"getValueBoolean neni implementovano pro typeFunction!");
  break;
  
  case typeString:
   if(RCStringEmpty(&object->data.string)){
    return false; // prazdny retezec
   }else{
    return true; // neprazdny retezec
   }
  break;
  
 }
 ERROR("getValueString: Neni implementovano pro dany typ!");
 exit(99);
}


/**
 * Zjisti jestli retezec obsahuje cislo ve spravnem tvaru
 * @author Zdenek Tretter
 */
bool isNumberInString(const char *str,int strlen)
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
    while(state != ERROR && state != OK && i<strlen)
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

/**
 * Dostat z value numerickou hodnotu
 */
double getValueNumeric(Value *object){
    switch(object->type)
    {
        case typeUndefined: throw(UndefinedVariable, -1);
        case typeFunction:  throw(BadArgumentType, NULL);
        case typeBoolean: throw(InvalidConversion, *object);
        case typeNil: throw(InvalidConversion, *object);
        case typeNumeric: return object->data.numeric;
        case typeString:
            if(isNumberInString(RCStringGetBuffer(&object->data.string),RCStringLength(&object->data.string)))
            {
                RCString str = copyRCString(&object->data.string);
                RCStringAppendChar(&str,'\0');
                double number = atof(RCStringGetBuffer(&str));
                deleteRCString(&str);
                if(errno == ERANGE) throw(InvalidConversion, *object);
                else return number;
            }
            else throw(InvalidConversion, *object);
    }
    ERROR("getValueNumeric: Neni implementovano pro dany typ!");
    exit(99);
}

Function* getValueFunction(Value *object){
    if(object->type!=typeFunction){
        throw(InvalidConversion, *object);
    }
    return object->data.function;
}

/**
 * value1 == value2 (viz.5.1)
 */
bool equalValue(Value *value1, Value *value2){
 if(value1->type==typeUndefined || value2->type==typeUndefined){
  throw(UndefinedVariable,true);
 }else
 if(value1->type==typeNil && value2->type==typeNil){
  return true;
 }else
 if(value1->type==typeBoolean && value2->type==typeBoolean){
  return (value1->data.boolean==value2->data.boolean);
 }else
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric==value2->data.numeric);
 }else
 if(value1->type==typeFunction && value2->type==typeFunction){
  return (value1->data.function==value2->data.function);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return (RCStringCmp(&value1->data.string,&value2->data.string)==0);
 }
 return false; // operandy nejsou stejneho typu, neni chyba!
}

/**
 * value1 > value2 (viz.5.1)
 */
bool greaterValue(Value *value1, Value *value2){
 if(value1->type==typeUndefined || value2->type==typeUndefined){
  throw(UndefinedVariable,true);
 }else
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric>value2->data.numeric);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return (RCStringCmp(&value1->data.string,&value2->data.string)>0);
 }
 throw(IncompatibleComparison,true);
 return false;
}

/**
 * value1 >= value2 (viz.5.1)
 */
bool greaterEqualValue(Value *value1, Value *value2){
 if(value1->type==typeUndefined || value2->type==typeUndefined){
  throw(UndefinedVariable,true);
 }else
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric>=value2->data.numeric);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return ( RCStringCmp(&value1->data.string,&value2->data.string)>=0);
 }
 throw(IncompatibleComparison,true);
 return false;
}

Value copyValue( Value* other )
{
	Value ret = *other;
	if( ret.type == typeString )
	{
		ret.data.string = copyRCString( &ret.data.string );
	}
	return ret;
}

/**
 * Uvolnit obsah hodnoty
 */
void freeValue(Value *object){
 if(object->type==typeString){
  deleteRCString( &object->data.string );
 }
}

Value addValue( Value* a, Value* b )
{
    if( a->type == typeNumeric && b->type == typeNumeric )
    {
        return newValueNumeric( getValueNumeric( a ) + getValueNumeric( b ) );
    }
    else if( ( a->type == typeString && b->type == typeString ) ||
             ( a->type == typeNumeric && b->type == typeString ) ||
             ( a->type == typeString && b->type == typeNumeric ) )
    {
        RCString as = getValueString( a );
        RCString bs = getValueString( b );
        RCStringAppendStr( &as, &bs );
        Value ret = newValueString( as );
        deleteRCString( &as );
        deleteRCString( &bs );
        return ret;
    }
    else
    {
        throw( BadArgumentType, "addValue" );
    }
    return newValueUndefined();
}
        
