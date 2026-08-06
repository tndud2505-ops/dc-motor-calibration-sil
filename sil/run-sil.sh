#!/usr/bin/env sh
set -eu

root_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
control_source=${CONTROL_SOURCE:-sil/dc_motor_control.c}
make -C "$root_dir" sil CONTROL_SOURCE="$control_source"
