#include <stdio.h>
#include "scanner.h"
#include "exceptions.h"
#include "../test.h"

void printTokenType(Token* t){
  switch(t->type){
    case tokId: printf("[Id]\n"); break;
    case tokString: printf("[String]\n"); break;
    case tokNum: printf("[Num]\n"); break;
    case tokOp: printf("[Op]\n"); break;
    case tokKeyW: printf("[KeyW]\n"); break;
    case tokAssign: printf("[Assign]\n"); break;
    case tokLParen: printf("[LParen]\n"); break;
    case tokRParen: printf("[RParen]\n"); break;
    case tokLBracket: printf("[LBracket]\n"); break;
    case tokRBracket: printf("[RBracket]\n"); break;
    case tokColon: printf("[Colon]\n"); break;
    case tokComma: printf("[Comma]\n"); break;
    case tokEndOfLine: printf("[EndOfLine]\n"); break;
    case tokEndOfFile: printf("[EndOfFile]\n"); break;
  }
}

BEGIN_TEST
  FILE* f=fopen("unitests/scanner/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  Token t;
  RCString s;

  try{

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "a2" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric == (double) 5.7 )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kIf )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "a2" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opLT )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==0.0 )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kWhile )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "a2" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opLT )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric == (double) 8.9 )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kEnd )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kElse )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kEnd )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "x" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "print" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokLParen )

  t=scanner(f);
  TEST( t.type==tokString )
  STRTEST( t.data.string, "text" )
  deleteRCString(&t.data.string);

  t=scanner(f);
  TEST( t.type==tokComma )

  t=scanner(f);
  TEST( t.type==tokString )
  STRTEST( t.data.string, "text2" )
  deleteRCString(&t.data.string);

  t=scanner(f);
  TEST( t.type==tokRParen )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "x" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "print" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokLParen )

  t=scanner(f);
  TEST( t.type==tokString )
  STRTEST( t.data.string, "" )
  deleteRCString(&t.data.string);

  t=scanner(f);
  TEST( t.type==tokRParen )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "retezec" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  TEST( t.type==tokString )
  STRTEST( t.data.string, "retez" )
  deleteRCString(&t.data.string);

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opPlus )

  t=scanner(f);
  TEST( t.type==tokString )
  STRTEST( t.data.string, " \n \t \\ \" \x01 \xFF " )
  deleteRCString(&t.data.string);

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kFunction )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "podretezec" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokLParen )

  t=scanner(f);
  TEST( t.type==tokRParen )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kReturn )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "retezec" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokLBracket )

  t=scanner(f);
  TEST( t.type==tokColon )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==19.0 )

  t=scanner(f);
  TEST( t.type==tokRBracket )

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opPlus )

  t=scanner(f);
  TEST( t.type==tokId )
  STRTEST( t.data.id, "retezec" )
  deleteRCString(&t.data.id);

  t=scanner(f);
  TEST( t.type==tokLBracket )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==0.0 )

  t=scanner(f);
  TEST( t.type==tokColon )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==1.0 )

  t=scanner(f);
  TEST( t.type==tokRBracket )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kEnd )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfLine )

  t=scanner(f);
  TEST( t.type==tokEndOfFile )

  }
  catch{
   on(ScannerError,e){
    scannerErrorPrint(*e);
    exit(1);
   }
  }

  fclose(f);
  
END_TEST
