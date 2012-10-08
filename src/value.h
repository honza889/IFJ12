#ifndef VALUE_H
#define VALUE_H

/**
 * Genericky datovy typ pro promenne jazyka
 */

#include <stdbool.h>
#include "global.h"

// forward deklarace pro vyreseni krizovych zavislosti
typedef struct SFunction Function; 

/**
 * Genericky datovy typ (GT)
 */
typedef struct {
 
 // Aktualni typ
 enum {
  typeUndefined=-1,
  typeNil=0,
  typeBoolean=1,
  typeNumeric=3,
  typeFunction=6,
  typeString=8
 } type;
 
 // Hodnota
 union {
  bool boolean;
  double numeric;
  Function *function;
  char *string;
 } data;
 
} Value;

void freeValue(Value *object);

// Ulozit do Value typ...
void setValueUndefined(Value *object);
void setValueNil(Value *object);
void setValueBoolean(Value *object, bool value);
void setValueNumeric(Value *object, double value);
//void setValueFunction(Value *object, Function *value);
void setValueString(Value *object, char *value);

Value newValueUndefined();
Value newValueNil();
Value newValueBoolean( bool value );
Value newValueNumeric( double value );
Value newValueFunction( Function *value );
Value newValueString( char *value );

// Ziskat z Value typ... (bez ohledu na to co obsahuje)
char* getValueString(Value *object);
bool getValueBoolean(Value *object);

#endif
