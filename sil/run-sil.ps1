param(
    [string] $ControlSource = 'common/dc_motor_control.c'
)
$ErrorActionPreference = 'Stop'
$packageRoot = Split-Path -Parent $PSScriptRoot
& make -C $packageRoot sil CONTROL_SOURCE=$ControlSource
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
