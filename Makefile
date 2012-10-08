CFLAGS=-std=c99 -I src -Wall

SOURCES=ifj12 value symbols

ifj12: $(addprefix obj/,$(addsuffix .o,$(SOURCES)))
	gcc -o $@ $^ $(CFLAGS)

obj/%.o: src/%.c
	mkdir -p dep obj # Adresare nejsou v gitu
	gcc -MMD -MP -MF dep/$*.d -c -o $@ $< $(CFLAGS)

-include $(addprefix dep/,$(addsuffix .d,$(SOURCES)))


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
	rm -f obj/*.o dep/*.d ./ifj12 unitests/*/test unitests/*/test.o unitests/*/out



