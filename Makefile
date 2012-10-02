CFLAGS=-std=c99 -I src

ifj12: obj/ifj12.o obj/value.o obj/symbols.o
	gcc -o $@ $^ $(CFLAGS)

obj/ifj12.o: src/ifj12.c
	mkdir -p obj # Adresar obj neni v gitu
	gcc -c -o $@ $^ $(CFLAGS)

obj/value.o: src/value.c
	mkdir -p obj # Adresar obj neni v gitu
	gcc -c -o $@ $^ $(CFLAGS)

obj/symbols.o: src/symbols.c
	mkdir -p obj # Adresar obj neni v gitu
	gcc -c -o $@ $^ $(CFLAGS)

#  Testy
#  *****
.PHONY: test unitests/value/test
test:   unitests/value/test
	unitests/tests.sh
unitests/value/test:
	gcc -c -o unitests/value/test.o unitests/value/test.c $(CFLAGS)
	gcc -o $@ unitests/value/test.o obj/value.o $(CFLAGS)
	unitests/value/test > unitests/value/out

clean:
	rm -f obj/*.o ./ifj12 unitests/*/test unitests/*/test.o unitests/*/out

