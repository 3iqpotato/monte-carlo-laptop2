param(
    [string]$Machine = $env:COMPUTERNAME,
    [long[]]$Sizes = @(10000, 100000, 1000000, 10000000, 100000000),
    [int[]]$Threads = @(),
    [int]$Repeats = 5,
    [int]$Warmup = 3,
    [long]$Seed = 42
)
$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    if ($Threads.Count -eq 0) {
        # Powers of two plus the points where the core type changes. On Intel hybrid CPUs only
        # P-cores have hyper-threading, so P = logical - physical (e.g. 6P+8E/20 -> 6, 12, 14, 20).
        $cpus = @(Get-CimInstance Win32_Processor)
        $physical = [int]($cpus | Measure-Object -Property NumberOfCores -Sum).Sum
        $logical = [int]($cpus | Measure-Object -Property NumberOfLogicalProcessors -Sum).Sum
        $ht = $logical - $physical
        $Threads = @(1, 2, 4, 8, 16, $ht, (2 * $ht), $physical, $logical) |
            Where-Object { $_ -ge 1 -and $_ -le $logical } | Sort-Object -Unique
    }

    & .\experiments\machine_info.ps1 -Machine $Machine | Out-Null

    & .\cpp\build.ps1
    & .\java\build.ps1

    New-Item -ItemType Directory -Force -Path "experiments\results" | Out-Null
    $outFile = "experiments\results\results_$Machine.csv"
    if (-not (Test-Path $outFile)) {
        "language,mode,n,threads,seed,repeat_index,time_seconds,estimate,abs_error,rel_error,samples_per_sec,machine" |
            Set-Content -Encoding ascii -Path $outFile
    }

    function Invoke-Run([string]$lang, [string[]]$runArgs) {
        $common = @("--seed", "$Seed", "--repeats", "$Repeats", "--warmup", "$Warmup", "--machine", $Machine)
        if ($lang -eq "cpp") {
            $lines = & .\cpp\build\monte_carlo.exe @runArgs @common
        } else {
            $lines = & java -cp java\out Main @runArgs @common
        }
        if ($LASTEXITCODE -ne 0) { throw "$lang run failed: $runArgs" }
        $lines | Where-Object { $_ -notmatch '^language,' } | Add-Content -Encoding ascii -Path $outFile
    }

    Write-Host "Machine=$Machine Sizes=$($Sizes -join ',') Threads=$($Threads -join ',') Repeats=$Repeats"
    foreach ($n in $Sizes) {
        foreach ($lang in @("cpp", "java")) {
            Invoke-Run $lang @("--mode", "seq", "--n", "$n")
            foreach ($t in $Threads) {
                Invoke-Run $lang @("--mode", "par", "--n", "$n", "--threads", "$t")
            }
        }
        Write-Host "Done N=$n"
    }
    Write-Host "Results appended to $outFile"
} finally {
    Pop-Location
}
