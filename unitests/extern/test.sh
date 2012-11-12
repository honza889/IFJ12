#!/bin/sh
echo "Simple:"
./ifj12-dbg unitests/extern/simple.ifj
echo
echo "Ze zadani 1:"
echo 5 | ./ifj12-dbg unitests/extern/zezadani1.ifj
echo
echo "Ze zadani 2:"
echo 5 | valgrind ./ifj12-dbg unitests/extern/zezadani2.ifj
echo
echo "Ze zadani 3:"
echo abcdefgh | ./ifj12-dbg unitests/extern/zezadani3.ifj
echo

