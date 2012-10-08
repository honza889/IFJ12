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
 * Hodnota promenne generickeho datoveho typu
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

// Konstruktory Value...
inline Value newValueUndefined(){ return (Value){ .type=typeUndefined }; };
inline Value newValueNil(){ return (Value){ .type=typeNil }; };
inline Value newValueBoolean(bool value){ return (Value){ .type=typeNil, .data.boolean=value }; };
inline Value newValueNumeric(double value){ return (Value){ .type=typeNumeric, .data.numeric=value }; };
inline Value newValueFunction(Function *value){ return (Value){ .type=typeFunction, .data.function=value }; };
inline Value newValueString(char *value){ return (Value){ .type=typeString, .data.string=value }; };

// Ulozit do Value typ...
void setValueUndefined(Value *object);
void setValueNil(Value *object);
void setValueBoolean(Value *object, bool value);
void setValueNumeric(Value *object, double value);
void setValueFunction(Value *object, Function *value);
void setValueString(Value *object, char *value);

// Ziskat z Value typ... (bez ohledu na to co obsahuje)
char* getValueString(Value *object);
bool getValueBoolean(Value *object);

inline int typeOfValue(Value *object){ return (int)object->type; };

#endif
