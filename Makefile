CFLAGS=-std=c99 -I src -Wall -pedantic
# Úroveň množství debugovacích informací
LDB=-g3

SOURCES=ifj12 value symbols ast

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

TESTS=value symbols ast

.PHONY: test debug clean
test:  $(addprefix unitests/,$(addsuffix /test,$(TESTS)))
	unitests/tests.sh

unitests/value/test: obj/value.o
unitests/symbols/test: obj/symbols.o obj/value.o
unitests/ast/test: obj/value.o obj/ast.o

unitests/%/test: unitests/%/test.c
	gcc -o $@ $^ $(CFLAGS)



debug: $(addprefix obj/dbg/,$(addsuffix .o,$(SOURCES)))
	gcc -o ifj12-dbg $^ $(CFLAGS) $(LDB)


clean:
	rm -f obj/*.o obj/dbg/*.o dep/*.d ./ifj12 ./ifj12-dbg unitests/*/test unitests/*/test.o unitests/*/out



