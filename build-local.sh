#!/usr/bin/env sh
set -eu

cd "$(dirname "$0")"

COMPILER="${CC:-cc}"
OUTPUT="dc_motor_sil"

echo "[BUILD] Compiler: $COMPILER"
"$COMPILER" -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode \
    code/provided_control.c code/sil_environment.c code/main.c \
    -o "$OUTPUT"

echo "[RUN] $OUTPUT"
./"$OUTPUT"
