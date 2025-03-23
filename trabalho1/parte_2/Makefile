all:
	gcc -Wall -g -o main.exe *.c
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main.exe
clean:
	rm -f main.exe