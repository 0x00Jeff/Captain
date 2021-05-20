CC=x86_64-w64-mingw32-gcc

Captain.exe: Captain.o
	$(CC) Captain.o -o Captain.exe -s
	rm -- Captain.o 

Captain.o: Captain.c Captain.h
