main: build/main.o build/argparser.o
	g++ build/main.o build/argparser.o -I include -o build/main

build/main.o: src/main.cpp
	g++ -c src/main.cpp -I include -o build/main.o

build/argparser.o: src/argparser.cpp
	g++ -c src/argparser.cpp -I include -o build/argparser.o

clean:
	rm -f ./build/*