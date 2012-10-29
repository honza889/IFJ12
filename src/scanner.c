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
    scanner->file = file;
    scanner->current = scan( file );
}

Token getTok( Scanner* scanner )
{
    return scanner->current;
}

void consumeTok( Scanner* scanner )
{
    scanner->current = scan( scanner->file );
}

Token expectTok( Scanner* scanner, TokenType tok )
{
    Token current = testTok( scanner, tok );
    consumeTok( scanner );
    return current;
}

Token testTok( Scanner* scanner, TokenType tok )
{
    if( ! ( getTok( scanner ).type & tok ) )
    {
        throw( UnexpectedToken, ((UnexpectedTokenException){ 
            .expected = tok, 
            .got = getTok( scanner ).type 
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
        if( getTok( scanner ).data.keyw != keyw )
        {
            throw( UnexpectedKeyWord, ((UnexpectedKeyWordException){ 
                .expected = keyw, 
                .got = getTok( scanner ).data.keyw 
            }));
        }
    }
    else
    {
        throw( UnexpectedToken, ((UnexpectedTokenException){
            .expected = tokKeyW, 
            .got = getTok( scanner ).type 
        }));
    }
}

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
      *last_letter = getc(f);
      break;
      
    case '-':
      token->type = tokOp;
      token->data.op = opMinus;
      *last_letter = getc(f);
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
      *last_letter = getc(f);
      break;
      
    case ']':
      token->type = tokRBracket;
      *last_letter = getc(f);
      break;
      
    case ':':
      token->type = tokColon;
      *last_letter = getc(f);
      break;
      
    case ',':
      token->type = tokComma;
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
 * @param[out]	line_num	Ukazatel na počítadlo řádků vstupního zdrojového kódu.
 * @return Vrací (bool) true, když uspěje (přidá písmeno do \a lexeme), jinak false.
 */
bool det_esc_sequence(FILE *f, char *last_letter, RCString *lexeme, unsigned *line_num)
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
          throw(ScannerError,((ScannerErrorException){.type=UnterminatedString, .line_num=*line_num}));
        }
        hex_char[i] = *last_letter;
      }
      hex_char[LEN_HEX_CHAR-1] = '\0';		// Nakonec řetězce musí přijít '\0'
      
      // Převod řetězce na hexadecimální číslo reprezentující znak.
      conv_num = strtol(hex_char, &endptr, 16);	// Předpokládám, že char má min. 8 bitů (viz assert() výše).
      if (errno != 0 || *endptr != '\0') {
        deleteRCString(lexeme);
        throw(ScannerError,((ScannerErrorException){.type=InvalidNumericLiteral, .line_num=*line_num}));
      }
      RCStringAppendChar(lexeme, conv_num);
      break;
    case EOF:
      deleteRCString(lexeme);
      // error: missing terminating " character
      throw(ScannerError,((ScannerErrorException){.type=UnterminatedString, .line_num=*line_num}));
      break;
    default:
      return false;
  }
  *last_letter = getc(f);
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
  
  if (! strcmp("else", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kElse;
  else if (! strcmp("end", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kEnd;
  else if (! strcmp("function", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kFunction;
  else if (! strcmp("if", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kIf;
  else if (! strcmp("return", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kReturn;
  else if (! strcmp("while", RCStringGetBuffer(&cmp_lexeme)))
    keyw = kWhile;
  else
    keyw = NOTKEYW;
  
  deleteRCString(&cmp_lexeme);
  return keyw;
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
 * Načte lexém, zpracuje a vrátí token.
 * @param[in]	f	Ukazatel na otevřený soubor.
 * @return Vrací token.
 */
Token scan(FILE *f)
{
    // TODO: přidat podporu true, false a nil
  Token token;
  RCString lexeme = makeEmptyRCString();	/** Načtený lexém. */
  static char last_letter = '\t';		/** Poslední načtený znak ze zdrojového kódu. */
  static unsigned line_num = 1;			/** Počítadlo řádků vstupního zdrojového kódu. */
  
  int keyw;
  
  double conv_num;	// proměnná do které se uloží převedené číslo
  char *endptr = NULL;	// ukazatel na písmeno kde přestala číst funkce strtod()
  
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
    }
    else if (last_letter == '\n'){
      line_num++;
      token.type = tokEndOfLine;
      last_letter = getc(f);
    }
    else if (isalpha(last_letter)) {
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
        if (last_letter == '.' || last_letter == 'e') {
          do {
            add_char(f, &last_letter, &lexeme);
          } while (isdigit(last_letter));
          break;
        }
      } while (isdigit(last_letter));
      
      RCStringAppendChar(&lexeme, '\0');	// Přidán znak '\0' na konec RCStringu pro fci strtod().
    
      conv_num = strtod(RCStringGetBuffer(&lexeme), &endptr);
      if (errno == 0 && *endptr == '\0') {
        token.type = tokLiteral;
        token.data.val = newValueNumeric( conv_num );
      }
      else {
        deleteRCString(&lexeme);
        throw(ScannerError,((ScannerErrorException){.type=InvalidNumericLiteral, .line_num=line_num}));
      }
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
          if (! det_esc_sequence(f, &last_letter, &lexeme, &line_num)) {
            deleteRCString(&lexeme);
            // .line_num zde obsahuje počátek neukončeného řetězce
            throw(ScannerError,((ScannerErrorException){.type=BadEscSequence, .line_num=start_line_num}));
          }
        }
      }
      
      // Řetězec se uloží do tokenu.
      token.type = tokLiteral;
      token.data.val = newValueString(lexeme);
      last_letter = getc(f);
    }
    else {
      repeat = ! FSM(f, &token, &last_letter, &line_num, &lexeme);
    } // Konec dlouhého přepínače if-else.
  } while (repeat);
  
  deleteRCString(&lexeme);
  return token;
}

void scannerErrorPrint(ScannerErrorException e){
  switch(e.type){
    case InvalidNumericLiteral: fprintf(stderr,"Parse error: Chybne zadane cislo"); break;
    case InvalidToken: fprintf(stderr,"Parse error: Nesmyslny lexem"); break;
    case UnterminatedComment: fprintf(stderr,"Parse error: Neukonceny komentar"); break;
    case UnterminatedString: fprintf(stderr,"Parse error: Neukonceny retezec"); break;
    case BadEscSequence: fprintf(stderr,"Parse error: Chybna escapovaci sekvence v retezci"); break;
  }
  fprintf(stderr," na radku %d\n",e.line_num);
}
