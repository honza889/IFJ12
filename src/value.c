#include "value.h"

#include <stdlib.h>
#include <math.h>
#include "global.h"

#define BUFFERSIZE 128

/**
 * Uvolnit obsah hodnoty
 */
void freeValue(Value *object){
 if(object->type==typeString && object->data.string){
  free(object->data.string);
 }
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
 * Ulozit do GT numeric (desetinne cislo)
 */
void setValueNumeric(Value *object, double value){
 freeValue(object);
 object->type=typeNumeric;
 object->data.numeric=value;
}

/**
 * Ulozit do GT retezec
 */
void setValueString(Value *object, char *value){
 freeValue(object);
 int valueSize=(strlen(value)+1)*sizeof(char);
 object->type=typeString;
 object->data.string=malloc(valueSize);
 MALLCHECK(object->data.string);
 memcpy(object->data.string,value,valueSize);
}

/**
 * Dostat z GT string (bez ohledu na to co obsahuje)
 */
char* getValueString(Value *object){
 char *output=NULL;
 int outputSize=0;
 switch(object->type){

  case typeUndefined:
   fprintf(stderr,"\nError: Undefined variable!\n");
   exit(3);
  break;
  
  case typeNil:
   NEWSTRING(output,"nil");
  break;
  
  case typeBoolean:
   if(object->data.boolean==true){
    NEWSTRING(output,"true");
   }else{
    NEWSTRING(output,"false");
   }
  break;

  case typeNumeric:
   output=malloc(BUFFERSIZE*sizeof(char));
   MALLCHECK(output);
   if( snprintf(output,BUFFERSIZE,"%g",object->data.numeric) >= BUFFERSIZE ){
    ERROR("Preteceni bufferu - necekane dlouhe cislo!");
   }
   output=realloc(output,(strlen(output)+1)*sizeof(char));
   MALLCHECK(output);
  break;
  
  case typeString:
   outputSize=((strlen(object->data.string)+1)*sizeof(char));
   output=malloc(outputSize);
   MALLCHECK(output);
   memcpy(output,object->data.string,outputSize);
  break;
  
 }
 return output;
}

bool getValueBoolean(Value *object){
 switch(object->type){
  
  case typeUndefined:
   fprintf(stderr,"\nError: Undefined variable!\n");
   exit(3);
  break;
  
  case typeNumeric:
   if(object->data.numeric==0.0 || object->data.numeric==NAN){
    return false;
   }else{
    return true;
   }
  break;
  
  case typeString:
   if(object->data.string[0]=='\0'){
    return false; // prazdny retezec
   }else{
    return true; // neprazdny retezec
   }
  break;
  
 }
 return false;
}
