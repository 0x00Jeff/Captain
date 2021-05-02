CC=x86_64-w64-mingw32-gcc

text.exe: test.o
	$(CC) test.o -o test.exe -s
	cp test.exe /mnt/1/Users/jeff/Desktop/game_hacking/
	#cp test.exe share/
	rm -- test.o 

game_hack.o: test.c test.h
	$(CC) -c test.c
