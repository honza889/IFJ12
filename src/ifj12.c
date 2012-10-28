#include <stdio.h>
#include "value.h"
#include "symbols.h"
#include "semantics.h"
#include "exceptions.h"
#include "global.h"

int main(int argc, char**argv)
{
	exceptions_init();
	
	if(argc!=2){
		fprintf(stderr, "Chybné volání interpretru! Použití:\n%s program.fal\n\n", argv[0]);
		exit(13);
	}
	
	FILE* f=fopen(argv[1],"r");
	if(f==NULL){
		fprintf(stderr, "Interpretovaný program se nepodařilo otevřít!\n\n");
		exit(13);
	}
	
	try{
		
		SymbolTable globalSymbolTable = {NULL,0};
		semantics(0,f,&globalSymbolTable);
		
	}catch{
		on(SyntaxError,row){
			fprintf(stderr, "\nSyntakticka chyba programu na radku %d!\n\n",*row);
            fclose( f );
			exit(2);
		}
        on( OutOfMemory, typename ){
            fprintf(stderr, "Nebylo mozne alokovat pamet pro typ '%s'", *typename );
            fclose( f );
            exit(99);
        }
		onAll{
			fprintf(stderr, "\nInterpretace skončila neodchycenou vyjímkou!\n\n");
            fclose( f );
			exit(99);
		}
	}
	
	fclose(f);	
	return 0;
}

