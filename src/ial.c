/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <stdlib.h>
#include <stdio.h>
#include "ial.h"
#include "alloc.h"


Value find( ValueList data, int count )
{
    //pro pochopeni celeho algoritmu doporucuji priklad na wiki
    //http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm

    int i, j, match = 0;

    RCString a = getValueString( &data[0] );
    RCString b = getValueString( &data[1] );
    int stringLength = RCStringLength( &a );    //delka retezce
    int patternLength = RCStringLength( &b );    //delka vzorku

    const char* string  = RCStringGetBuffer( &a ); //retezec
    const char* pattern  = RCStringGetBuffer( &b );//hledany retezec

    if(patternLength == 0){   //kontrola parametru
        deleteRCString( &a );
        deleteRCString( &b );
        return newValueNumeric( 0 ); // prazdny retezec se vyskytuje na pozici 0.0
    }else if(stringLength == 0){
        deleteRCString( &a );
        deleteRCString( &b );
        return newValueNumeric( -1 );
    }


    ///vytvorime pole pro korekci pri chybe
    int correctionArray[ RCStringLength( &b ) ];

    correctionArray[0] = -1;                            //prvni prvek vzdy musi byt -1

        /** ted budeme prochazet pouze vzorek, ktery chceme vyhledat a urcime
        *   u kazdeho znaku korekci chyby o kterou se posuneme v nasledujicim vyhledavani
        *   napr. "abcababcaa" potrebujeme pole "-1000121234"
        **/
    for( i = 1, j=0; i < patternLength; i++ ){   //1. prvek uz mame -> zacni od indexu 1
                                                        // a prochazej celej vzorek
       if(pattern[i] == pattern[j]){    //porovnavame prvku vzorku se zacatkem vzorku

           correctionArray[i] = j;  //uloz spocitanou hodnotu chyby, prvni je 0
                j++;
           }else{
                correctionArray[i] = j; //zapis hodnotu a
                j = 0;                  //vratime se na zacatek

               if(pattern[i] == pattern[0]){    //ALE pokud tento znak se rovna zacatku
                    j++;                        //zacni uz ted pocitat (j bude tady vzdy 1)
               }
       }
    }//konec vytvareni pole

/*
      //pro kontrolu vypis vytvoreneho pole
      printf("vypis pole chyb\n");

    for( i = 0, j=0; i < RCStringLength( &b ); i++ ){
        printf("%d", correctionArray[i]);
    }
    printf("\n");
*/

    /**
    *   nyni samotny KMP algoritmus
    *   porovnavame znaky vzorku a retezce a podle tabulky kterou jsme si udelali
    *   se budeme nekdy vracet
    **/
    //necht: i = index na prohledavany retezec
    // j = index hledaneho vzorku
    i=0;
    j=0;


    while(patternLength+i-1 < stringLength){

        if(string[i+j] == pattern[j]){      //shoda znaku
            if( j == patternLength-1 ){     //konec: dosli jsme na konec vzorku

                deleteRCString( &a );
                deleteRCString( &b );
                return newValueNumeric( i );//vratime pozici nalezeneho vzorku
            }

            j++;                            //posun na dalsi znak vzorku

        }else{
            i = i + j - correctionArray[j]; //posuneme vyhledavani v retezci o j(tam kde byla nalezena chyba)
            // minus vypocitana hodnota pro znak

            if(correctionArray[j] > -1){   //mimo prvni prvek

                j = correctionArray[j];     //posun se ve vzorku na pozici z pole
            }else{
                j = 0;                      //nebo zacni od prvniho prvku

            }
        }
    }

    deleteRCString( &a );
    deleteRCString( &b );
    return newValueNumeric( -1 );
}

// Seradi znaky ve dvou retezcich
// Oba retezce musi byt uz samostatne serazene a neukoncene nulovym znakem
// @param str1 - ukazatel na prvni retezec
// @param str1 - ukazatel na druhy retezec
// @param len  - pocet znaku v obou retezcich dohromady
// Vraci ukazatel na serazeny retezec
char* merge(char *str1, char *str2, int len)
{
    char* result = newArray(char, len);
    int i = 0, j = 0, k = 0, len1 = len>>1, len2 = (len>>1) + (len&1);
    while(i < len1 && j < len2)
    {   // Serazuje dokud oba retezce obsahuji najaky neserazeny znak
        if(str2[j] < str1[i]) result[k++] = str2[j++];
        else result[k++] = str1[i++];
    }
    // Pripoji zbytek prvniho nebo druheho retezce k jiz serazenym znakum
    while(i < len1) result[k++] = str1[i++];
    while(j < len2) result[k++] = str2[j++];
    free(str1);
    free(str2);
    return result;
}

// Rekurzivne rozdeli seznam na jednotlive polozky a serazene je slepuje dohromady
// @param str - ukazatel na retezec, jehoz znaky se maji seradit (neobsahuje \0)
// @param len - delka retezce
// Vraci ukazatel na seradeny retezec
char* mergesort(char *str, int len)
{
    if(len == 1)
    {
        char* result = newArray(char, 1);
        result[0] = str[0];
        return result;
    }            // prvni mergesort se vola s prvni pulkou retezce, druhy s druhou
    return merge(mergesort(str, len>>1), mergesort(&str[len>>1], (len>>1) + (len&1)), len);
}

// Seradi znaky v retezci
// Obalovaci funkce kvuli konverzi z/na RCString
Value sort(ValueList data, int count)
{
    RCString str = getValueString(&data[0]);
    if(str.length<=1){
      return newValueString(str);
    }
    char *result = mergesort((char*)RCStringGetBuffer(&str), str.length);
    result = resizeArray(result, char, str.length + 1);
    result[str.length] = '\0';
    deleteRCString(&str);
    return newValueString(makeRCString(result));
}
