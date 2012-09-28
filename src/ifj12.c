#include "global.h"
#include "gentype.h"

int main(int argc, char**argv)
{
 printf("Hello world!\n");
 
 GenType test;
 test.value=NULL; // dulezite inicializovat!
 setGTnil(&test);
 printf("(%s)",getGTstring(&test));
 setGTbool(&test,true);
 printf("(%s)",getGTstring(&test));
 setGTbool(&test,false);
 printf("(%s)",getGTstring(&test));
 setGTint(&test,8);
 printf("(%s)",getGTstring(&test));
 setGTnumeric(&test,0.00034567891234567899);
 printf("(%s)",getGTstring(&test));
 setGTstring(&test,"abcd");
 printf("(%s)",getGTstring(&test));
 freeGTvalue(&test);
 
 printf("\n");
}

