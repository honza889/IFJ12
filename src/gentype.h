#ifndef GENTYPE_H
#define GENTYPE_H

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
  typeNil=0,
  typeBool=1,
  typeInt=3,
  typeNumeric=4,
  typeFunction=6,
  typeString=8
 } type;
 
 // Ukazatel na hodnotu
 void* value;
 
} GenType;

void freeGTvalue(GenType *object);

// Ulozit do GT typ...
void setGTnil(GenType *object);
void setGTbool(GenType *object, bool value);
void setGTint(GenType *object, int value);
void setGTnumeric(GenType *object, double value);
//void setGTfunction(GenType *object, void *value);
void setGTstring(GenType *object, char *value);

// Ziskat z GT typ... (bez ohledu na to co obsahuje)
char* getGTstring(GenType *object);

#endif
