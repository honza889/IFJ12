#include "global.h"

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
 setSymbol("promenna1",&test);
 printf("(%s)",getGTstring(getSymbol("promenna1")));
 freeGTvalue(&test);
 
 printf("\n");
}

