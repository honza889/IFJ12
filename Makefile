CFLAGS=-std=c99 -I src

ifj12: obj/ifj12.o
	gcc -o $@ $^ $(CFLAGS)

obj/ifj12.o: src/ifj12.c
	mkdir -p obj # Adresar obj neni v gitu
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f obj/*.o ./ifj12

