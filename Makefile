helloSDL: main.o
	gcc -o CM1008_Project_1 main.o -lmingw32 -lSDL2main -lSDL2

main.o: .\main.c
	gcc -c -g -IC:\msys64\mingw64\include\SDL2 .\main.c

clean:
	rm *.exe
	rm *.o