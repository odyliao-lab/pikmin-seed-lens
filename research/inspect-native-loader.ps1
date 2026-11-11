param(
 [Parameter(Mandatory=$true)][string]$ReadElf,
 [Parameter(Mandatory=$true)][string]$LibraryDirectory,
 [ValidateSet('libnb.so','libndk_translation.so')][string]$BridgeLibrary='libnb.so',
 [switch]$IncludeArt
)
$ErrorActionPreference='Stop'
$wanted=@{
 'libnativeloader.so'=@('FindNativeLoaderNamespaceByClassLoader','FindNamespaceByClassLoader','OpenNativeLibraryInNamespace','IsNamespaceNativeBridged','NativeLoaderFreeErrorMessage')
 'libnativebridge.so'=@('NativeBridgeInitialized','NativeBridgeGetVersion','NativeBridgeGetExportedNamespace','NativeBridgeCreateNamespace','NativeBridgeLoadLibraryExt','NativeBridgeGetTrampoline','NativeBridgeGetTrampoline2')
}
$wanted[$BridgeLibrary]=@('NativeBridgeItf')
if($IncludeArt){$wanted['libart.so']=@('JNI_GetCreatedJavaVMs')}
foreach($name in ($wanted.Keys|Sort-Object)){
 $path=Join-Path $LibraryDirectory $name
 $symbols=& $ReadElf --dyn-syms $path
 if($LASTEXITCODE){throw "ELF inspection failed: $name"}
 $available=@($symbols|Where-Object{$_ -match '\bGLOBAL\b' -and $_ -notmatch '\bUND\b'}|ForEach-Object{($_.Trim() -split '\s+')[-1] -replace '@.*$',''})
 $present=@($wanted[$name]|Where-Object{$_ -in $available})
 $missing=@($wanted[$name]|Where-Object{$_ -notin $available})
 [pscustomobject]@{
  library=$name;sha256=(Get-FileHash $path -Algorithm SHA256).Hash.ToLowerInvariant()
  present=$present;missing=$missing
  conclusion='Export presence only; no call, namespace ownership, loading or renderer acceptance'
 }|ConvertTo-Json -Compress
}
