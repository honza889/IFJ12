#ifndef IAL_H_INCLUDED
#define IAL_H_INCLUDED

#include "value.h"
#include "rcstring.h"


/**
*   author: Martin Fryc
*   date: 19.10.2012
*   proj IFJ-find
*
*   vyhledava podretezec v retezci
*   parametr ValueList data, obsahuje retezce (data[0]=retezec,
*   data[1]=hledany podretezec)
*   int count je tedy vzdy 2
*   vraci Value numeric
*   v pripade nalezeni vrati index prvku, kde zacina nalezeny podretezec
*             nenalezeni vrati delku retezce
**/
Value find( ValueList data, int count );

#endif // IAL_H_INCLUDED
