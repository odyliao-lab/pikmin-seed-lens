param([Parameter(Mandatory=$true)][string]$NdkPath)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root ('.tools/research/tcg-software-selection-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out | Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe'
& $clang --target=x86_64-unknown-linux-gnu -fuse-ld=lld -std=c11 -O2 -Wall -Wextra -Werror `
    -fPIC -fvisibility=hidden -fno-stack-protector -nostdlib -shared `
    '-Wl,--build-id=sha1' '-Wl,-soname,libseedlens-tcg-selection-v1.so' `
    "$PSScriptRoot/tcg_software_selection.c" -o "$out/libseedlens-tcg-selection-v1.so"
if($LASTEXITCODE){throw 'Linux HOST research helper build failed'}
Get-FileHash -Algorithm SHA256 "$out/libseedlens-tcg-selection-v1.so"
Write-Output "PRIVATE_BUILD=$out"
