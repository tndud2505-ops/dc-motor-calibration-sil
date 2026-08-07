CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic

SOURCES := provided_control.c sil_environment.c main.c
BINARY := dc_motor_sil

.PHONY: all run clean

all: $(BINARY)

$(BINARY): $(SOURCES) sil_api.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(BINARY)

run: $(BINARY)
	./$(BINARY)

clean:
	-$(RM) $(BINARY) $(BINARY).exe *.o
