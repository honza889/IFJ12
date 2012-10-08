#!/bin/bash

FILES=`ls unitests`
for FILE in $FILES; do
	if [ -d "unitests/$FILE" ]; then
		echo "*********************************"
		echo "  Test: $FILE"
		echo "*********************************"
		valgrind --leak-check=full -q unitests/$FILE/test > "unitests/$FILE/out" 2>&1
		FILESIZE=$(stat -c%s "unitests/$FILE/out")
		if [[ $FILESIZE > 1 ]] ; then # Nekdy soubor obsahoval \n
			cat unitests/$FILE/out
		else
			echo "Valgrind korektni"
			unitests/$FILE/test
	   	if [ "$?" == "0" ]; then
				echo "Exit korektni"
			fi
		fi
	fi
done

