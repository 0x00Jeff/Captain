FILE=		Captain.exe
OBJ	=		$(FILE:.exe=.o)
SRC	=		$(FILE:.exe=.c)
HEAD=		$(FILE:.exe=.h)

CC=			x86_64-w64-mingw32-gcc
CFLAGS=		-Wall -Wextra -s

$(FILE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@


$(OBJ): $(HEAD)