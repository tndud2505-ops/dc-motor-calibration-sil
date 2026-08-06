CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic

SIL_DIR := sil
CONTROL_SOURCE ?= $(SIL_DIR)/dc_motor_control.c
SIL_SOURCES := $(CONTROL_SOURCE) \
	$(SIL_DIR)/sil_environment.c \
	$(SIL_DIR)/sil_main.c
SIL_BINARY := $(SIL_DIR)/dc_motor_sil

.PHONY: all sil clean

all: sil

sil: $(SIL_BINARY)
	$(SIL_BINARY)

$(SIL_BINARY): $(SIL_SOURCES) $(SIL_DIR)/dc_motor_control.h $(SIL_DIR)/sil_environment.h
	$(CC) $(CFLAGS) -I$(SIL_DIR) $(SIL_SOURCES) -o $(SIL_BINARY)

clean:
	-$(RM) $(SIL_BINARY) $(SIL_DIR)/*.o
