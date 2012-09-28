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
 //setGTint(&test,8);
 freeGTvalue(&test);
 
 printf("\n");
}

