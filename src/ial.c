#include <stdlib.h>
#include <stdio.h>
#include "ial.h"


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

/*      //pro kontrolu vypis vytvoreneho pole
    for( i = 0, j=0; i < RCStringLength( &b ); i++ ){
        printf("%d", correctionArray[i]);
    }
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

    while(patternLength+i < stringLength){

        if(string[i+j] == pattern[j]){      //shoda znaku

            if( j == patternLength-1 ){     //konec: dosli jsme na konec vzorku
                return newValueNumeric( i );//vratime pozici nalezeneho vzorku
            }

            j++;                            //posun na dalsi znak vzorku

        }else{
            i = i + j - correctionArray[j]; //posuneme vyhledavani v retezci o j(tam kde byla nalezena chyba)
            // minus vypocitana hodnota pro znak

            if(correctionArray[i] != -1){   //mimo prvni prvek
                j = correctionArray[j];     //posun se ve vzorku na pozici z pole
            }else{
                j = 0;                      //nebo zacni od prvniho prvku

            }
        }
    }


    deleteRCString( &a );
    deleteRCString( &b );
    return newValueNumeric( stringLength );
}
