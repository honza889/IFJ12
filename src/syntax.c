#include <stdbool.h>
#include "global.h"
#include "syntax.h"

Token syntax(FILE *f){
	// TODO: Kontrola syntaxe - pokud selze, vyjimka
	return scanner(f);
}

