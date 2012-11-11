#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "global.h"
#include "scanner.h"
#include "exceptions.h"

void initScanner( Scanner* scanner, FILE* file )
{
    scanner->first = 0;
    scanner->count = 1;
    scanner->file = file;
    scanner->current[scanner->first] = scan( file );
}

/*       _\|/_
         (o o)
 +----oOO-{_}-OOo------------------------------------------+
 | Následují funkce nad bufferem o velikosti SCAN_BUF_CAP. |
 +--------------------------------------------------------*/

/*********************************/
/* Fce nad prvním prvkem fronty. */
/*********************************/
Token getTok( Scanner* scanner )
{
    assert(scanner->count > 0);
    return scanner->current[scanner->first];
}

void consumeTok( Scanner* scanner )
{
    consumeTokN(scanner, 1);
}

Token expectTok( Scanner* scanner, TokenType tok )
{
    assert(scanner->count > 0);
    Token current = testTok( scanner, tok );
    consumeTok( scanner );
    return current;
}

Token testTok( Scanner* scanner, TokenType tok )
{
    assert(scanner->count > 0);
    if( ! ( getTok( scanner ).type & tok ) )
    {
        // BUG: Může způsobit leak!
        throw( UnexpectedToken, ((UnexpectedTokenException){ 
            .expected = tok, 
            .got = getTok( scanner ).type,
            .line_num = getTok( scanner ).line_num
        }));
    }
    
    Token theRealToken = getTok( scanner );
        return theRealToken;
}

void expectKeyw( Scanner* scanner, KeyWord keyw )
{
    testKeyw( scanner, keyw );
    consumeTok( scanner );
}

void testKeyw( Scanner* scanner, KeyWord keyw )
{
    if( getTok( scanner ).type == tokKeyW )
    {
        if( ! ( getTok( scanner ).data.keyw & keyw ) )
        {
            throw( UnexpectedKeyWord, ((UnexpectedKeyWordException){ 
                .expected = keyw, 
                .got = getTok( scanner ).data.keyw,
                .line_num = getTok( scanner ).line_num
            }));
        }
    }
    else
    {
        // BUG: Může způsobit leak!
        throw( UnexpectedToken, ((UnexpectedTokenException){
            .expected = tokKeyW, 
            .got = getTok( scanner ).type,
            .line_num = getTok( scanner ).line_num
        }));
    }
}

/**********************************************/
/* Fce nad bufferem o velikosti SCAN_BUF_CAP. */
/**********************************************/
Token getTokN( Scanner* scanner, unsigned index )
{
    unsigned new_index = index;
    assert(scanner->count > 0);
    // Pokud je prvek o daném indexu již načten, tak se jen vrátí token.
    if (index+1 <= scanner->count) {
        new_index = (scanner->first + index) % SCAN_BUF_CAP;
    }
    // Jinak se prvek o daném indexu načte, pokud to kapacita bufferu dovolí.
    else {
        assert(index < SCAN_BUF_CAP);
        new_index = scanner->count;
        while (scanner->count <= index) {	// Načti do bufferu prvky od posledního po index (včetně).
          new_index = (scanner->first + scanner->count) % SCAN_BUF_CAP;
          scanner->current[new_index] = scan( scanner->file );
          scanner->count++;
        }
    }
    
    return scanner->current[new_index];
}

void consumeTokN( Scanner* scanner, unsigned N )
{
    // Počet prvků, které chci zkonzumovat, musí být větší nule a menší nebo rovno počtu načtených prvků.
    assert(N > 0 && N <= scanner->count);
    for (unsigned i=0; i<N; i++) {
      Token *deletedTok = &scanner->current[scanner->first];
      if (deletedTok->type == tokId)
        deleteRCString( &deletedTok->data.id );
      else if (deletedTok->type == tokLiteral && deletedTok->data.val.type == typeString)
        deleteRCString( &deletedTok->data.val.data.string );
      scanner->first = (scanner->first + 1) % SCAN_BUF_CAP;
    }
    scanner->count -= N;
    // Musím zajistit aby operace nad tokeny měly připravený další token.
    // Aneb kdo sní poslední kousek kupuje další :-D
    if (scanner->count == 0) {
        scanner->count++;
        scanner->current[scanner->first] = scan( scanner->file );
    }
}

Token expectNextTok( Scanner* scanner, TokenType tok )
{
    assert(scanner->count > 0);
    // Načtu nový token za poslední token v bufferu.
    Token current = testTokN( scanner, tok, scanner->count );
    // Zkonzumuju onen nový token.
    scanner->count--;
    return current;
}

Token testTokN( Scanner* scanner, TokenType tok, unsigned index )
{
    assert(scanner->count > 0);
    Token current = getTokN( scanner, index );
    
    if( ! ( current.type & tok ) )
    {
        // BUG: Může způsobit leak!
        throw( UnexpectedToken, ((UnexpectedTokenException){ 
            .expected = tok, 
            .got = current.type,
            .line_num = current.line_num
        }));
    }
    
    return current;
}

void UnexpectedTokenPrint(UnexpectedTokenException e) {
  fprintf(stderr,"Parse error: Predpokladany token je ");
  for (int i=1, j=1; i <= tokEndOfFile; i<<=1) {
    switch (e.expected & i) {
      case tokId:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"identifikator"); break;
      case tokLiteral:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"literal"); break;
      case tokOp:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"operator"); break;
      case tokKeyW:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"klicove slovo"); break;
      case tokAssign:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"znak prirazeni '='"); break;
      case tokLParen:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"leva zavorka '('"); break;
      case tokRParen:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"prava zavorka ')'"); break;
      case tokLBracket:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"leva hranata zavorka '['"); break;
      case tokRBracket:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"prava hranata zavorka ']'"); break;
      case tokColon:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"dvojtecka ':'"); break;
      case tokComma:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"carka ','"); break;
      case tokEndOfLine:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"konec radku '\\n'"); break;
      case tokEndOfFile:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"konec souboru (EOF)"); break;
    }
  }
  fprintf(stderr,", ale ziskany token je ");
  switch (e.got) {
    case tokId:		fprintf(stderr,"identifikator"); break;
    case tokLiteral:	fprintf(stderr,"literal"); break;
    case tokOp:		fprintf(stderr,"operator"); break;
    case tokKeyW:	fprintf(stderr,"klicove slovo"); break;
    case tokAssign:	fprintf(stderr,"znak prirazeni '='"); break;
    case tokLParen:	fprintf(stderr,"leva zavorka '('"); break;
    case tokRParen:	fprintf(stderr,"prava zavorka ')'"); break;
    case tokLBracket:	fprintf(stderr,"leva hranata zavorka '['"); break;
    case tokRBracket:	fprintf(stderr,"prava hranata zavorka ']'"); break;
    case tokColon:	fprintf(stderr,"dvojtecka ':'"); break;
    case tokComma:	fprintf(stderr,"carka ','"); break;
    case tokEndOfLine:	fprintf(stderr,"konec radku '\\n'"); break;
    case tokEndOfFile:	fprintf(stderr,"necekany konec souboru (EOF)"); break;
  }
  fprintf(stderr," na radku %d.\n", e.line_num);
}

void UnexpectedKeyWordPrint(UnexpectedKeyWordException e) {
  fprintf(stderr,"Parse error: Predpokladane klicove slovo je ");
  for (int i=1, j=1; i <= kWhile; i<<=1) {
    switch (e.expected & i) {
      case kElse:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"else\""); break;
      case kEnd:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"end\""); break;
      case kFunction:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"function\""); break;
      case kIf:		if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"if\""); break;
      case kReturn:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"return\""); break;
      case kWhile:	if (j++>1) {fprintf(stderr," nebo ");} fprintf(stderr,"\"while\""); break;
    }
  }
  fprintf(stderr,", ale ziskane klicove slovo je ");
  switch (e.got) {
    case kElse:		fprintf(stderr,"\"else\""); break;
    case kEnd:		fprintf(stderr,"\"end\""); break;
    case kFunction:	fprintf(stderr,"\"function\""); break;
    case kIf:		fprintf(stderr,"\"if\""); break;
    case kReturn:	fprintf(stderr,"\"return\""); break;
    case kWhile:	fprintf(stderr,"\"while\""); break;
  }
  fprintf(stderr," na radku %d.\n", e.line_num);
}

/*       _\|/_
         (o o)
 +----oOO-{_}-OOo--------------+
 | Následují skenovací funkce. |
 +----------------------------*/
/**
 * Funkce má za cíl detekovat, zda-li last_letter je znak oprerátoru, pokud ne, pak detekuje a
 * přeskočí komentář, detekuje token přiřazení (=) a závorky ('(',')','[',']').
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	token		Ukazatel na strukturu Token.
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @param[out]	line_num	Ukazatel na počítadlo řádků vstupního zdrojového kódu.
 * @param[out]	lexeme		Ukazatel na RCString (!= NULL). Když dojde k výjimce, tak abych ho mohl uvolnit.
 * @return Vrací (bool) true, když uspěje (naplní \a token), jinak false.
 */
bool FSM(FILE *f, Token *token, char *last_letter, unsigned *line_num, RCString *lexeme)
{
  assert(lexeme != NULL);
  
  switch (*last_letter) {
    case '/':
      *last_letter = getc(f);
      if (*last_letter == '/')
      {
        while (*last_letter != '\n' && *last_letter != EOF)
          *last_letter = getc(f);
        // Nahlášení detekce EOF nebo navýšení počítadla řádků nechám na fci scanner()
        return false;	// narazilo se na konec řádkového komentáře, token však naplněn nebyl
      }
      else if (*last_letter == '*')
      {
        unsigned start_line_num = *line_num;	// Pomocná proměnná, která si pamatuje řádek s počátkem blokového komentáře.
        
        while (*last_letter != EOF) {
          *last_letter = getc(f);
          if (*last_letter == '\n') (*line_num)++;
          else if (*last_letter == '*' && (*last_letter = getc(f)) == '/') {
            *last_letter = getc(f);
            return false;	// narazilo se na konec blokového komentáře, token však naplněn nebyl
          }
        }
        // Narazilo se na EOF -> error: unterminated comment
        deleteRCString(lexeme);
        // .line_num zde obsahuje počátek neukončeného blokového komentáře
        throw(ScannerError,((ScannerErrorException){.type=UnterminatedComment, .line_num=start_line_num}));
      }
      else
      {
        token->type = tokOp;
        token->data.op = opDivide;
        token->line_num = *line_num;
      }
      break;
      
    case '*':
      *last_letter = getc(f);
      if (*last_letter == '*')
      {
        token->type = tokOp;
        token->data.op = opPower;
        token->line_num = *line_num;
        *last_letter = getc(f);
      }
      else
      {
        token->type = tokOp;
        token->data.op = opMultiple;
        token->line_num = *line_num;
      }
      break;
      
    case '+':
      token->type = tokOp;
      token->data.op = opPlus;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case '-':
      token->type = tokOp;
      token->data.op = opMinus;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case '<':
      *last_letter = getc(f);
      if (*last_letter == '=')
      {
        token->type = tokOp;
        token->data.op = opLE;
        token->line_num = *line_num;
        *last_letter = getc(f);
      }
      else
      {
        token->type = tokOp;
        token->data.op = opLT;
        token->line_num = *line_num;
      }
      break;
      
    case '>':
      *last_letter = getc(f);
      if (*last_letter == '=')
      {
        token->type = tokOp;
        token->data.op = opGE;
        token->line_num = *line_num;
        *last_letter = getc(f);
      }
      else
      {
        token->type = tokOp;
        token->data.op = opGT;
        token->line_num = *line_num;
      }
      break;
      
    case '!':
      *last_letter = getc(f);
      if (*last_letter == '=')
      {
        token->type = tokOp;
        token->data.op = opNE;
        token->line_num = *line_num;
        *last_letter = getc(f);
      }
      else
      {
        deleteRCString(lexeme);
        throw(ScannerError,((ScannerErrorException){.type=InvalidToken, .line_num=*line_num}));
      }
      break;
      
    case '=':
      *last_letter = getc(f);
      if (*last_letter == '=')
      {
        token->type = tokOp;
        token->data.op = opEQ;
        token->line_num = *line_num;
        *last_letter = getc(f);
      }
      else
      {
        token->type = tokAssign;
        token->line_num = *line_num;
      }
      break;
      
    case '(':
      token->type = tokLParen;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case ')':
      token->type = tokRParen;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case '[':
      token->type = tokLBracket;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case ']':
      token->type = tokRBracket;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case ':':
      token->type = tokColon;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    case ',':
      token->type = tokComma;
      token->line_num = *line_num;
      *last_letter = getc(f);
      break;
      
    default:
      deleteRCString(lexeme);
      throw(ScannerError,((ScannerErrorException){.type=InvalidToken, .line_num=*line_num}));
  } // Konec přepínače switch (*last_letter).
  return true;
}

/**
 * Funkce má za cíl detekovat, escape sekvenci a přidat jí reprezentovaný znak.
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @param[out]	lexeme		Ukazatel na RCString (!= NULL).
 * @param[in]	line_num	Hodnota aktuálního řádku vstupního zdrojového kódu pro vyhození výjimky.
 * @return Vrací (bool) true, když uspěje (přidá písmeno do \a lexeme), jinak false.
 */
bool det_esc_sequence(FILE *f, char *last_letter, RCString *lexeme, unsigned line_num)
{
  assert(lexeme != NULL);
  assert(UCHAR_MAX >= 0xFF);	// V této fci předpokládám, že char má min. 8 bitů.
  
  const unsigned LEN_HEX_CHAR = 3;	// Délka řetězce, do kterého se uloží dvě hexadecimální cifry a ukončení '\0'.
  char hex_char[LEN_HEX_CHAR];		// Řetězec pro dvě hexadecimální cifry a ukončení '\0'.
  char *endptr = NULL;			// Ukazatel na písmeno kde přestala číst funkce strtol().
  char conv_num;			// Proměnná do které se uloží převedené číslo reprezentující znak.
  
  // Následuje detekce escape sekvencí (\n, \t, \\, \", \xFF, \x0a), které jsou v řetězci a jejich převod.
  switch (*last_letter = getc(f)) {
    case 'n':
      RCStringAppendChar(lexeme, '\n');
      break;
    case 't':
      RCStringAppendChar(lexeme, '\t');
      break;
    case '\\':
      RCStringAppendChar(lexeme, '\\');
      break;
    case '"':
      RCStringAppendChar(lexeme, '"');
      break;
    case 'x':
      // Načtení řetězce jenž obsahuje hexadecimální číslo reprezentující znak.
      for (int i=0; (i < LEN_HEX_CHAR-1); i++) {
        if ((*last_letter = getc(f)) == EOF) {
          deleteRCString(lexeme);
          throw(ScannerError,((ScannerErrorException){.type=UnterminatedString, .line_num=line_num}));
        }
        hex_char[i] = *last_letter;
      }
      hex_char[LEN_HEX_CHAR-1] = '\0';		// Nakonec řetězce musí přijít '\0'
      
      // Převod řetězce na hexadecimální číslo reprezentující znak.
      conv_num = strtol(hex_char, &endptr, 16);	// Předpokládám, že char má min. 8 bitů (viz assert() výše).
      if (errno != 0 || *endptr != '\0') {
        deleteRCString(lexeme);
        throw(ScannerError,((ScannerErrorException){.type=InvalidNumericLiteral, .line_num=line_num}));
      }
      RCStringAppendChar(lexeme, conv_num);
      break;
    case EOF:
      deleteRCString(lexeme);
      // error: missing terminating " character
      throw(ScannerError,((ScannerErrorException){.type=UnterminatedString, .line_num=line_num}));
      break;
    default:
      return false;
  }
  *last_letter = getc(f);
  return true;
}

/**
 * Funkce má za cíl detekovat, zda-li řetězec uložený v lexeme je klíčové slovo.
 * @param[in]	lexeme		RCString obsahující řetězec pro porovnání.
 * @param[out]	token		Ukazatel na strukturu Token.
 * @param[in]	line_num	Hodnota aktuálního řádku vstupního zdrojového kódu pro vyhození výjimky.
 * @return Vrací (bool) true, když uspěje (detekuje klíčové slovo a uloží do \a token), jinak false.
 */
bool det_key_word(RCString lexeme, Token *token, unsigned line_num)
{
  RCString cmp_lexeme = copyRCString(&lexeme);
  RCStringAppendChar(&cmp_lexeme, '\0');	// Přidán znak '\0' na konec RCStringu pro fci strcmp().
  bool result = true;				// Proměnná s návratovou hodnotou.
  
  token->type = tokKeyW;	// Nejprve zjistím jestli je token z výčtu klíčových slov.
  
  if (! strcmp("else", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kElse;
  else if (! strcmp("end", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kEnd;
  else if (! strcmp("function", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kFunction;
  else if (! strcmp("if", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kIf;
  else if (! strcmp("return", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kReturn;
  else if (! strcmp("while", RCStringGetBuffer(&cmp_lexeme)))
    token->data.keyw = kWhile;
  else {
    
    token->type = tokLiteral;	// Poté se dívám na klíčová slova mimo výčet.
    
    if (! strcmp("true", RCStringGetBuffer(&cmp_lexeme)))
      token->data.val = newValueBoolean(true);
    else if (! strcmp("false", RCStringGetBuffer(&cmp_lexeme)))
      token->data.val = newValueBoolean(false);
    else if (! strcmp("nil", RCStringGetBuffer(&cmp_lexeme)))
      token->data.val = newValueNil();
    else {
      if (! strcmp("as", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("def", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("directive", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("export", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("from", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("import", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("launch", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("load", RCStringGetBuffer(&cmp_lexeme))	||
          ! strcmp("macro", RCStringGetBuffer(&cmp_lexeme))	)
      {
        deleteRCString(&cmp_lexeme);
        throw(ScannerError,((ScannerErrorException){.type=UndefinedKeyword, .line_num=line_num}));
      }
      result = false;
    }
  }
  token->line_num = line_num;
  
  deleteRCString(&cmp_lexeme);
  return result;
}

/**
 * Načte lexém ze souboru.
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @param[out]	lexeme		Ukazatel na RCString (!= NULL).
 * @return Vrací (bool) true, když uspěje (přidá písmeno do \a lexeme), jinak false.
 */
bool add_char(FILE *f, char *last_letter, RCString *lexeme)
{
  assert(lexeme != NULL);
  
  if (*last_letter == EOF)
    return false;
  
  // Načítání znaků do bufferu
  RCStringAppendChar(lexeme, *last_letter);// Přidání znaku do řetězce.
  *last_letter = getc(f);
  
  return true;
}
/**
 * Načte literál číslo podle definice ifj12.
 * Pokud se nepodaří načíst číslo vyhodí výjimku InvalidNumericLiteral.
 * @param[in]	f		Ukazatel na otevřený soubor.
 * @param[out]	token		Ukazatel na strukturu Token.
 * @param[out]	last_letter	Poslední načtený znak ze zdrojového kódu.
 * @param[in]	line_num	Hodnota aktuálního řádku vstupního zdrojového kódu pro vyhození výjimky.
 * @param[out]	lexeme		Ukazatel na RCString (!= NULL).
 */
void load_number(FILE *f, Token *token, char *last_letter, unsigned line_num, RCString *lexeme)
{
  assert(isdigit(*last_letter));
  
  bool correct = false;	// pomocná proměnná pro určení zda-li je číslo správně zapsáno
  double conv_num;	// proměnná do které se uloží převedené číslo
  char *endptr = NULL;	// ukazatel na písmeno kde přestala číst funkce strtod()
  
  do {
    add_char(f, last_letter, lexeme);	// Přidám do lexému číslici 0-9.
    if (*last_letter == '.') {
      add_char(f, last_letter, lexeme);	// Přidám do lexému řádovou tečku.
      if (isdigit(*last_letter)) {	// Po tečce musí následovat číslo.
        correct = true;			// Pokud ano, zápis je správný.
        do {
          add_char(f, last_letter, lexeme);	// Přidám do desetiné části lexému číslici 0-9.
          if (*last_letter == 'e') {		// Po desetiném čísle může následovat exponent
            add_char(f, last_letter, lexeme);
            // Ihned znaku 'e' se může vyskytnout číslice, znak '+' nebo '-'.
            if (! (isdigit(*last_letter) || *last_letter == '+' || *last_letter == '-')) {
              correct = false;		// Pokud ne, zápis je špatný.
              break;
            }
            do {
              add_char(f, last_letter, lexeme);	// Přidám do exponenciální části lexému číslici 0-9.
            } while (isdigit(*last_letter));
          }
        } while (isdigit(*last_letter));
      }
      break;
    }
    else if (*last_letter == 'e') {		// Po celém čísle může následovat exponent.
      correct = true;				// Pokud ano, zápis je správný.
      do {
        add_char(f, last_letter, lexeme);	// Přidám do exponenciální části lexému číslici 0-9.
      } while (isdigit(*last_letter));
      break;
    }
  } while (isdigit(*last_letter));
  
  // Pokud číslo nebylo zapsáno podle ifj12, hodím výjimku.
  if (!correct) {
    deleteRCString(lexeme);
    throw(ScannerError,((ScannerErrorException){.type=InvalidNumericLiteral, .line_num=line_num}));
  }

  RCStringAppendChar(lexeme, '\0');	// Přidán znak '\0' na konec RCStringu pro fci strtod().

  conv_num = strtod(RCStringGetBuffer(lexeme), &endptr);
  if (errno == 0 && *endptr == '\0') {
    token->type = tokLiteral;
    token->data.val = newValueNumeric( conv_num );
    token->line_num = line_num;
  }
  else {
    deleteRCString(lexeme);
    throw(ScannerError,((ScannerErrorException){.type=InvalidNumericLiteral, .line_num=line_num}));
  }
}

/**
 * Načte lexém, zpracuje a vrátí token.
 * @param[in]	f	Ukazatel na otevřený soubor.
 * @return Vrací token.
 */
Token scan(FILE *f)
{
  Token token;
  RCString lexeme = makeEmptyRCString();	/** Načtený lexém. */
  static char last_letter = '\t';		/** Poslední načtený znak ze zdrojového kódu. */
  static unsigned line_num = 1;			/** Počítadlo řádků vstupního zdrojového kódu. */
  
  int keyw;
  
  bool repeat;		// true pokud se má čtení restartovat, protože se nic nenačetlo (byl komentář)
  
  // začátek do-while(repeat)
  // Cyklí dokud se nenaplní token (korektní EOF je taky token).
  do {
    repeat = false;
    
    // přeskočení bílých znaků
    while (isspace(last_letter)) {
      if (last_letter == '\n') break;
      last_letter = getc(f);
    }
    
    // následuje dlouhý přepínač if-else
    if (last_letter == EOF) {
      token.type = tokEndOfFile;
      token.line_num = line_num;
    }
    else if (last_letter == '\n'){
      token.type = tokEndOfLine;
      token.line_num = line_num++;
      last_letter = getc(f);
    }
    else if (isalpha(last_letter) || last_letter == '_') {
      do {
        add_char(f, &last_letter, &lexeme);
      } while (isalnum(last_letter) || last_letter == '_');
      
      // Pokud se nedetekuje (a neuloží do tokenu) klíčové slovo, pak je to identifikátor.
      if (! det_key_word(lexeme, &token, line_num)) {
        token.type = tokId;
        token.data.id = copyRCString(&lexeme);
        token.line_num = line_num;
      }
    }
    else if (isdigit(last_letter)) {
      load_number(f, &token, &last_letter, line_num, &lexeme);
    }
    else if (last_letter == '"') {
      unsigned start_line_num = line_num;	// Pomocná proměnná, která si pamatuje řádek s počátkem řetězce.
      
      last_letter = getc(f);
      while (last_letter != '"') {
        // Pokud add_char neuspěje narazilo se na EOF.
        if (! add_char(f, &last_letter, &lexeme)) {
          deleteRCString(&lexeme);
          // error: missing terminating " character
          throw(ScannerError,((ScannerErrorException){.type=UnterminatedString, .line_num=line_num}));
        }
        
        // Pokud je načtené písmeno '\n' inkrementuje se počítadlo řádků.
        if (last_letter == '\n')
          line_num++;
        
        // Pokud je načtené písmeno escape sekvence převede se na ní reprezentující znak.
        if (last_letter == '\\') {
          // Pokud det_esc_sequence neuspěje narazilo se na špatnou escape sekvenci.
          if (! det_esc_sequence(f, &last_letter, &lexeme, line_num)) {
            deleteRCString(&lexeme);
            // .line_num zde obsahuje počátek neukončeného řetězce
            throw(ScannerError,((ScannerErrorException){.type=BadEscSequence, .line_num=start_line_num}));
          }
        }
      }
      
      // Řetězec se uloží do tokenu.
      token.type = tokLiteral;
      token.data.val = newValueString(lexeme);
      token.line_num = line_num;
      last_letter = getc(f);
    }
    else {
      repeat = ! FSM(f, &token, &last_letter, &line_num, &lexeme);
    } // Konec dlouhého přepínače if-else.
  } while (repeat);
  
  deleteRCString(&lexeme);
  return token;
}

void scannerErrorPrint(ScannerErrorException e) {
  switch (e.type) {
    case InvalidNumericLiteral:	fprintf(stderr,"Scan error: Chybne zadane cislo"); break;
    case InvalidToken:		fprintf(stderr,"Scan error: Nesmyslny lexem"); break;
    case UndefinedKeyword:	fprintf(stderr,"Scan error: Klicove slovo s nedefinovanym vyznamem"); break;
    case UnterminatedComment:	fprintf(stderr,"Scan error: Neukonceny komentar"); break;
    case UnterminatedString:	fprintf(stderr,"Scan error: Neukonceny retezec"); break;
    case BadEscSequence:	fprintf(stderr,"Scan error: Chybna escapovaci sekvence v retezci"); break;
  }
  fprintf(stderr," na radku %d.\n", e.line_num);
}
