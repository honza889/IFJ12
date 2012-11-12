#!/bin/sh
echo "Simple:"
valgrind ./ifj12 unitests/extern/simple.ifj
echo
echo "Ze zadani 1:"
valgrind ./ifj12 unitests/extern/zezadani1.ifj
echo
echo "Ze zadani 2:"
valgrind ./ifj12 unitests/extern/zezadani2.ifj
echo
echo "Ze zadani 3:"
valgrind ./ifj12 unitests/extern/zezadani3.ifj
echo

