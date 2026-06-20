
build:
	gcc -O3 -Wall -std=gnu99 ./src/*.c -lSDL2 -lm -o app

run:
	./app

clean:
	rm app
 
