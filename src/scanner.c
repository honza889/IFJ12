#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include "global.h"
#include "scanner.h"
#include "value.h"
#include "rcstring.h"
#include "exceptions.h"

#define rcstring2str(rcstring) (rcstring).buffer->string

/**
 * Funkce má za cíl detekovat, zda-li last_letter je znak oprerátoru, pokud ne, pak detekuje a
 * přeskočí komentář, detekuje token přiřazení (=) a závorky ('(',')','[',']').
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	token		Ukazatel na strukturu Token.
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @param[out]	line_num	Ukazatel na počítadlo řádků vstupního zdrojového kódu.
 * @return Jestli byl načten token (jinak komentář -> restartovat načítání)
 */
bool FSM(FILE *f, Token *token, char *last_letter, unsigned *line_num)
{
	switch (*last_letter) {
		case '/':
			*last_letter = getc(f);
			if (*last_letter == '/')
			{
				while (*last_letter != '\n' && *last_letter != EOF){
					*last_letter = getc(f);
					if (*last_letter == '\n')
						*line_num++;
				}
				return false;	// Nahlášení detekce EOF nechám na fci scanner()
			}
			else if (*last_letter == '*')
			{
				while (*last_letter != EOF) {
					*last_letter = getc(f);
					if (*last_letter == '\n') line_num++;
					else if (*last_letter == '*' && (*last_letter = getc(f)) == '/') {
						*last_letter = getc(f);
						return true;
					}
				}	// TODO: Nahlásit, když komentář nebude uzavřený?
				return true;	// Nahlášení detekce EOF nechám na fci scanner()
			}
			else
			{
				token->type = tokOp;
				token->data.op = opDivide;
			}
			break;
			
		case '*':
			*last_letter = getc(f);
			if (*last_letter == '*')
			{
				token->type = tokOp;
				token->data.op = opPower;
				*last_letter = getc(f);
			}
			else
			{
				token->type = tokOp;
				token->data.op = opMultiple;
			}
			break;
			
		case '+':
			token->type = tokOp;
			token->data.op = opPlus;
			break;
			
		case '-':
			token->type = tokOp;
			token->data.op = opMinus;
			break;
			
		case '<':
			*last_letter = getc(f);
			if (*last_letter == '=')
			{
				token->type = tokOp;
				token->data.op = opLE;
				*last_letter = getc(f);
			}
			else
			{
				token->type = tokOp;
				token->data.op = opLT;
			}
			break;
			
		case '>':
			*last_letter = getc(f);
			if (*last_letter == '=')
			{
				token->type = tokOp;
				token->data.op = opGE;
				*last_letter = getc(f);
			}
			else
			{
				token->type = tokOp;
				token->data.op = opGT;
			}
			break;
			
		case '!':
			*last_letter = getc(f);
			if (*last_letter == '=')
			{
				token->type = tokOp;
				token->data.op = opNE;
				*last_letter = getc(f);
			}
			else
			{
				throw(ScannedBadLexeme,*line_num);
			}
			break;
			
		case '=':
			*last_letter = getc(f);
			if (*last_letter == '=')
			{
				token->type = tokOp;
				token->data.op = opEQ;
				*last_letter = getc(f);
			}
			else
			{
				token->type = tokAssign;
			}
			break;
			
		case '(':
			token->type = tokLParen;
			*last_letter = getc(f);
			break;
			
		case ')':
			token->type = tokRParen;
			*last_letter = getc(f);
			break;
			
		case '[':
			token->type = tokLBracket;
			break;
			
		case ']':
			token->type = tokRBracket;
			break;
			
		default:
			throw(ScannedBadLexeme,*line_num);
	}
	return true;
}

/**
 * Funkce má za cíl detekovat, zda-li řetězec uložený v lexeme je klíčové slovo.
 * @param[in]	lexeme	RCString obsahující řetězec pro porovnání.
 * @return Vrací chybový kód.
 */
int cmp_key_words(RCString lexeme)
{
	RCString cmp_lexeme = copyRCString(&lexeme);
	RCStringAppendChar(&cmp_lexeme, '\0');	// Přidán znak '\0' na konec RCStringu pro fci strcmp().
	enum {					/** Klíčová slova */
		kElse, kEnd, kFunction, kIf,
		kReturn, kWhile, NOTKEYW = -1
	} keyw;
	
	if (! strcmp("else", rcstring2str(cmp_lexeme)))
		keyw = kElse;
	else if (! strcmp("end", rcstring2str(cmp_lexeme)))
		keyw = kEnd;
	else if (! strcmp("function", rcstring2str(cmp_lexeme)))
		keyw = kFunction;
	else if (! strcmp("if", rcstring2str(cmp_lexeme)))
		keyw = kIf;
	else if (! strcmp("return", rcstring2str(cmp_lexeme)))
		keyw = kReturn;
	else if (! strcmp("while", rcstring2str(cmp_lexeme)))
		keyw = kWhile;
	else
		keyw = NOTKEYW;
	
	deleteRCString(&cmp_lexeme);
	return keyw;
}

/**
 * Načte lexém ze souboru.
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	lexeme		Ukazatel na RCString (!= NULL).
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @return Vrací chybový kód.
 */
int add_char(FILE *f, char *last_letter, RCString *lexeme)
{
	assert(lexeme != NULL);
	
	if (*last_letter == EOF)
		return EOF;
	
	// Načítání znaků do bufferu
	RCStringAppendChar(lexeme, *last_letter);// Přidání znaku do řetězce.
	*last_letter = getc(f);
	return EXIT_SUCCESS;
}

/**
 * Načte lexém, zpracuje a vrátí token v parametru.
 * @param[in]	f	Ukazatel na otevřený soubor.
 * @param[out]	token	Ukazatel na strukturu Token.
 * @return Vrací chybový kód.
 */
Token scanner(FILE *f)
{
	Token token;
	RCString lexeme = makeEmptyRCString();	/** Načtený lexém. */
	static char last_letter = '\t';		/** Poslední načtený znak ze zdrojového kódu. */
	static unsigned line_num = 1;			/** Počítadlo řádků vstupního zdrojového kódu. */
	
	int keyw;
	
	double conv_num;	// proměnná do které se uloží převedené číslo
	char *endptr = NULL;	// ukazatel na písmeno kde přestala číst funkce strtod()
	
	bool repeat; // true pokud se ma cteni restartovat, protoze se nic nenacetlo (byl komentar)
	
	do{
	repeat = false;
	
	while (isspace(last_letter)) {
		if (last_letter == '\n')
			line_num++;
		last_letter = getc(f);
	}
	
	if (last_letter == EOF){
		deleteRCString(&lexeme);
		return (Token){.type=tokEndOfFile};
	}
	
	if (isalpha(last_letter)) {
		do {
			add_char(f, &last_letter, &lexeme);
		} while (isalnum(last_letter));
		
		if ((keyw = cmp_key_words(lexeme)) != NOTKEYW) {
			token.type = tokKeyW;
			token.data.keyw = keyw;
		}
		else {
			token.type = tokId;
			token.data.id = copyRCString(&lexeme);
		}
	}
	else if (isdigit(last_letter)) {
		do {
			add_char(f, &last_letter, &lexeme);
			if (last_letter == '.' || last_letter == 'e') {	// TODO: ošetřit, abych nenačítal řetězec
				do {
					add_char(f, &last_letter, &lexeme);
				} while (isdigit(last_letter));
				break;
			}
		} while (isdigit(last_letter));
		
		RCStringAppendChar(&lexeme, '\0');	// Přidán znak '\0' na konec RCStringu pro fci strtod().
		
		conv_num = strtod(rcstring2str(lexeme), &endptr);
		if (errno == 0 && *endptr == '\0') {
			token.type = tokNum;
			token.data.val.type = typeNumeric;
			token.data.val.data.numeric = conv_num;
		}
		else {
			deleteRCString(&lexeme);
			throw(ScannedBadNumber,line_num);
		}
	}
	else if (last_letter == '"') {
		last_letter = getc(f);
		while (last_letter != '"'){
			add_char(f, &last_letter, &lexeme);
			if (last_letter == '\n')
				line_num++;
		// TODO: Detekovat backslash-nuté znaky (\n, \t, \\, \", \xFF, \x0a), které jsou v řetězci?
		}
		token.type = tokString;
		token.data.string = copyRCString(&lexeme);
		last_letter = getc(f);
	}
	else {
		repeat = ! FSM(f, &token, &last_letter, &line_num);
	}
	}while(repeat);
	deleteRCString(&lexeme);
	return token;
}

