param([Parameter(Mandatory=$true)][string]$NdkPath)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root ('.tools/research/arm64-fixture-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out | Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe'
# Deliberately bounded FPSIMD-only research. No product/CMake/APK modification.
& $clang --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror `
    -mbranch-protection=none -static-libstdc++ "$PSScriptRoot/arm64_call_fixture.cpp" -o "$out/arm64-call-fixture"
if($LASTEXITCODE){throw 'ARM64 isolated fixture build failed'}
Get-FileHash -Algorithm SHA256 "$out/arm64-call-fixture"
