$ErrorActionPreference = "Stop"
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force -Path "out" | Out-Null
    $files = Get-ChildItem -Path "src" -Filter "*.java" | ForEach-Object { "src/" + $_.Name }
    javac -encoding UTF-8 -d out @files
    if ($LASTEXITCODE -ne 0) { throw "Java build failed" }
    Write-Host "Built classes in java/out"
} finally {
    Pop-Location
}
