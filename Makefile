all: ./src/main.c
	@ mkdir -p bin
	@ gcc ./src/main.c -o ./bin/elfparser
	@ chmod +x ./bin/elfparser

install:
	gcc ./src/main.c -o ./bin/elfparser
	cp bin/elfparser /usr/bin/
  
test:
	gcc ./src/main.c -o ./bin/elfparser
	chmod +x ./bin/elfparser
	./bin/elfparser ./tests/test
	readelf -h ./tests/test