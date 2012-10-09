
#include <stdio.h>
#include <assert.h>
#include "value.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
 
 char *s;
 Value v = {typeUndefined};
 Value w = {typeUndefined};
 int status = 0;
 
 /************** nedefinovne **************/

 EXCEPT_TEST( IncompatibleComparison, greaterValue(&v,&w) )

 try{
  greaterValue(&v,&w);
  TEST( false )
 }
 catch{
  on( IncompatibleComparison, e ){
   status = 1;
  }
 }
 TEST( status == 1 );
 
 try{
  greaterValue(&v,&w);
  TEST( false )
 }
 catch{
  on( IncompatibleComparison, e ){
   status = 1;
  }
 }
 TEST( status == 1 );

 /************** set/get **************/
 
 setValueNil(&v);
 STRTEST( s=getValueString(&v),"Nil" );free(s);
 TEST( getValueBoolean(&v) == false );
 TEST( typeOfValue(&v) == 0 );
 
 setValueBoolean(&v,true);
 STRTEST( s=getValueString(&v),"true" );free(s);
 TEST( getValueBoolean(&v) == true );
 TEST( typeOfValue(&v) == 1 );

 setValueBoolean(&v,false);
 STRTEST( s=getValueString(&v), "false" );free(s);
 TEST( getValueBoolean(&v) == false );
 TEST( typeOfValue(&v) == 1 );

 setValueNumeric(&v,123.456); // %g vypisuje jen na 6 cifer
 STRTEST( s=getValueString(&v), "123.456" ); free( s );
 TEST( getValueBoolean(&v) );
 TEST( typeOfValue(&v) == 3 );
 
 setValueString(&v,"Toto je řetězec znaků");
 STRTEST( s=getValueString(&v), "Toto je řetězec znaků" );free(s);
 TEST( getValueBoolean(&v) == true );
 TEST( typeOfValue(&v) == 8 );
 
 /************** porovnavani - spravne **************/
 
 setValueNil(&v);
 setValueNil(&w);
 TEST( equalValue(&v,&w) == true );
 
 setValueBoolean(&v,true);
 setValueBoolean(&w,true);
 TEST( equalValue(&v,&w) == true );
 
 setValueNumeric(&v,1.2);
 setValueNumeric(&w,1.2);
 TEST( equalValue(&v,&w) == true );
 
 setValueString(&v,"Řetězec");
 setValueString(&w,"Řetězec");
 TEST( equalValue(&v,&w) == true );
 
 setValueString(&v,"a");
 setValueString(&w,"a");
 TEST( equalValue(&v,&w) == true );
 TEST( greaterValue(&v,&w) == false );
 TEST( greaterEqualValue(&v,&w) == true );
 TEST( lesserValue(&v,&w) == false );
 TEST( lesserEqualValue(&v,&w) == true );
 
 setValueString(&v,"a");
 setValueString(&w,"z");
 TEST( equalValue(&v,&w) == false );
 TEST( greaterValue(&v,&w) == false );
 TEST( greaterEqualValue(&v,&w) == false );
 TEST( lesserValue(&v,&w) == true );
 TEST( lesserEqualValue(&v,&w) == true );

 setValueString(&v,"z");
 setValueString(&w,"a");
 TEST( equalValue(&v,&w) == false );
 TEST( greaterValue(&v,&w) == true );
 TEST( greaterEqualValue(&v,&w) == true );
 TEST( lesserValue(&v,&w) == false );
 TEST( lesserEqualValue(&v,&w) == false );
 
 EXCEPT_TEST( IncompatibleComparison, greaterValue(&v,&w) )
 
 /************** porovnavani - chybne **************/
 
 setValueNumeric(&v,5.0);
 setValueString(&w,"a");
 try{
  greaterValue(&v,&w);
  TEST( false )
 }
 catch{
  on( IncompatibleComparison, e ){
   status = 2;
  }
 }
 TEST( status == 2 );
 
 freeValue(&v);
 freeValue(&w);
 
END_TEST
