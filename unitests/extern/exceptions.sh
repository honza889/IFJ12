#!/bin/bash
result=0
export line_num=$LINENO

expectResult() {
  # Definice lokálních proměnných.
  local got=$?	# Načtení navratové hodnoty.
  local expected=$1
  
  if [ $expected -ne $got ]; then
    echo "$0: $line_num: ExpectedResult \"$expected\" but got \"$got\"." >&2
    return 1
  fi
}

################################
echo 'Test navratovych hodnot:'
################################

#=== Návratová hodnota 1 =============================================================
# Chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému):
echo -e 'x = "pok\0us"' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 1 )	|| result=$?

#=== Návratová hodnota 2 =============================================================
# Chyba v programu v rámci syntaktické analýzy (chybná syntaxe struktury programu):
echo -n 'x = numeric("10.0")' > /tmp/exceptions.ifj 	# Schází EOL.
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 2 )	|| result=$?

echo 'x = 1.0(5.0)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 2 )	|| result=$?

echo 'return' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 2 )	|| result=$?

#=== Návratová hodnota 3 =============================================================
# Nedefinovaná proměnná:
echo 'x = numeric(a)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 3 )	|| result=$?

#=== Návratová hodnota 4 =============================================================
# Nedefinovaná funkce:
echo 'x = anlkavl(a)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 4 )	|| result=$?

#=== Návratová hodnota 5 =============================================================
# Ostatní sémantické chyby:
echo 'a = 123.0' > /tmp/exceptions.ifj
echo 'x = a[1.0:2.0]' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = "abc"["1.0":2.0]' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = "abc"[1.0:"2.0"]' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = "0.0"/2.0' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?

#=== Návratová hodnota 10 ============================================================
# Běhová chyba dělení nulou:
echo 'a = 0.0' > /tmp/exceptions.ifj
echo 'x = 2.0/a' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 10 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = 0.0/0.0' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 10 )	|| result=$?

#=== Návratová hodnota 11 ============================================================
# Běhová chyba nekompatibility typů (např. nekorektní operace nad proměnnými atd.):

echo 'function MyMain(str)' 	> /tmp/exceptions.ifj
echo '  x = 2.0/str' 		>> /tmp/exceptions.ifj
echo '  return x' 		>> /tmp/exceptions.ifj
echo 'end' 			>> /tmp/exceptions.ifj
echo 'a = MyMain("0.0")' 	>> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'function MyMain(str)' 	> /tmp/exceptions.ifj
echo '  x = str/2.0' 		>> /tmp/exceptions.ifj
echo '  return x' 		>> /tmp/exceptions.ifj
echo 'end' 			>> /tmp/exceptions.ifj
echo 'a = MyMain("0.0")' 	>> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'function MyMain(str)' 	> /tmp/exceptions.ifj
echo '  x = "abc"[str:2.0]' 	>> /tmp/exceptions.ifj
echo '  return x' 		>> /tmp/exceptions.ifj
echo 'end' 			>> /tmp/exceptions.ifj
echo 'a = MyMain("1.0")' 	>> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'function MyMain(str)' 	> /tmp/exceptions.ifj
echo '  x = "abc"[1.0:str]' 	>> /tmp/exceptions.ifj
echo '  return x' 		>> /tmp/exceptions.ifj
echo 'end' 			>> /tmp/exceptions.ifj
echo 'a = MyMain("2.0")' 	>> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'b = 123.0' > /tmp/exceptions.ifj
echo 'x = sort(b)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = sort(123.0)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'a = 123.0' > /tmp/exceptions.ifj
echo 'x = find(a, "abc")' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'b = 123.0' > /tmp/exceptions.ifj
echo 'x = find("abc", b)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = find(123.0, "abc")' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'x = find("abc", 123.0)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

#=== Návratová hodnota 12 ============================================================
# Běhová chyba při přetypování na číslo:
echo 'a = ""' > /tmp/exceptions.ifj
echo 'x = numeric(a)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 12 )	|| result=$?
#-------------------------------------------------------------------------------------

echo 'a = "123"' > /tmp/exceptions.ifj
echo 'x = numeric(a)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 12 )	|| result=$?

#=== Návratová hodnota 13 ============================================================
# Ostatní běhové chyby:
echo -e 'pok\0us' > /tmp/exceptions.ifj.data
echo 'x = input()' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj < /tmp/exceptions.ifj.data; expectResult 13 )	|| result=$?

#=== Návratová hodnota 99 ============================================================
# Interní chyba interpretu tj. neovlivněná vstupním programem (např. chyba alokace
# paměti, chyba při otvírání vstupního souboru atd.):
if [ ! -e d609724e15a6649efc94fcabff61b61387386aaa ]; then
  line_num=$LINENO; ( ./ifj12-dbg d609724e15a6649efc94fcabff61b61387386aaa; expectResult 99 )	|| result=$?
fi

echo > /tmp/exceptions.ifj
chmod -r /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 99 )	|| result=$?
rm /tmp/exceptions.ifj

line_num=$LINENO; ( ./ifj12-dbg; expectResult 99 )	|| result=$?

################################

exit $result
