all:
	gcc -Wall -g -o main *.c
run:
	./main
valgrind:
	valgrind --leak-check=full ./main
clean:
	rm -f main