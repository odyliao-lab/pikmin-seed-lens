param([Parameter(Mandatory=$true)][string]$NdkPath,[switch]$Api34Research,[switch]$Api34SingleThreadResearch,[switch]$Api34AospResearch,[switch]$Api34AospOneCoreResearch,[switch]$Api34AospMultiResearch)
$ErrorActionPreference='Stop'
if($Api34SingleThreadResearch -and !$Api34Research){throw 'Single-thread research requires explicit API34 research'}
if($Api34AospResearch -and !($Api34Research -and ($Api34SingleThreadResearch -or $Api34AospMultiResearch))){throw 'AOSP research requires explicit API34 scheduling research'}
if($Api34AospOneCoreResearch -and !($Api34Research -and $Api34SingleThreadResearch -and $Api34AospResearch)){throw 'One-core research requires explicit API34 AOSP single-thread research'}
if($Api34AospMultiResearch -and (!($Api34Research -and $Api34AospResearch) -or $Api34SingleThreadResearch -or $Api34AospOneCoreResearch)){throw 'Multi-thread research requires API34 AOSP and refuses single-thread/one-core scopes'}
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root ('.tools/research/tcg-software-selection-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out | Out-Null
$clang=Join-Path $NdkPath 'toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe'
$defines=@()
$library='libseedlens-tcg-selection-v1.so'
if($Api34Research){$defines=@('-DSEEDLENS_API34_HOST_RESEARCH=1');$library='libseedlens-api34-tcg-selection-v1.so'}
if($Api34SingleThreadResearch){$defines+='-DSEEDLENS_API34_TCG_SINGLE_RESEARCH=1';$library='libseedlens-api34-tcg-single-selection-v1.so'}
if($Api34AospResearch){$defines+='-DSEEDLENS_API34_AOSP_HOST_RESEARCH=1';$library='libseedlens-api34-aosp-tcg-single-selection-v1.so'}
if($Api34AospOneCoreResearch){$defines+='-DSEEDLENS_API34_AOSP_ONE_CORE_RESEARCH=1';$library='libseedlens-api34-aosp-tcg-onecore-selection-v1.so'}
if($Api34AospMultiResearch){$defines+='-DSEEDLENS_API34_AOSP_MULTI_RESEARCH=1';$library='libseedlens-api34-aosp-tcg-multi-selection-v1.so'}
& $clang --target=x86_64-unknown-linux-gnu -fuse-ld=lld -std=c11 -O2 -Wall -Wextra -Werror `
    -fPIC -fvisibility=hidden -fno-stack-protector -nostdlib -shared `
    '-Wl,--build-id=sha1' "-Wl,-soname,$library" @defines `
    "$PSScriptRoot/tcg_software_selection.c" -o "$out/$library"
if($LASTEXITCODE){throw 'Linux HOST research helper build failed'}
Get-FileHash -Algorithm SHA256 "$out/$library"
Write-Output "PRIVATE_BUILD=$out"
