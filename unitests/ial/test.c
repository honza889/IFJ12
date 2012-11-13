#include <stdio.h>
#include "ial.h"
#include "exceptions.h"
#include "../test.h"

BEGIN_TEST


    Value stringy[2] = { newValueUndefined(), newValueUndefined()  };
    Value ret;

    setValueCString( stringy, "paparachurparparticipate in parachute" );
    setValueCString( stringy+1, "parachu" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 2){
        printf("%s", "TEST ial.c - find cislo 1\n");
        printf("'Ocekavana hodnota = '2'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "parachurparparticipate in parachute" );
    setValueCString( stringy+1, "parachu" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 2\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "paparachurparparticipate in parachute" );
    setValueCString( stringy+1, "parachut" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 28){
        printf("%s", "TEST ial.c - find cislo 3\n");
        printf("'Ocekavana hodnota = '28'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "paparachurparparticipate in parachute" );
    setValueCString( stringy+1, "parachuB" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != -1){
        printf("%s", "TEST ial.c - find cislo 4\n");
        printf("'Ocekavana hodnota = '-1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "pa" );
    setValueCString( stringy+1, "p" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 5\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
        RCString str = getValueString( &ret );
        RCStringPrint( &str, stdout );
        deleteRCString( &str );
    }


    setValueCString( stringy, "pa" );
    setValueCString( stringy+1, "a" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 1){
        printf("%s", "TEST ial.c - find cislo 6\n");
        printf("'Ocekavana hodnota = '1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "pa" );
    setValueCString( stringy+1, "pa" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 7\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "e" );
    setValueCString( stringy+1, "e" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 8\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "e" );
    setValueCString( stringy+1, "a" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != -1){
        printf("%s", "TEST ial.c - find cislo 9\n");
        printf("'Ocekavana hodnota = '-1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "papar" );
    setValueCString( stringy+1, "r" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 4){
        printf("%s", "TEST ial.c - find cislo 10\n");
        printf("'Ocekavana hodnota = '4'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "papar" );
    setValueCString( stringy+1, "p" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 11\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "p" );
    setValueCString( stringy+1, "" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 12\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "" );
    setValueCString( stringy+1, "p" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != -1){
        printf("%s", "TEST ial.c - find cislo 13\n");
        printf("'Ocekavana hodnota = '-1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }


    setValueCString( stringy, "parachutek" );
    setValueCString( stringy+1, "k" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 9){
        printf("%s", "TEST ial.c - find cislo 14\n");
        printf("'Ocekavana hodnota = '9'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }

    setValueCString( stringy, "para" );
    setValueCString( stringy+1, "ar" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 1){
        printf("%s", "TEST ial.c - find cislo 15\n");
        printf("'Ocekavana hodnota = '1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }

    setValueCString( stringy, "parachutek" );
    setValueCString( stringy+1, "rachutek" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 2){
        printf("%s", "TEST ial.c - find cislo 16\n");
        printf("'Ocekavana hodnota = '2'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }

    setValueCString( stringy, "parachutek" );
    setValueCString( stringy+1, "c" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 4){
        printf("%s", "TEST ial.c - find cislo 17\n");
        printf("'Ocekavana hodnota = '4'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }

    setValueCString( stringy, "parachutek" );
    setValueCString( stringy+1, "" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != 0){
        printf("%s", "TEST ial.c - find cislo 18\n");
        printf("'Ocekavana hodnota = '0'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }

    setValueCString( stringy, "" );
    setValueCString( stringy+1, "c" );
    ret = find( stringy, 2 );
    if(ret.data.numeric != -1){
        printf("%s", "TEST ial.c - find cislo 19\n");
        printf("'Ocekavana hodnota = '-1'\nSkutecna hodnota = '%f'\n", ret.data.numeric);
    }
    freeValue( stringy );
    freeValue( stringy+1 );
    freeValue( &ret );

END_TEST
