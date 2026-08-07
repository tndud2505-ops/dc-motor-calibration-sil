$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$sourceFiles = @("provided_control.c", "sil_environment.c", "main.c")

Push-Location $projectRoot
try {
    $compiler = Get-Command gcc -ErrorAction SilentlyContinue
    if ($null -eq $compiler) {
        $compiler = Get-Command clang -ErrorAction SilentlyContinue
    }

    if ($null -ne $compiler) {
        Write-Host "[BUILD] Native compiler: $($compiler.Name)"
        $arguments = @(
            "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic"
        ) + $sourceFiles + @("-o", "dc_motor_sil.exe")

        & $compiler.Source @arguments
        if ($LASTEXITCODE -ne 0) {
            throw "C compilation failed."
        }

        Write-Host "[RUN] dc_motor_sil.exe"
        & (Join-Path $projectRoot "dc_motor_sil.exe")
        if ($LASTEXITCODE -ne 0) {
            throw "SIL execution failed."
        }
        exit 0
    }

    $wsl = Get-Command wsl.exe -ErrorAction SilentlyContinue
    if ($null -eq $wsl) {
        throw "No C compiler was found. Install WSL with build-essential, GCC, or Clang. See LOCAL_BUILD.md."
    }

    if ($projectRoot -notmatch '^([A-Za-z]):\\(.*)$') {
        throw "WSL fallback supports project folders on a Windows drive such as C: or D:."
    }

    $driveLetter = $Matches[1].ToLowerInvariant()
    $relativePath = $Matches[2] -replace '\\', '/'
    $wslProjectRoot = "/mnt/$driveLetter/$relativePath"

    if ($wslProjectRoot.Contains("'")) {
        throw "The project path cannot contain a single quote when WSL fallback is used."
    }

    $wslCommand = "cd '$wslProjectRoot' && cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic provided_control.c sil_environment.c main.c -o dc_motor_sil && ./dc_motor_sil"

    Write-Host "[BUILD] WSL compiler: cc"
    Write-Host "[RUN] WSL dc_motor_sil"
    & wsl.exe sh -lc $wslCommand
    if ($LASTEXITCODE -ne 0) {
        throw "WSL compilation or SIL execution failed."
    }
}
catch {
    Write-Host "[ERROR] $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
finally {
    Pop-Location
}
