main:
	gcc -o main src/main.c src/server.c src/routes.c

clean:
	rm ./main
