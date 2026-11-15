param([string]$Machine = $env:COMPUTERNAME)
# Writes the hardware/software description of this machine to experiments/results/machine_<Machine>.txt
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$cpus = @(Get-CimInstance Win32_Processor)
$cores = ($cpus | Measure-Object -Property NumberOfCores -Sum).Sum
$logical = ($cpus | Measure-Object -Property NumberOfLogicalProcessors -Sum).Sum
$ram = [math]::Round((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory / 1GB, 1)
$os = Get-CimInstance Win32_OperatingSystem

$lines = @(
    "machine=$Machine",
    "cpu=$($cpus[0].Name.Trim())",
    "physical_cores=$cores",
    "logical_processors=$logical",
    "ht_capable_cores=$($logical - $cores)",
    "ram_gb=$ram",
    "os=$($os.Caption) $($os.Version)",
    "gpp=$((cmd /c 'g++ --version 2>&1') | Select-Object -First 1)",
    "java=$(((cmd /c 'java -version 2>&1') | Select-Object -First 2 | ForEach-Object { $_.Trim() }) -join ' | ')"
)
$dir = Join-Path $root "experiments\results"
New-Item -ItemType Directory -Force -Path $dir | Out-Null
$lines | Set-Content -Encoding ascii -Path (Join-Path $dir "machine_$Machine.txt")
$lines
