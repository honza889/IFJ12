#include "global.h"

int main(int argc, char**argv)
{
 
 // Vytvoreni promennych:
 setGTstring(setSymbol("jablko"),"Jabloň");
 setGTstring(setSymbol("hruska"),"Hrušeň");
 setGTstring(setSymbol("tresne"),"Třešeň");
 
 // Trivialni ziskani obsahu promennych: (vysledek getGTstring neni uvolnovan free()!)
 printf("(%s)\n",getGTstring(getSymbol("jablko")));
 printf("(%s)\n",getGTstring(getSymbol("hruska")));
 printf("(%s)\n",getGTstring(getSymbol("tresne")));
 
 // Prepis promenne ala PHP:
 setGTstring(setSymbol("hruska"),"Jiná Hrušeň");
 setGTint(setSymbol("merunka"),125);
 
 // Opet vypis pro srovnani:
 printf("\n");
 printf("(%s)\n",getGTstring(getSymbol("jablko")));
 printf("(%s)\n",getGTstring(getSymbol("hruska")));
 printf("(%s)\n",getGTstring(getSymbol("tresne")));
 printf("(%s)\n",getGTstring(getSymbol("merunka")));
 
}

