#include "value.h"

#include <stdlib.h>
#include <math.h>
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
  
  case typeString:
   return copyRCString( &object->data.string );
  break;
  
  
  default:
   ERROR("Neni implementovano: getValueString");
  break;
  
 }
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
   ERROR("getValueBoolean: neni implementovano pro typeFunction!");
  break;
  
  case typeString:
   if(RCStringEmpty(&object->data.string)){
    return false; // prazdny retezec
   }else{
    return true; // neprazdny retezec
   }
  break;
  
 }
 return false;
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

