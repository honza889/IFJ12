#include "gentype.h"

#define BUFFERSIZE 64

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
 * Ulozit do GT numeric (desetinne cislo)
 */
void setGTnumeric(GenType *object, double value){
 freeGTvalue(object);
 object->type=typeNumeric;
 object->value=malloc(sizeof(value));
 MALLCHECK(object->value);
 memcpy(object->value,&value,sizeof(value));
}

/**
 * Ulozit do GT retezec
 */
void setGTstring(GenType *object, char *value){
 freeGTvalue(object);
 int valueSize=(strlen(value)+1)*sizeof(char);
 object->type=typeString;
 object->value=malloc(valueSize);
 MALLCHECK(object->value);
 memcpy(object->value,value,valueSize);
}

/**
 * Dostat z GT string (bez ohledu na to co obsahuje)
 */
char* getGTstring(GenType *object){
 char *output=NULL;
 int outputSize=0;
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
   output=malloc(BUFFERSIZE*sizeof(char));
   MALLCHECK(output);
   if( snprintf(output,BUFFERSIZE,"%d",*(int*)object->value) >= BUFFERSIZE ){
    ERROR("Preteceni bufferu - necekane dlouhe cislo!");
   }
   output=realloc(output,(strlen(output)+1)*sizeof(char));
   MALLCHECK(output);
  break;

  case typeNumeric:
   output=malloc(BUFFERSIZE*sizeof(char));
   MALLCHECK(output);
   if( snprintf(output,BUFFERSIZE,"%g",*(double*)object->value) >= BUFFERSIZE ){
    ERROR("Preteceni bufferu - necekane dlouhe cislo!");
   }
   output=realloc(output,(strlen(output)+1)*sizeof(char));
   MALLCHECK(output);
  break;
  
  case typeString:
   // Pozdeji mozna pujde optimalizovat - zalezi jak se bude vraceny retezec vyuzivat
   outputSize=((strlen(object->value)+1)*sizeof(char));
   output=malloc(outputSize);
   MALLCHECK(output);
   memcpy(output,object->value,outputSize);
   return object->value;
  break;
  
 }
 return output;
}

