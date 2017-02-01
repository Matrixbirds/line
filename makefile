CC=gcc
CFLAGS=-g -Wall -std=c99
SRCS := main.c http.c server.c
PSRCS := $(SRCS:.c=.e)
OBJ := $(SRCS:.c=.o)
default: main
main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)
%.o: %.h
%.e: %.c
	$(CC) $(CFLAGS) -E $< -o $@
clean:
	$(RM) $(OBJ) main *.e
precompile: $(PSRCS)
