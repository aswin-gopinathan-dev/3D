
build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o app

run:
	./app

clean:
	rm app
 
