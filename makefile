bin/main: bin/main.o bin/game.o bin/ObjectField.o
	g++ -g -o bin/main bin/main.o bin/game.o bin/ObjectField.o -std=c++11 -Wall -pedantic -lncurses

bin/main.o: src/main.cpp
	mkdir -p bin
	g++ -g -c -o bin/main.o src/main.cpp -std=c++11 -Wall -pedantic

bin/game.o: src/game.cpp src/game.h src/ObjectField.h
	mkdir -p bin
	g++ -g -c -o bin/game.o src/game.cpp -std=c++11 -Wall -pedantic

bin/ObjectField.o: src/ObjectField.cpp src/ObjectField.h
	mkdir -p bin
	g++ -g -c -o bin/ObjectField.o src/ObjectField.cpp -std=c++11 -Wall -pedantic
