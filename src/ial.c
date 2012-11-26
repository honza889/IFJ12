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

/************************** VYHLEDAVANI PODRETEZCE *************************/

Value find( ValueList data, int count )
{
    //pro pochopeni celeho algoritmu doporucuji priklad na wiki
    //http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm

    int i, j;

    RCString a = getValueString( testValue(&data[0], typeString) );
    RCString b = getValueString( testValue(&data[1], typeString) );
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


/********************************* RAZENI **********************************/

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
    RCString str = getValueString(testValue(&data[0], typeString));
    if (str.length <= 1) {
      Value ret = newValueString(str);
      deleteRCString(&str);
      return ret;
    }

    char *result = mergesort((char*)RCStringGetBuffer(&str), str.length);
    result = resizeArray(result, char, str.length + 1);
    result[str.length] = '\0';
    deleteRCString(&str);

    RCString resultStr = makeRCString(result);
    free(result);

    Value ret = newValueString(resultStr);
    deleteRCString(&resultStr);

    return ret;
}


/***************************** TABULKA SYMBOLU *****************************/

/**
 * Vyhledani uzlu (uzel=root/greater/lesser)
 * Vstupem ukazatel na ukazatel na vrchol stromu (Symbol**)
 * Vystupem ukazatel na uzel pro symbol (Symbol**)
 */
Symbol** searchSymbol(Symbol **destination, RCString name, bool *exist){
 while(*destination!=NULL){
  int cmp=RCStringCmp(&(*destination)->name,&name);
  if(cmp==0){ // destination name == name
   *exist=true; // symbol nalezen
   break;
  }else if(cmp<0){ // destination name < name
   destination = &((*destination)->greater);
  }else{ // destination name > name
   destination = &((*destination)->lesser);
  }
 }
 return destination;
}

/**
 * Vytvoreni uzlu v zadane tabulce
 */
bool setNewSymbol( RCString name, SymbolTable *table )
{
 bool exist=false;
 Symbol **destination; // ukazatel na promennou v budoucim rodici
 
 // Prohledame zda uz v tabulce je
 destination = searchSymbol(&(table->root),name,&exist);
 
 // Vytvoreni symbolu, pokud nejde o prepis exitujiciho
 if(!exist){
  // destination nyni ukazuje na NULL ukazatel (volny uzel)
  
  Symbol *newSymbol = malloc(sizeof(Symbol));
  MALLCHECK(newSymbol);
  
  // Kopie nazvu promenne
  newSymbol->name=copyRCString(&name);
  
  newSymbol->lesser = NULL;
  newSymbol->greater = NULL;
  newSymbol->index = table->count++;
  *destination = newSymbol;
  
 }
 return !exist; // true pokud byl vytvoren, false pokud uz existoval
}


/**
 * Vrati index symbolu daneho jmena, pokud neexistuje, vytvori ho
 * @return index (zaporny=globalTable, kladny=localTable)
 */
int getSymbol(RCString name, SymbolTable *globalTable, SymbolTable *localTable){
 
 bool inLocal=false;
 bool exist=false;
 int index;
 Symbol **destination; // ukazatel na promennou v budoucim rodici
 
 // Nejprve bude prohledana globalni tabulka
 destination = searchSymbol(&(globalTable->root),name,&exist);
 
 // Mame-li lokalni tabulku, a symbol v minule nebyl, prohledame lokalni
 if(localTable!=NULL && !exist){
  destination = searchSymbol(&(localTable->root),name,&exist);
  inLocal=true;
 }
 
 // Vytvoreni symbolu, pokud nejde o prepis exitujiciho
 if(!exist){
  // destination nyni ukazuje na NULL ukazatel (volny uzel)
  
  Symbol *newSymbol = malloc(sizeof(Symbol));
  MALLCHECK(newSymbol);
  
  // Kopie nazvu promenne
  newSymbol->name=copyRCString(&name);		// DE-BUG: 1. kopie
  
  // Ziskani a nasledne navyseni indexu
  index=(inLocal?localTable:globalTable)->count++;
  
  newSymbol->lesser = NULL;
  newSymbol->greater = NULL;
  newSymbol->index = index;
  *destination = newSymbol;
  
 }else{ // Jde o prepis stavajiciho
  
  // destination nyni ukazuje na ukazatel na stejnojmeny prvek
  index = (*destination)->index;
  
 }
 
 // prevod fyzicke adresy na logickou
 return (inLocal?index:-index-1);
}

/**
 * Uvolni symbol (vcetne obsahu)
 */
void freeSymbol(Symbol *s){
 deleteRCString(&s->name);
 if(s->lesser!=NULL) freeSymbol(s->lesser);
 if(s->greater!=NULL) freeSymbol(s->greater);
 free(s);
}

/**
 * Uvolni strom symbolu
 */
void freeSymbolTable(SymbolTable *st){
 if(st->root!=NULL) freeSymbol(st->root);
}

