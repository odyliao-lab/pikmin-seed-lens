param([Parameter(Mandatory=$true)][string]$NdkPath,[switch]$FaultDiagnostics)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root ('.tools/research/tcg-memory-init-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out|Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe'
$defines=@()
if($FaultDiagnostics){$defines+= '-DSEED_TCG_FAULT_DIAGNOSTICS=1'}
& $clang --target=aarch64-linux-android31 -std=c++20 -O2 -Wall -Wextra -Werror `
    -mbranch-protection=none -static @defines "$PSScriptRoot/tcg_memory_init.cpp" -o "$out/init"
if($LASTEXITCODE){throw 'Isolated CPU init fixture build failed'}
python "$PSScriptRoot/make_cpu_initramfs.py" --binary "$out/init" --output "$out/cpu-probe.cpio.gz"
if($LASTEXITCODE){throw 'Research ramdisk creation failed'}
Get-FileHash -Algorithm SHA256 "$out/init","$out/cpu-probe.cpio.gz"
Write-Output "PRIVATE_BUILD=$out"
