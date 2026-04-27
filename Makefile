SRCDIR=.\src
INCDIR=.\include
INCLUDE = -IC:\msys64\mingw64\include\SDL2 -I$(INCDIR) 
CFLAGS = -g $(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -mwindows -mconsole -lm

CM1008_Project_1: main.o player.o map.o physics.o weapon.o sound.o
	gcc -o CM1008_Project_1 main.o player.o map.o physics.o weapon.o sound.o $(LDFLAGS)

main.o: $(SRCDIR)\main.c
	gcc $(CFLAGS) $(SRCDIR)\main.c

player.o: $(SRCDIR)\player.c $(INCDIR)\player.h
	gcc $(CFLAGS) $(SRCDIR)\player.c

map.o: $(SRCDIR)\map.c $(INCDIR)\map.h
	gcc $(CFLAGS) $(SRCDIR)\map.c

physics.o: $(SRCDIR)\physics.c $(INCDIR)\physics.h
	gcc $(CFLAGS) $(SRCDIR)\physics.c

weapon.o: $(SRCDIR)\weapon.c $(INCDIR)\weapon.h
	gcc $(CFLAGS) $(SRCDIR)\weapon.c

sound.o: $(SRCDIR)\sound.c $(INCDIR)\sound.h
	gcc $(CFLAGS) $(SRCDIR)\sound.c

clean:
	del $(OBJDIR)\server_main.o
	del $(OBJDIR)\client_main.o
	del $(UBJDIR)\menu.o
	del $(OBJDIR)\map.o
	del $(OBJDIR)\physics.o
	del $(OBJDIR)\player.o
	del $(OBJDIR)\weapon.o
	del server.exe
	del client.exe