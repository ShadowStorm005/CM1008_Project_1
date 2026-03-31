SRCDIR=.\src
INCDIR=.\include
INCLUDE = -IC:\msys64\mingw64\include\SDL2 -I$(INCDIR) 
CFLAGS = -g $(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows -lm

CM1008_Project_1: main.o player.o
	gcc -o CM1008_Project_1 main.o player.o $(LDFLAGS)

main.o: $(SRCDIR)\main.c
	gcc $(CFLAGS) $(SRCDIR)\main.c

player.o: $(SRCDIR)\player.c $(INCDIR)\player.h
	gcc $(CFLAGS) $(SRCDIR)\player.c

#map.o: $(SRCDIR)\map.c $(INCDIR)\map.h
#	gcc $(CFLAGS) $(SRCDIR)\map.c

clean:
	rm *.exe
	rm *.o