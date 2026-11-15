$ErrorActionPreference = "Stop"
# Relative paths: g++ under Windows PowerShell 5.1 receives non-ASCII (e.g. Cyrillic) absolute paths mangled.
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force -Path "build" | Out-Null
    g++ -O2 -std=c++17 -fopenmp -o build/monte_carlo.exe src/main.cpp
    if ($LASTEXITCODE -ne 0) { throw "C++ build failed" }
    Write-Host "Built cpp/build/monte_carlo.exe"
} finally {
    Pop-Location
}
