CFLAGS=-std=c99 -I src -Wall -Wno-unused-variable -pedantic -Wdouble-promotion
# Úroveň množství debugovacích informací
LDB=-g3

SOURCES=ifj12 value symbols ast exceptions rcstring bif scanner syntax semantics ial alloc

ifj12: $(addprefix obj/,$(addsuffix .o,$(SOURCES)))
	gcc -o $@ $^ $(CFLAGS)

obj/%.o: src/%.c
	mkdir -p dep obj # Adresare nejsou v gitu
	gcc -MMD -MP -MF dep/$*.d -c -o $@ $< $(CFLAGS)

obj/dbg/%.o: src/%.c
	mkdir -p dep/dbg obj/dbg # Adresare nejsou v gitu
	gcc -MMD -MP -MF dep/dbg/$*.d -c -o $@ $< $(CFLAGS) $(LDB)

-include $(addprefix dep/,$(addsuffix .d,$(SOURCES)))


#  Testy
#  *****

TESTS=value symbols ast exceptions scanner rcstring ial semantics

.PHONY: test debug clean
test:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh "$?"
tests:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh valgrind "$?"

unitests/value/test: obj/dbg/value.o obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/symbols/test: obj/dbg/symbols.o obj/dbg/value.o obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/ast/test: obj/dbg/value.o obj/dbg/ast.o obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/exceptions/test: obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/scanner/test: obj/dbg/scanner.o obj/dbg/rcstring.o obj/dbg/alloc.o
unitests/semantics/test: obj/dbg/semantics.o obj/dbg/syntax.o obj/dbg/scanner.o obj/dbg/symbols.o obj/dbg/rcstring.o obj/dbg/value.o obj/dbg/ast.o obj/dbg/alloc.o
unitests/rcstring/test: obj/dbg/rcstring.o obj/dbg/alloc.o
unitests/ial/test: obj/dbg/ial.o obj/dbg/value.o obj/dbg/alloc.o

unitests/%/test: unitests/%/test.c obj/dbg/exceptions.o obj/dbg/rcstring.o unitests/test.h
	gcc -o $@ $^ $(LDB) $(CFLAGS)


#  Debug
#  *****

debug: $(addprefix obj/dbg/,$(addsuffix .o,$(SOURCES)))
	gcc -o ifj12-dbg $^ $(CFLAGS) $(LDB)

clean:
	rm -f obj/*.o obj/dbg/*.o dep/*.d ./ifj12 ./ifj12-dbg unitests/*/test unitests/*/test.o unitests/*/out

