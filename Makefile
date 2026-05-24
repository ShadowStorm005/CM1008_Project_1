SRCDIR  = src
INCDIR  = include
OBJDIR  = OBJ_FILES

INCLUDE  = -IC:/msys64/mingw64/include/SDL2 -I$(INCDIR)
CFLAGS   = -g $(INCLUDE) -c
LDFLAGS  = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -mconsole -lm

SERVER_OBJ = $(OBJDIR)/server_main.o $(OBJDIR)/map.o $(OBJDIR)/physics.o $(OBJDIR)/player.o $(OBJDIR)/weapon.o
CLIENT_OBJ = $(OBJDIR)/client_main.o $(OBJDIR)/menu.o $(OBJDIR)/map.o $(OBJDIR)/physics.o $(OBJDIR)/player.o $(OBJDIR)/weapon.o

all: server.exe client.exe

server.exe: $(SERVER_OBJ)
	gcc $(SERVER_OBJ) -o server.exe $(LDFLAGS) $(INCLUDE)

client.exe: $(CLIENT_OBJ)
	gcc $(CLIENT_OBJ) -o client.exe $(LDFLAGS) $(INCLUDE)

$(OBJDIR)/server_main.o: server/server_main.c $(INCDIR)/server_net.h $(INCDIR)/game_net.h $(INCDIR)/server_creation_functions.h | $(OBJDIR) # server compiler
	gcc $(CFLAGS) server/server_main.c -o $(OBJDIR)/server_main.o

$(OBJDIR)/client_main.o: client/client_main.c $(INCDIR)/client_net.h $(INCDIR)/game_net.h $(INCDIR)/menu.h | $(OBJDIR) # client compiler
	gcc $(CFLAGS) client/client_main.c -o $(OBJDIR)/client_main.o

$(OBJDIR)/menu.o: $(SRCDIR)/menu.c $(INCDIR)/menu.h $(INCDIR)/map.h| $(OBJDIR) # physics compiler
	gcc $(CFLAGS) $(SRCDIR)/menu.c -o $(OBJDIR)/menu.o

$(OBJDIR)/map.o: $(SRCDIR)/map.c $(INCDIR)/map.h $(INCDIR)/server_creation_functions.h | $(OBJDIR) # map compiler
	gcc $(CFLAGS) $(SRCDIR)/map.c -o $(OBJDIR)/map.o

$(OBJDIR)/physics.o: $(SRCDIR)/physics.c $(INCDIR)/physics.h | $(OBJDIR) # physics compiler
	gcc $(CFLAGS) $(SRCDIR)/physics.c -o $(OBJDIR)/physics.o

$(OBJDIR)/player.o: $(SRCDIR)/player.c $(INCDIR)/player.h $(INCDIR)/server_creation_functions.h | $(OBJDIR) # player compiler
	gcc $(CFLAGS) $(SRCDIR)/player.c -o $(OBJDIR)/player.o

$(OBJDIR)/weapon.o: $(SRCDIR)/weapon.c $(INCDIR)/weapon.h $(INCDIR)/server_creation_functions.h | $(OBJDIR) # weapon compiler
	gcc $(CFLAGS) $(SRCDIR)/weapon.c -o $(OBJDIR)/weapon.o

$(OBJDIR):
	if not exist $(OBJDIR) mkdir $(OBJDIR)

clean:
	del $(OBJDIR)\server_main.o
	del $(OBJDIR)\client_main.o
	del $(OBJDIR)\menu.o
	del $(OBJDIR)\map.o
	del $(OBJDIR)\physics.o
	del $(OBJDIR)\player.o
	del $(OBJDIR)\weapon.o
	del server.exe
	del client.exe