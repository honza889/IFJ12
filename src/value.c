/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include "value.h"
#include "global.h"
#include "exceptions.h"

// Maximalni delka cisla (ve znacich) pro prevod na retezec
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
		ERROR("Preteceni bufferu - necekane dlouhe cislo!");exit(99);
	}
	RCString ret = makeRCString( output );
	free( output );
	return ret;
   }
  break;
  
  case typeFunction:
    throw(FunctionIsNotVariable, *object);
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
   throw(FunctionIsNotVariable, *object);
  break;
  
  case typeString:
   if(RCStringEmpty(&object->data.string)){
    return false; // prazdny retezec
   }else{
    return true; // neprazdny retezec
   }
  break;
  
 }
 ERROR("getValueBoolean: Neni implementovano pro dany typ!");
 exit(99);
}


/**
 * Zjisti jestli retezec obsahuje cislo ve spravnem tvaru
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
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
    if(strlen==0) return false;
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
    return ( state == OK || state == NUMBER2 || state==NUMBER3 );
}

/**
 * Dostat z value numerickou hodnotu
 */
double getValueNumeric(Value *object){
    switch(object->type)
    {
        case typeUndefined: throw(UndefinedVariable, -1);
        case typeFunction:  throw(FunctionIsNotVariable, *object);
        case typeBoolean: throw(InvalidConversion, *object);
        case typeNil: throw(InvalidConversion, *object);
        case typeNumeric: return object->data.numeric;
        case typeString:
            if(isNumberInString(RCStringGetBuffer(&object->data.string),RCStringLength(&object->data.string)))
            {
                RCString str = copyRCString(&object->data.string);
                RCStringAppendChar(&str,'\0');
                double number = strtod( RCStringGetBuffer(&str),NULL);
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
    if(object->type==typeUndefined){
        throw(UndefinedFunction, -1);
    }
    if(object->type!=typeFunction){
        throw(VariableIsNotFunction, *object);
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

/**
 * Zjisti, zda je \a value typu \a expected.
 * Pokud ne, haze vyjimku UnexpectedValueType.
 */
Value* testValue( Value* value, ValueType expected )
{
    if ( value->type != expected )
    {
        throw( UnexpectedValueType, ((UnexpectedValueTypeException){ 
            .expected = expected,
            .got = value->type
        }));
    }
    
    return value;
}

/**
 * Zjisti, zda je \a value typu typeNumeric a zda je nezáporné číslo.
 * Pokud ne, haze vyjimku NegativeNumeric nebo UnexpectedValueType.
 */
Value* testValuePositiveNumeric( Value* value )
{
    if ( getValueNumeric(testValue( value, typeNumeric )) < 0 )
    {
        throw( NegativeNumeric, *value);
    }
    
    return value;
}

/**
 * Alokuje tabulku symbolu
 */
Value* initValueTable(int length){
 Value *table = malloc(length*sizeof(Value));
 for(int i=0;i<length;i++){
  table[i]=(Value){.type=typeUndefined};
 }
 return table;
}

/**
 * Uvolni tabulku symbolu
 */
void freeValueTable(Value *table, int length) {
  for (int i=0; i < length; i++) {
    if (table[i].type==typeFunction) {
      deleteFunction( *table[i].data.function );
      free(table[i].data.function);
      table[i].data.function = NULL;
    }
    else {
      freeValue( &table[i] );
    }
  }
  free(table);
  table=NULL;
}

/**
 * Uvolni globalni tabulku symbolu - funkci
 */
void freeFunctionsTable(Value *table, int length) {
  for (int i=0; i < length; i++) {
    deleteFunction( *table[i].data.function );
    free(table[i].data.function);
    table[i].data.function = NULL;
  }
  free(table);
  table=NULL;
}

/**
 * Operace scitani: a + b
 */
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
    else if( a->type == typeUndefined || b->type == typeUndefined )
    {
        throw( UndefinedVariable, 0 );
    } 
    else
    {
        throw( BadArgumentType, "operator +" );
    }
    return newValueUndefined();
}
    
/**
 * Operace odcitani: a - b
 */
Value subtractValue( Value* a, Value* b )
{
    if( a->type == typeNumeric && b->type == typeNumeric )
    {
        return newValueNumeric( getValueNumeric( a ) - getValueNumeric( b ) );
    }
    else if( a->type == typeUndefined || b->type == typeUndefined )
    {
        throw( UndefinedVariable, 0 );
    }
    else
    {
        throw( BadArgumentType, "operator -" );
    }
    return newValueUndefined();
}

/**
 * Operace nasobeni: a * b
 */
Value multiplyValue( Value* a, Value* b )
{
    if( a->type == typeNumeric && b->type == typeNumeric )
    {
        return newValueNumeric( getValueNumeric( a ) * getValueNumeric( b ) );
    }
    else if( a->type == typeString && b->type == typeNumeric )
    {
        RCString result = makeEmptyRCString();
        RCString addedStr = getValueString( a );
        int howMany = (int)getValueNumeric( b );
        for( int i = 0; i < howMany; i++ )
        {
            RCStringAppendStr( &result, &addedStr );
        }
        Value ret = newValueString( result );
        deleteRCString( &addedStr );
        deleteRCString( &result );
        return ret;
    }
    else if( a->type == typeUndefined || b->type == typeUndefined )
    {
        throw( UndefinedVariable, 0 );
    }
    else
    {
        throw( BadArgumentType, "operator *" );
    }
    return newValueUndefined();
}

/**
 * Operace deleni: a / b
 */
Value divideValue( Value* a, Value* b )
{
    if( a->type == typeNumeric && b->type == typeNumeric )
    {
        return newValueNumeric( getValueNumeric( a ) / getValueNumeric( b ) );
    }
    else if( a->type == typeUndefined || b->type == typeUndefined )
    {
        throw( UndefinedVariable, 0 );
    }
    else
    {
        throw( BadArgumentType, "operator /" );
    }
    return newValueUndefined();
}

/**
 * Operace umocneni: a ** b
 */
Value powerValue( Value* a, Value* b )
{
    if( a->type == typeNumeric && b->type == typeNumeric )
    {
        return newValueNumeric( pow ( getValueNumeric( a ), getValueNumeric( b ) ) );
    }
    else if( a->type == typeUndefined || b->type == typeUndefined )
    {
        throw( UndefinedVariable, 0 );
    }
    else
    {
        throw( BadArgumentType, "operator **" );
    }
    return newValueUndefined();
}

