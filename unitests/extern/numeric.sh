#!/bin/bash
result=0
export line_num=$LINENO

expect() {
  # Definice lokálních proměnných.
  local got=""
  local expected=$1
  
  read got	# Načtení ze stdin.
  if [ "$expected" != "$got" ]; then
    echo "$0: $line_num: Expected \"$expected\" but got \"$got\"." >&2
    return 1
  fi
}

expectResult() {
  # Definice lokálních proměnných.
  local got=$?	# Načtení navratové hodnoty.
  local expected=$1
  
  if [ $expected -ne $got ]; then
    echo "$0: $line_num: ExpectedResult \"$expected\" but got \"$got\"." >&2
    return 1
  fi
}

echo 'Test chovani funkce numeric:'
line_num=$LINENO; echo 'x=print(numeric("42.0"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 42 )		|| result=$?
line_num=$LINENO; echo 'x=print(numeric("42.0e20"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 4.2e+21 )	|| result=$?
line_num=$LINENO; echo 'x=print(numeric("42.0e+2"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 4200 )	|| result=$?
line_num=$LINENO; echo 'x=print(numeric("42.0e-2"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 0.42 )	|| result=$?
line_num=$LINENO; echo 'x=print(numeric("ab42.0cc"))' > /tmp/numeric.ifj	&& ( ./ifj12-dbg /tmp/numeric.ifj; expectResult 12 )	|| result=$?
line_num=$LINENO; echo 'x=print(numeric("  42.0 c "))' > /tmp/numeric.ifj	&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 42 )		|| result=$?
line_num=$LINENO; echo 'x=print(numeric("42.0cde"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 42 )		|| result=$?
line_num=$LINENO; echo 'x=print(numeric(" 1.0 2.0 "))' > /tmp/numeric.ifj	&& ( ./ifj12-dbg /tmp/numeric.ifj | expect 1 )		|| result=$?
line_num=$LINENO; echo 'x=print(numeric(""))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj; expectResult 12 )	|| result=$?
line_num=$LINENO; echo 'x=print(numeric("abc"))' > /tmp/numeric.ifj		&& ( ./ifj12-dbg /tmp/numeric.ifj; expectResult 12 )	|| result=$?

exit $result