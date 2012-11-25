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

# Chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému):
#echo 'x = numeric(a)' > /tmp/exceptions.ifj
#line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 1 )	|| result=$?

# Chyba v programu v rámci syntaktické analýzy (chybná syntaxe struktury programu):
#echo 'x = numeric(a)' > /tmp/exceptions.ifj
#line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 2 )	|| result=$?

# Nedefinovaná proměnná:
echo 'x = numeric(a)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 3 )	|| result=$?

# Nedefinovaná funkce:
echo 'x = anlkavl(a)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 4 )	|| result=$?

# Ostatní sémantické chyby:
echo 'a = 123.0' > /tmp/exceptions.ifj
echo 'x = a[1.0:2.0]' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?

echo 'x = "0.0"/2.0' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 5 )	|| result=$?

# Běhová chyba dělení nulou:
echo 'a = 0.0' > /tmp/exceptions.ifj
echo 'x = 2.0/a' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 10 )	|| result=$?

echo 'x = 0.0/0.0' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 10 )	|| result=$?

# Běhová chyba nekompatibility typů (např. nekorektní operace nad proměnnými atd.):
echo 'a = "0.0"' > /tmp/exceptions.ifj
echo 'x = 2.0/a' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'a = "0.0"' > /tmp/exceptions.ifj
echo 'x = a/2.0' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'a = "1.0"' > /tmp/exceptions.ifj
echo 'x = "abc"[a:2.0]' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'b = "1.0"' > /tmp/exceptions.ifj
echo 'x = "abc"[2.0:b]' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'b = 123.0' > /tmp/exceptions.ifj
echo 'x = sort(b)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'x = sort(123.0)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'a = 123.0' > /tmp/exceptions.ifj
echo 'x = find(a, "abc")' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'b = 123.0' > /tmp/exceptions.ifj
echo 'x = find("abc", b)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'x = find(123.0, "abc")' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

echo 'x = find("abc", 123.0)' > /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 11 )	|| result=$?

# Běhová chyba při přetypování na číslo:
echo 'a = ""' > /tmp/exceptions.ifj
echo 'x = numeric(a)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 12 )	|| result=$?

echo 'a = "123"' > /tmp/exceptions.ifj
echo 'x = numeric(a)' >> /tmp/exceptions.ifj
line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 12 )	|| result=$?

# Ostatní běhové chyby:
#echo 'a = ""' > /tmp/exceptions.ifj
#echo 'x = numeric(a)' >> /tmp/exceptions.ifj
#line_num=$LINENO; ( ./ifj12-dbg /tmp/exceptions.ifj; expectResult 13 )	|| result=$?

################################

exit $result