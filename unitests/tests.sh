#!/bin/bash

FILES=`ls unitests`
for FILE in $FILES; do
	if [ -d "unitests/$FILE" ]; then
		echo "*********************************"
		echo "  Test: $FILE"
		echo "*********************************"
		unitests/$FILE/test
   	if [ "$?" == "0" ]; then
			echo "Korektni";
		fi
	fi
done

