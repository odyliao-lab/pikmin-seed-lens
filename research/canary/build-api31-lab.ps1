[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$CmakePath,
 [Parameter(Mandatory)][string]$NinjaPath,
 [switch]$Unsigned,
 [string]$SigningKey='',
 [string]$ExpectedCertificateSha256='',
 [string]$StorePassEnv='SEED_LENS_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
[xml]$probeManifest=Get-Content "$PSScriptRoot/AndroidManifest.api31.xml" -Raw
[xml]$carrierManifest=Get-Content "$PSScriptRoot/AndroidManifest.carrier.xml" -Raw
$versions=@{probe=[string]$probeManifest.manifest.versionName;carrier=[string]$carrierManifest.manifest.versionName}
foreach($value in $versions.Values){if($value -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid research version'}}
if(!$Unsigned){
 if(!$SigningKey -or $ExpectedCertificateSha256 -notmatch '^[0-9a-fA-F]{64}$'){throw 'Explicit external key and expected certificate required'}
 $key=(Resolve-Path -LiteralPath $SigningKey).Path
 if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'External key required'}
 foreach($name in @($StorePassEnv,$KeyPassEnv)){
  if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}
 }
 $reserved="$root/.tools/research/api31-lab-signed"
 foreach($kind in @('probe','carrier')){
  if(Test-Path "$reserved/$kind-$($versions[$kind])"){throw 'Signed research artifact/version already reserved; bump manifest version and code, never overwrite'}
 }
 New-Item -ItemType Directory $reserved -Force | Out-Null
 foreach($kind in @('probe','carrier')){New-Item -ItemType Directory "$reserved/$kind-$($versions[$kind])" | Out-Null}
}
$out=Join-Path $root ('.tools/research/api31-lab-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory "$out/classes","$out/dex","$out/fixture/lib/arm64-v8a","$out/payload/assets/bridge","$out/carrier/lib/x86_64" | Out-Null
$clang="$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe"
foreach($name in @('fixture','canary')){
 $library=if($name -eq 'fixture'){"$out/fixture/lib/arm64-v8a/libfixture.so"}else{"$out/payload/assets/bridge/libcanary.so"}
 & $clang --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -fPIC -shared -static-libstdc++ '-Wl,-z,max-page-size=16384' -fvisibility=hidden "$PSScriptRoot/$name.cpp" -o $library
 if($LASTEXITCODE){throw "ARM fixture build failed: $name"}
 $dynamic=& "$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-readelf.exe" -d $library
 if($LASTEXITCODE -or ($dynamic -match 'libc\+\+_shared')){throw 'Unpackaged runtime dependency'}
}
$build="$out/x64-build"
$common=@('-G','Ninja',"-DCMAKE_MAKE_PROGRAM=$NinjaPath","-DCMAKE_TOOLCHAIN_FILE=$NdkPath/build/cmake/android.toolchain.cmake",'-DANDROID_ABI=x86_64','-DANDROID_PLATFORM=android-28','-DANDROID_STL=c++_static','-DCMAKE_BUILD_TYPE=Release',"-DSEED_CANARY_LIBRARY=$out/payload/assets/bridge/libcanary.so",'-S',$PSScriptRoot,'-B',$build)
& $CmakePath @common
if($LASTEXITCODE){throw 'Research host configure failed'}
& $CmakePath --build $build --parallel 4 --target seedprobe_api31_host
if($LASTEXITCODE){throw 'API31 host build failed'}
$hostLibrary="$out/carrier/lib/x86_64/libseedprobe_api31_host.so"
Copy-Item -LiteralPath "$build/libseedprobe_api31_host.so" -Destination $hostLibrary
$elf=[IO.File]::ReadAllBytes($hostLibrary)
if($elf.Length -lt 4096 -or $elf.Length -gt 33554432 -or [BitConverter]::ToString($elf,0,7) -cne '7F-45-4C-46-02-01-01' -or [BitConverter]::ToString($elf,16,4) -cne '03-00-3E-00'){throw 'Carrier host must be x64 ELF64 shared object'}
& $CmakePath @common "-DSEED_API31_HOST_LIBRARY=$hostLibrary"
if($LASTEXITCODE){throw 'PM verifier configure failed'}
& $CmakePath --build $build --parallel 4 --target api31-canary-root
if($LASTEXITCODE){throw 'PM verifier build failed'}
Copy-Item -LiteralPath "$build/api31-canary-root" -Destination "$out/api31-canary-root"
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" "$PSScriptRoot/ProbeInstrumentation.java"
if($LASTEXITCODE){throw 'Research Java compile failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Class jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Research dex failed'}
foreach($kind in @('probe','carrier')){
 $manifest=if($kind -eq 'probe'){"$PSScriptRoot/AndroidManifest.api31.xml"}else{"$PSScriptRoot/AndroidManifest.carrier.xml"}
 & "$bt/aapt2.exe" link -I $jar --manifest $manifest -o "$out/$kind-unsigned.apk"
 if($LASTEXITCODE){throw 'Research manifest packaging failed'}
 if($kind -eq 'probe'){
  & "$JavaHome/bin/jar.exe" uf "$out/$kind-unsigned.apk" -C "$out/dex" classes.dex
  if($LASTEXITCODE){throw 'Dex addition failed'}
  & "$JavaHome/bin/jar.exe" u0f "$out/$kind-unsigned.apk" -C "$out/fixture" lib -C "$out/payload" assets
 }else{
  & "$JavaHome/bin/jar.exe" u0f "$out/$kind-unsigned.apk" -C "$out/carrier" lib
 }
 if($LASTEXITCODE){throw 'Native research packaging failed'}
 & "$bt/zipalign.exe" -P 16 -f 4 "$out/$kind-unsigned.apk" "$out/$kind-aligned.apk"
 if($LASTEXITCODE){throw 'Research alignment failed'}
 $version=$versions[$kind]
 $suffix=if($Unsigned){'unsigned-not-for-install'}else{'signed-research-only'}
 $apk="$out/$kind-$version-$suffix.apk"
 if($Unsigned){Copy-Item -LiteralPath "$out/$kind-aligned.apk" -Destination $apk}else{
  & "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/$kind-aligned.apk"
  if($LASTEXITCODE){throw 'Research signing failed'}
  $certificate=& "$bt/apksigner.bat" verify --print-certs $apk
  if($LASTEXITCODE -or !($certificate -match $ExpectedCertificateSha256)){throw 'Research signing identity mismatch'}
 }
 & "$bt/zipalign.exe" -c -P 16 4 $apk
 if($LASTEXITCODE){throw 'Final research APK alignment failed'}
 $listing=& "$bt/aapt.exe" list -v $apk
 if($LASTEXITCODE){throw 'Research APK listing failed'}
 if($kind -eq 'probe' -and (!($listing -match 'Stored.*assets/bridge/libcanary.so') -or ($listing -match 'lib/x86_64/|lib/arm64-v8a/libcanary.so'))){throw 'Probe must remain ARM64-only; canary only in APK assets'}
 if($kind -eq 'carrier' -and !($listing -match 'Stored.*lib/x86_64/libseedprobe_api31_host.so')){throw 'Carrier missing stored x64 host'}
 if(!$Unsigned){Copy-Item -LiteralPath $apk -Destination "$reserved/$kind-$version/$kind-$version-signed-research-only.apk"}
 Get-FileHash -Algorithm SHA256 $apk
}
Get-FileHash -Algorithm SHA256 $hostLibrary,"$out/api31-canary-root"
Write-Output "API31_LAB_BUILD=$out"
Write-Output 'Two development-only packages; product remains one APK. No deployment or device qualification implied.'
if($Unsigned){Write-Output 'NOT_FOR_INSTALL_OR_DELIVERY'}
