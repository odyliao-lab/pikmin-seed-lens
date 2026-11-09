param([Parameter(Mandatory=$true)][string]$NdkPath)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root ('.tools/research/tcg-mmu-vector-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out | Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe'
& $clang --target=x86_64-unknown-linux-gnu -fuse-ld=lld -nostdlib -shared `
    '-Wl,-Bsymbolic' '-Wl,-z,now' '-Wl,-z,relro' '-Wl,--build-id=sha1' '-Wl,-z,noexecstack' `
    "$PSScriptRoot/tcg_mmu_vector_save.S" -o "$out/libseed-tcg-mmu-save.so"
if($LASTEXITCODE){throw 'Linux host vector wrapper build failed'}
& $clang --target=x86_64-unknown-linux-gnu -fuse-ld=lld -nostdlib -static `
    '-Wl,-e,_start' '-Wl,-z,noexecstack' "$PSScriptRoot/tcg_mmu_vector_test.S" `
    "$PSScriptRoot/tcg_mmu_vector_save.S" -o "$out/vector-host-test"
if($LASTEXITCODE){throw 'Linux host ABI fixture build failed'}
Get-FileHash -Algorithm SHA256 "$out/libseed-tcg-mmu-save.so","$out/vector-host-test"
Write-Output "PRIVATE_HOST_RESEARCH_BUILD=$out; compiled only, Linux execution required"
