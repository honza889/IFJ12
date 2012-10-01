#ifndef VALUE_H
#define VALUE_H

/**
 * Genericky datovy typ pro promenne jazyka
 */

#include <stdbool.h>
#include "global.h"

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
  void *function;
  void *string;
 } data;
 
} Value;

void freeValue(Value *object);

// Ulozit do Value typ...
void setValueNil(Value *object);
void setValueBoolean(Value *object, bool value);
void setValueNumeric(Value *object, double value);
//void setValueFunction(Value *object, void *value);
void setValueString(Value *object, char *value);

// Ziskat z Value typ... (bez ohledu na to co obsahuje)
char* getValueString(Value *object);

#endif
