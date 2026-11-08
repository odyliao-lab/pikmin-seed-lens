param([Parameter(Mandatory=$true)][string]$NdkPath)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root '.tools/research'
New-Item -ItemType Directory -Force $out|Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe'
& $clang --target=x86_64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -static-libstdc++ "$PSScriptRoot/trace_probe.cpp" -o "$out/trace-probe"
if($LASTEXITCODE){throw 'Isolated trace probe build failed'}
Get-FileHash -Algorithm SHA256 "$out/trace-probe"
