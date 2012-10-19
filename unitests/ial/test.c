#include <stdio.h>
#include "ial.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST

//GL + HF TO DO..
   Value stringy[2] = { newValueUndefined(), newValueUndefined()  };

    setValueCString( stringy, "paparachurparparticipate in parachute" );
    setValueCString( stringy+1, "parachu" );

    Value ret = find( stringy, 2 );

    printf("'%f'", ret.data.numeric);

    freeValue( stringy );
    freeValue( stringy+1 );
    freeValue( &ret );

END_TEST
