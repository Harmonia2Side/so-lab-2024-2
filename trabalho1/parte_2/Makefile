all:
	gcc -Wall -g -o main.exe *.c
valgrind:
	valgrind --leak-check=full ./main.exe
clean:
	rm -f main.exe