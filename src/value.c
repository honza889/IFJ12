#include "value.h"

#include <stdlib.h>
#include <math.h>
#include "global.h"

#define BUFFERSIZE 128

/**
 * Konstruktor nedefinovane promenne
 */
Value newValueUndefined(){
 return (Value){
  .type=typeUndefined
 };
}

/**
 * Konstruktor Nil
 */
Value newValueNil(){
 return (Value){
  .type=typeNil
 };
}

/**
 * Konstruktor Booleovske promenne
 */
Value newValueBoolean( bool value ){
 return (Value){
  .type=typeBoolean,
  .data.boolean=value
 };
}

/**
 * Konstruktor ciselne promenne
 */
Value newValueNumeric( double value ){
 return (Value){
  .type=typeNumeric,
  .data.numeric=value
 };
}

/**
 * Konstruktor ukazatele na funkci
 */
Value newValueFunction( Function *value ){
 return (Value){
  .type=typeFunction,
  .data.function=value
 };
}

/**
 * Konstruktor retezce
 */
Value newValueString( char *value ){
 char *string = malloc(sizeof(char));
 string[0]='\0';
 return (Value){
  .type=typeString,
  .data.string=string
 };
}

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

bool getValueBoolean(Value *object){
 switch(object->type){
  
  case typeUndefined:
   fprintf(stderr,"\nError: Undefined variable!\n");
   exit(3);
  break;
  
  case typeNil:
   ERROR("Neni implementovano: getValueBoolean pro typeNil!");
  break;
  
  case typeBoolean:
   return object->data.boolean;
  break;
  
  case typeNumeric:
   if(object->data.numeric==0.0 || object->data.numeric==NAN){
    return false;
   }else{
    return true;
   }
  break;
  
  case typeFunction:
   ERROR("Neni implementovano: getValueBoolean pro typeFunction!");
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
 * Uvolnit obsah hodnoty
 */
void freeValue(Value *object){
 if(object->type==typeString && object->data.string){
  free(object->data.string);
 }
}

