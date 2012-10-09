#ifndef VALUE_H
#define VALUE_H

/**
 * Genericky datovy typ pro promenne jazyka
 */

#include <stdbool.h>
#include "global.h"
#include "ast.h"

void freeValue(Value *object);

// Konstruktory Value...
static inline Value newValueUndefined(){ 
	return (Value){ .type=typeUndefined };
}

static inline Value newValueNil(){
	return (Value){ .type=typeNil };
}

static inline Value newValueBoolean(bool value){ 
	return (Value){ .type=typeNil, .data.boolean=value };
}

static inline Value newValueNumeric(double value){ 
	return (Value){ .type=typeNumeric, .data.numeric=value };
}

static inline Value newValueFunction( struct SFunction *value){ 
	return (Value){ .type=typeFunction, .data.function=value };
}

static inline Value newValueString(char *value){ 
	return (Value){ .type=typeString, .data.string=value };
}

// Ulozit do Value typ...
void setValueUndefined(Value *object);
void setValueNil(Value *object);
void setValueBoolean(Value *object, bool value);
void setValueNumeric(Value *object, double value);
void setValueFunction(Value *object, struct SFunction *value);
void setValueString(Value *object, char *value);

// Ziskat z Value typ... (bez ohledu na to co obsahuje)
char* getValueString(Value *object);
bool getValueBoolean(Value *object);

// Porovnavani
bool equalValue(Value *value1, Value *value2); // value1 == value2
bool greaterValue(Value *value1, Value *value2); // value1 > value2
bool greaterEqualValue(Value *value1, Value *value2); // value1 >= value2
static inline bool lesserValue(Value *value1, Value *value2){ // value1 < value2
 return greaterValue(value2,value1);
}
static inline bool lesserEqualValue(Value *value1, Value *value2){ // value1 <= value2
 return greaterEqualValue(value2,value1);
}

static inline int typeOfValue(Value *object){
	return (int)object->type;
}

#endif
