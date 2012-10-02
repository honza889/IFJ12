CFLAGS=-std=c99 -I src

ifj12: obj/ifj12.o obj/value.o obj/symbols.o
	gcc -o $@ $^ $(CFLAGS)

obj/%.o: src/%.c
	mkdir -p obj # Adresar obj neni v gitu
	gcc -c -o $@ $^ $(CFLAGS)


#  Testy
#  *****

TESTS=value symbols

.PHONY: test
test:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh

unitests/value/test: obj/value.o
unitests/symbols/test: obj/symbols.o obj/value.o

unitests/%/test: unitests/%/test.c
	gcc -o $@ $^ $(CFLAGS)





clean:
	rm -f obj/*.o ./ifj12 unitests/*/test unitests/*/test.o unitests/*/out



