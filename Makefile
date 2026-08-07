CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic

SOURCES := code/provided_control.c code/sil_environment.c code/main.c
BINARY := dc_motor_sil

.PHONY: all run clean

all: $(BINARY)

$(BINARY): $(SOURCES) code/sil_api.h
	$(CC) $(CFLAGS) -Icode $(SOURCES) -o $(BINARY)

run: $(BINARY)
	./$(BINARY)

clean:
	-$(RM) $(BINARY) $(BINARY).exe *.o
