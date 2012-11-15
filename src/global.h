/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(message) \
  fprintf(stderr,"%s (%s:%s:%d)\n",(message),__FILE__,__FUNCTION__,__LINE__)

#define MALLCHECK(pointer) \
  if((pointer)==NULL){ \
    ERROR("Nepodarilo se alokovat pamet!"); \
    exit(99); \
  }

#define NEWSTRING(pointer,string) \
  pointer=(char*)malloc(sizeof(string)); \
  MALLCHECK(pointer); \
  memcpy(pointer,string,sizeof(string));

static inline int minInt( int a, int b )
{
  return a < b ? a : b;
}

static inline int maxInt( int a, int b )
{
  return a > b ? a : b;
}

#endif

