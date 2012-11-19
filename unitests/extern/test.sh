#!/bin/bash
pause() {
  echo '<Pro pokracovani stisknete ENTER>'
  read pokracovat
}

if [ "$1" == "novalgrind" ]; then
  DOVALGRIND=""
else
  DOVALGRIND="valgrind --leak-check=full -q"
fi

echo "Jednoduchy test IFJ12:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/simple.ifj'
echo

echo "Ze zadani 1:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/zezadani1.ifj <<< 5.0'
echo

echo "Ze zadani 2:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/zezadani2.ifj <<< 5.0'
echo

echo "Ze zadani 3:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/zezadani3.ifj <<< "acdbghfe"'
echo; pause

echo "Testy BIF a IAL funkci:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/bif.ifj'
echo

echo "Test parseExpression a AST - priority operatoru:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/priority.ifj'
echo

echo "Test substringovani:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/substr.ifj'
echo

echo "Test function, if, while:"
eval $DOVALGRIND './ifj12-dbg unitests/extern/blocks.ifj'
echo; pause

# echo "Test nacteni dlouheho zrojaku:"
# eval $DOVALGRIND './ifj12-dbg unitests/extern/dlouhyZdrojak.ifj'
# echo; pause

echo "=============="
echo "= TESTY CHYB ="
echo "=============="
IFS=''; echo
echo "Testy chovani funkce numeric:"
got=$(unitests/extern/numeric.sh 2>&1 >/dev/null)
if [ $? -eq 0 ]; then
  echo "unitests/extern/numeric.sh: OK"
else
  echo $got
  echo "---------------------------------"
  echo "unitests/extern/numeric.sh: ERROR"
fi
echo; pause
