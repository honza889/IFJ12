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
    case tokEndOfFile: printf("[EndOfFile]\n"); break;
  }
}

BEGIN_TEST
  FILE* f=fopen("unitests/scanner/input.txt","r");
  if(f==NULL) ERROR("Nepodarilo se otevrit soubor!");
  Token t;
  RCString s;

  t=scanner(f);
  s=makeRCString("a2");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==5.7 )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kIf )

  t=scanner(f);
  s=makeRCString("a2");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opLT )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==0.0 )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kWhile )

  t=scanner(f);
  s=makeRCString("a2");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokOp )
  TEST( t.data.op==opLT )

  t=scanner(f);
  TEST( t.type==tokNum )
  TEST( t.data.val.type==typeNumeric )
  TEST( t.data.val.data.numeric==8.9 )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kEnd )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kElse )

  t=scanner(f);
  TEST( t.type==tokKeyW )
  TEST( t.data.keyw==kEnd )

  t=scanner(f);
  s=makeRCString("x");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  s=makeRCString("print");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokLParen )

  t=scanner(f);
  s=makeRCString("text");
  TEST( t.type==tokString )
  TEST( RCStringCmp(&t.data.string,&s) == 0 )
  deleteRCString(&t.data.string);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokRParen )

  t=scanner(f);
  s=makeRCString("x");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokAssign )

  t=scanner(f);
  s=makeRCString("print");
  TEST( t.type==tokId )
  TEST( RCStringCmp(&t.data.id,&s) == 0 )
  deleteRCString(&t.data.id);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokLParen )

  t=scanner(f);
  s=makeRCString("");
  TEST( t.type==tokString )
  TEST( RCStringCmp(&t.data.string,&s) == 0 )
  deleteRCString(&t.data.string);
  deleteRCString(&s);

  t=scanner(f);
  TEST( t.type==tokRParen )

  fclose(f);
  
END_TEST
