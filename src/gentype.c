#include "gentype.h"

/**
 * Uvolnit hodnotu generickeho typu
 */
void freeGTvalue(GenType *object){
 if(object->value!=NULL){
  //if(object->type==typeArray) freeArray(object->value);
  free(object->value);
 }
 object->value=NULL;
}

/**
 * Ulozit do GT nil
 */
void setGTnil(GenType *object){
 freeGTvalue(object);
 object->type=typeNil;
}

/**
 * Ulozit do GT bool
 */
void setGTbool(GenType *object, bool value){
 freeGTvalue(object);
 object->type=typeBool;
 object->value=malloc(sizeof(value));
 MALLCHECK(object->value);
 memcpy(object->value,&value,sizeof(value));
}

/**
 * Ulozit do GT int
 */
void setGTint(GenType *object, int value){
 freeGTvalue(object);
 object->type=typeInt;
 object->value=malloc(sizeof(value));
 MALLCHECK(object->value);
 memcpy(object->value,&value,sizeof(value));
}

/**
 * Dostat z GT string
 */
char* getGTstring(GenType *object){
 
 char *output;
 
 switch(object->type){
  case typeNil:
   NEWSTRING(output,"nil");
  break;
  case typeBool:
   if(*(bool*)object->value==true){
    NEWSTRING(output,"true");
   }else{
    NEWSTRING(output,"false");
   }
  break;
  case typeInt:
   
  break;
 }
 
 return output;
}

