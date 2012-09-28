/**
 * Genericky datovy typ pro promenne jazyka
 */

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
//void setGTfunction(GenType *object, void *value);
//void setGTstring(GenType *object, String *value);

// Ziskat z GT typ... (bez ohledu na to co obsahuje)
char* getGTstring(GenType *object);

