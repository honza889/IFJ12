/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#ifndef VALUE_H
#define VALUE_H


#include <stdbool.h>
#include "global.h"
#include "ast.h"
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "semantics.h"
#include "exceptions.h"

//NECO
void validateFunction( Function* f );


#endif
