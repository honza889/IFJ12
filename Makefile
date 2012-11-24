# Parametry překladu
CFLAGS=-std=c99 -I src -Wall -pedantic -Wno-unused-variable
# Úroveň množství debugovacích informací
LDB=-g3
# Parametry slinkování
LINK=-lm

SOURCES=ifj12 value symbols ast exceptions rcstring bif scanner syntax ial alloc

ifj12: $(addprefix obj/,$(addsuffix .o,$(SOURCES)))
	gcc -o $@ $^ $(CFLAGS) $(LINK)

obj/%.o: src/%.c
	mkdir -p dep obj # Adresare nejsou v gitu
	gcc -MMD -MP -MF dep/$*.d -c -o $@ $< $(CFLAGS) -DNDEBUG

obj/dbg/%.o: src/%.c
	mkdir -p dep/dbg obj/dbg # Adresare nejsou v gitu
	gcc -MMD -MP -MF dep/dbg/$*.d -c -o $@ $< $(CFLAGS) $(LDB)

-include $(addprefix dep/,$(addsuffix .d,$(SOURCES)))


#  Testy
#  *****

TESTS=value symbols ast exceptions scanner rcstring ial syntax interpret parsexp

.PHONY: test etest debug clean pack
test:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh "$?"
tests:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh valgrind "$?"

unitests/value/test: obj/dbg/value.o obj/dbg/exceptions.o obj/dbg/alloc.o obj/dbg/ast.o
unitests/symbols/test: obj/dbg/symbols.o obj/dbg/value.o obj/dbg/exceptions.o obj/dbg/alloc.o obj/dbg/ast.o
unitests/ast/test: obj/dbg/value.o obj/dbg/ast.o obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/exceptions/test: obj/dbg/exceptions.o obj/dbg/alloc.o
unitests/scanner/test: obj/dbg/scanner.o obj/dbg/rcstring.o obj/dbg/alloc.o
unitests/syntax/test: obj/dbg/syntax.o obj/dbg/scanner.o obj/dbg/symbols.o obj/dbg/rcstring.o obj/dbg/value.o obj/dbg/ast.o obj/dbg/alloc.o obj/dbg/bif.o obj/dbg/ial.o
unitests/interpret/test: obj/dbg/syntax.o obj/dbg/scanner.o obj/dbg/symbols.o obj/dbg/rcstring.o obj/dbg/value.o obj/dbg/ast.o obj/dbg/alloc.o obj/dbg/bif.o obj/dbg/ial.o
unitests/rcstring/test: obj/dbg/rcstring.o obj/dbg/alloc.o
unitests/ial/test: obj/dbg/ial.o obj/dbg/value.o obj/dbg/alloc.o obj/dbg/ast.o
unitests/parsexp/test: obj/dbg/parsexp.o  obj/dbg/bif.o obj/dbg/ial.o obj/dbg/scanner.o obj/dbg/symbols.o obj/dbg/rcstring.o obj/dbg/value.o obj/dbg/ast.o obj/dbg/alloc.o

unitests/%/test: unitests/%/test.c obj/dbg/exceptions.o obj/dbg/rcstring.o unitests/test.h
	gcc -o $@ $^ $(LDB) $(CFLAGS) $(LINK)

etest: debug
	unitests/extern/test.sh

#  Debug
#  *****

debug: $(addprefix obj/dbg/,$(addsuffix .o,$(SOURCES)))
	gcc -o ifj12-dbg $^ $(CFLAGS) $(LDB) -lm

clean:
	rm -f obj/*.o obj/dbg/*.o dep/*.d dep/dbg/*.d ./ifj12 ./ifj12-dbg
	rm -f unitests/*/test unitests/*/test.o unitests/*/out
	rm -f pack/ifj12 pack/*.*

pack:
	bash -c 'cp src/*[^~] pack/; cd pack; tar -zcvf ../xbiber00.tar.gz *'
