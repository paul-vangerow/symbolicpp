main: src/main.cpp
	g++ src/main.cpp -o build/main

clean:
	rm -f ./build/*