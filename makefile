CC=clang
CFLAGS=-std=c99 -Wall -pedantic
EXECS=a6

all: $(EXECS)

clean:
	rm -i $($EXECS) *.o

%: %.o
%: %.c


mainFile:  mainFile.o hash.h hash.o
	$(CC) $(CFLAGS) mainFile.o hash.o -o mainFile

# create .o files by compiling the .c files
%.o : %.c hash.h
	$(CC) -c $(CFLAGS) $< -o $@
