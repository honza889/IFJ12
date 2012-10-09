#!/bin/bash
echo $1
FILES=`ls unitests`
for FILE in $FILES; do
	if [ -d "unitests/$FILE" ]; then
		echo "*********************************"
		echo "  Test: $FILE"
		echo "*********************************"
		if [ "$1" == "valgrind" ]; then
			valgrind --leak-check=full -q unitests/$FILE/test > "unitests/$FILE/out" 2>&1
		else
			unitests/$FILE/test > "unitests/$FILE/out" 2>&1
		fi
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

