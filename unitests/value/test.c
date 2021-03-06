#include <stdio.h>
#include "value.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
 RCString s;
 Value v = {typeUndefined};
 Value w = {typeUndefined};
 
 
 /************** nedefinovne **************/

 EXCEPT_TEST( UndefinedVariable, greaterValue(&v,&w) );
 EXCEPT_TEST( UndefinedVariable, greaterEqualValue(&v,&w) );
 EXCEPT_TEST( UndefinedVariable, equalValue(&v,&w) );
 EXCEPT_TEST( UndefinedVariable, lesserEqualValue(&v,&w) );
 EXCEPT_TEST( UndefinedVariable, lesserValue(&v,&w) );
 
 
 /************** set/get **************/
 
 setValueNil(&v);
 STRTEST( s=getValueString(&v),"Nil" );deleteRCString(&s);
 TEST( getValueBoolean(&v) == false );
 TEST( typeOfValue(&v) == 0 );
 EXCEPT_TEST( InvalidConversion , getValueNumeric(&v) );

 setValueBoolean(&v,true);
 STRTEST( s=getValueString(&v),"true" );deleteRCString(&s);
 TEST( getValueBoolean(&v) == true );
 TEST( typeOfValue(&v) == 1 );
 EXCEPT_TEST( InvalidConversion , getValueNumeric(&v) );

 setValueBoolean(&v,false);
 STRTEST( s=getValueString(&v), "false" );deleteRCString(&s);
 TEST( getValueBoolean(&v) == false );
 TEST( typeOfValue(&v) == 1 );
 EXCEPT_TEST( InvalidConversion , getValueNumeric(&v) );

 setValueNumeric(&v,123.456); // %g vypisuje jen na 6 cifer
 STRTEST( s=getValueString(&v), "123.456" ); deleteRCString( &s );
 TEST( getValueBoolean(&v) );
 TEST( typeOfValue(&v) == 3 );
 DBLTEST( getValueNumeric(&v), 123.456 );
 
 setValueCString(&v,"Toto je řetězec znaků");
 STRTEST( s=getValueString(&v), "Toto je řetězec znaků" );deleteRCString(&s);
 TEST( getValueBoolean(&v) == true );
 TEST( typeOfValue(&v) == 8 );
 EXCEPT_TEST( InvalidConversion , getValueNumeric(&v) );
 
 setValueCString(&v,"15.97");
 TEST( typeOfValue(&v) == 8 );
 DBLTEST( getValueNumeric(&v), 15.97 );

 setValueCString(&v,"1.97e-3");
 TEST( typeOfValue(&v) == 8 );
 DBLTEST( getValueNumeric(&v), 0.00197 );

 /**************** numeric - spravne ****************/

 setValueCString(&v,"1.0");
 DBLTEST( getValueNumeric(&v), 1.0);

 setValueCString(&v,"1e0");
 DBLTEST( getValueNumeric(&v), 1.0);

 setValueCString(&v,"1e+1");
 DBLTEST( getValueNumeric(&v), 10.0);

 setValueCString(&v,"1e-1");
 DBLTEST( getValueNumeric(&v), 0.1);

 setValueCString(&v,"  1.2e-2xxx");
 DBLTEST( getValueNumeric(&v), 0.012);

 /************** numeric - nekorektni ***************/

 setValueCString(&v,"");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 setValueCString(&v," ");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 setValueCString(&v,"1");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 setValueCString(&v,"x1.0");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 setValueCString(&v,"1ee0");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 setValueCString(&v," 1 1.2 ");
 EXCEPT_TEST(InvalidConversion, getValueNumeric(&v) );

 
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
 
 setValueCString(&v,"Řetězec");
 setValueCString(&w,"Řetězec");
 TEST( equalValue(&v,&w) == true );
 
 
 setValueCString(&v,"a");
 setValueCString(&w,"a");
 TEST( equalValue(&v,&w) == true );
 TEST( greaterValue(&v,&w) == false );
 TEST( greaterEqualValue(&v,&w) == true );
 TEST( lesserValue(&v,&w) == false );
 TEST( lesserEqualValue(&v,&w) == true );
 
 setValueCString(&v,"a");
 setValueCString(&w,"z");
 TEST( equalValue(&v,&w) == false );
 TEST( greaterValue(&v,&w) == false );
 TEST( greaterEqualValue(&v,&w) == false );
 TEST( lesserValue(&v,&w) == true );
 TEST( lesserEqualValue(&v,&w) == true );

 setValueCString(&v,"z");
 setValueCString(&w,"a");
 TEST( equalValue(&v,&w) == false );
 TEST( greaterValue(&v,&w) == true );
 TEST( greaterEqualValue(&v,&w) == true );
 TEST( lesserValue(&v,&w) == false );
 TEST( lesserEqualValue(&v,&w) == false );
 
 
 /************** porovnavani ruznych typu **************/
 
 setValueNumeric(&v,5.0);
 setValueCString(&w,"a");
 TEST( equalValue(&v,&w) == false );
 EXCEPT_TEST( IncompatibleComparison, greaterValue(&v,&w) );
 EXCEPT_TEST( IncompatibleComparison, greaterEqualValue(&v,&w) );
 EXCEPT_TEST( IncompatibleComparison, lesserEqualValue(&v,&w) );
 EXCEPT_TEST( IncompatibleComparison, lesserValue(&v,&w) );
 
 
 freeValue(&v);
 freeValue(&w);
END_TEST
