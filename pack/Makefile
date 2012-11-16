#
# Interpretr imperativního jazyka IFJ12
#
# @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
# @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
# @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
# @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
# @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
#

# PRODUKCNI MAKEFILE

SOURCES=ifj12 value symbols ast exceptions rcstring bif scanner syntax ial alloc

CFLAGS=-std=c99 -Wall -Wno-unused-variable -pedantic -DNDEBUG -lm

ifj12: $(addsuffix .c,$(SOURCES))
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm ifj12

