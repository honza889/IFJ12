#include "value.h"

#include <stdlib.h>
#include <math.h>
#include "global.h"

#define BUFFERSIZE 128

/**
 * Ulozit hodnotu Undefined
 */
void setValueUndefined(Value *object){
 freeValue(object);
 object->type=typeUndefined;
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
 * Ulozit do Value numeric (desetinne cislo)
 */
void setValueNumeric(Value *object, double value){
 freeValue(object);
 object->type=typeNumeric;
 object->data.numeric=value;
}

/**
 * Ulozit do Value (ukazatel na) funkci
 */
void setValueFunction(Value *object, Function *value){
 freeValue(object);
 object->type=typeFunction;
 object->data.function=value;
}

/**
 * Ulozit do Value retezec
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
 * Dostat z Value string (bez ohledu na to co obsahuje)
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
   NEWSTRING(output,"Nil");
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
  
  case typeFunction:
   ERROR("Neni implementovano: getValueString pro typeFuntion!");
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

/**
 * Dostat z Value logickou hodnotu (napr. pri pouziti ve vyrazu)
 */
bool getValueBoolean(Value *object){
 switch(object->type){
  
  case typeUndefined:
   fprintf(stderr,"\nError: Undefined variable!\n");
   exit(3);
  break;
  
  case typeNil:
   return false;
  break;
  
  case typeBoolean:
   return object->data.boolean;
  break;
  
  case typeNumeric:
   if(object->data.numeric==0.0){
    return false; // nulova hodnota ciselne promenne
   }else{
    return true; // nenulova hodnota ciselne promenne
   }
  break;
  
  case typeFunction:
   ERROR("getValueBoolean: neni implementovano pro typeFunction!");
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

/**
 * value1 == value2 (viz.5.1)
 */
bool equalValue(Value *value1, Value *value2){
 if(value1->type==typeNil && value2->type==typeNil){
  return true;
 }else
 if(value1->type==typeBoolean && value2->type==typeBoolean){
  return (value1->data.boolean==value2->data.boolean);
 }else
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric==value2->data.numeric);
 }else
 if(value1->type==typeFunction && value2->type==typeFunction){
  return (value1->data.function==value2->data.function);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return (strcmp(value1->data.string,value2->data.string)==0);
 }else{
  return false; // operandy nejsou stejneho typu, neni chyba!
 }
}

/**
 * value1 > value2 (viz.5.1)
 */
bool greaterValue(Value *value1, Value *value2){
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric>value2->data.numeric);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return (strcmp(value1->data.string,value2->data.string)>0);
 }else{
  return false; // TODO: chyba!
 }
}

/**
 * value1 >= value2 (viz.5.1)
 */
bool greaterEqualValue(Value *value1, Value *value2){
 if(value1->type==typeNumeric && value2->type==typeNumeric){
  return (value1->data.numeric>=value2->data.numeric);
 }else
 if(value1->type==typeString && value2->type==typeString){
  return (strcmp(value1->data.string,value2->data.string)>=0);
 }else{
  return false; // TODO: chyba!
 }
}

/**
 * Uvolnit obsah hodnoty
 */
void freeValue(Value *object){
 if(object->type==typeString && object->data.string){
  free(object->data.string);
 }
}

