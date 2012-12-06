/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef VALUE_H
#define VALUE_H

/**
 * Genericky datovy typ pro promenne jazyka
 */

#include <stdbool.h>
#include "global.h"
#include "ast.h"

Value copyValue( Value* other ); // kvuli RCStringu
void freeValue(Value *object);

/**
 * Zjisti, zda je \a value typu \a expected.
 * Pokud ne, haze vyjimku UnexpectedValueType.
 */
Value* testValue( Value* value, ValueType expected );

/**
 * Zjisti, zda je \a value typu typeNumeric a zda je nezáporné číslo.
 * Pokud ne, haze vyjimku NegativeNumeric nebo UnexpectedValueType.
 */
Value* testValuePositiveNumeric( Value* value );

// Konstruktory Value...
static inline Value newValueUndefined(){ 
  return (Value){ .type=typeUndefined };
}

static inline Value newValueNil(){
  return (Value){ .type=typeNil };
}

static inline Value newValueBoolean(bool value){ 
  return (Value){ .type=typeBoolean, .data.boolean=value };
}

static inline Value newValueNumeric(double value){ 
  return (Value){ .type=typeNumeric, .data.numeric=value };
}

static inline Value newValueFunction( struct SFunction *value){ 
  return (Value){ .type=typeFunction, .data.function=value };
}

static inline Value newValueString( RCString value){ 
  return (Value){ .type=typeString, .data.string = copyRCString( &value ) };
}

static inline Value newValueCString( const char* str)
{
    return (Value){ .type=typeString, .data.string = makeRCString( str ) };
}

// Ulozit do Value typ...
void setValueUndefined(Value *object);
void setValueNil(Value *object);
void setValueBoolean(Value *object, bool value);
void setValueNumeric(Value *object, double value);
void setValueFunction(Value *object, struct SFunction *value);
void setValueString(Value *object, RCString value);
void setValueCString(Value *object, const char* str);

// Ziskat z Value typ... (bez ohledu na to co obsahuje)
bool getValueBoolean(Value *object);
double getValueNumeric(Value *object);
Function* getValueFunction(Value *object);
RCString getValueString(Value *object);

// Porovnavani
bool equalValue(Value *value1, Value *value2); // value1 == value2
static inline bool notEqualValue(Value *value1, Value *value2){ // value1 < value2
 return ! equalValue(value2,value1);
}
bool greaterValue(Value *value1, Value *value2); // value1 > value2
bool greaterEqualValue(Value *value1, Value *value2); // value1 >= value2
static inline bool lesserValue(Value *value1, Value *value2){ // value1 < value2
 return greaterValue(value2,value1);
}
static inline bool lesserEqualValue(Value *value1, Value *value2){ // value1 <= value2
 return greaterEqualValue(value2,value1);
}

/** Alokuje a inicializuje tabulku symbolu */
Value* initValueTable(int length);

/** Uvolnuje Value tabulky symbolu */
void freeValueTable(Value *table, int length);

/** Uvolnuje tabulku funkci */
void freeFunctionsTable(Value *table,int length);

//Operace
Value addValue( Value* a, Value* b );
Value subtractValue( Value* a, Value* b );
Value multiplyValue( Value* a, Value* b );
Value divideValue( Value* a, Value* b );
Value powerValue( Value* a, Value* b );
Value andValue( Value* a, Value* b );
Value orValue( Value* a, Value* b );
Value notValue( Value* a );
Value inValue( Value* a, Value* b );
Value notInValue( Value* a, Value* b );

static inline int typeOfValue(Value *object){
  return (int)object->type;
}

#endif
