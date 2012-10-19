#!/bin/bash

if [ "$1" == "valgrind" ]; then
	DOVALGRIND=true
	shift
else
    DOVALGRIND=false
fi


for FILE in $1; do
	FILE=${FILE%/test}
	FILE=${FILE#unitests/}
	if [ -f "unitests/$FILE/test" ]; then
		echo "*********************************"
		echo "  Test: $FILE"
		echo "¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨"
		if [ $DOVALGRIND == false ]; then
			unitests/$FILE/test
		  	if [ "$?" == "0" ]; then
				echo "Exit OK"
			fi
		else
			valgrind --leak-check=full -q unitests/$FILE/test > "unitests/$FILE/out" 2>&1
			FILESIZE=$(stat -c%s "unitests/$FILE/out")
			if [[ $FILESIZE > 1 ]] ; then # Nekdy soubor obsahoval \n
				cat unitests/$FILE/out
			else
				echo "Valgrind OK"
				unitests/$FILE/test
		  	 	if [ "$?" == "0" ]; then
					echo "Exit OK"
				fi
			fi
		fi
	fi
done

