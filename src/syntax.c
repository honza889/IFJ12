#include <stdbool.h>
#include "global.h"
#include "syntax.h"
#include "exceptions.h"

Token syntax(FILE *f){
	// TODO: Kontrola syntaxe - pokud selze, vyjimka
	//throw(SyntaxError,15);
	return scanner(f);
}

