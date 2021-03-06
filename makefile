all: uno

cli.o: cli.h cli.c
	gcc -c cli.c -Wall -Wextra

file.o: file.h file.c
	gcc -c file.c -Wall -Wextra

stack.o: stack.h stack.c
	gcc -c stack.c -Wall -Wextra

game.o: game.h game.c
	gcc -c game.c -Wall -Wextra

player.o: player.h player.c
	gcc -c player.c -Wall -Wextra

list.o: list.h list.c
	gcc -c list.c -Wall -Wextra

listbi.o: listbi.h listbi.c
	gcc -c listbi.c -Wall -Wextra

main.o: cli.h file.h stack.h game.h player.h list.h listbi.h main.c
	gcc -c main.c

uno: cli.o file.o stack.o game.o player.o list.o listbi.o main.o
	gcc cli.o file.o stack.o game.o player.o list.o listbi.o main.o -o uno

clean:
	rm -f *.o uno