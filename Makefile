CFLAGS=-std=c99 -I src -Wall -Wno-unused-variable -pedantic
# Úroveň množství debugovacích informací
LDB=-g3

SOURCES=ifj12 value symbols ast exceptions

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

TESTS=value symbols ast exceptions

.PHONY: test debug clean
test:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh
tests:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh valgrind

unitests/value/test: obj/value.o obj/exceptions.o
unitests/symbols/test: obj/symbols.o obj/value.o obj/exceptions.o
unitests/ast/test: obj/value.o obj/ast.o obj/exceptions.o
unitests/exceptions/test: obj/exceptions.o

unitests/%/test: unitests/%/test.c obj/exceptions.o unitests/test.h
	gcc -o $@ $^ $(CFLAGS)

#  Debug
#  *****

debug: $(addprefix obj/dbg/,$(addsuffix .o,$(SOURCES)))
	gcc -o ifj12-dbg $^ $(CFLAGS) $(LDB)

clean:
	rm -f obj/*.o obj/dbg/*.o dep/*.d ./ifj12 ./ifj12-dbg unitests/*/test unitests/*/test.o unitests/*/out

