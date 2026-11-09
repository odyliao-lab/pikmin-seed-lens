param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$SigningKey
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'External key required'}
foreach($name in @('SEED_LENS_STORE_PASS','SEED_LENS_KEY_PASS')){if(![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}}
[xml]$manifest=Get-Content "$PSScriptRoot/AndroidManifest.xml" -Raw
$version=[string]$manifest.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid research version'}
$out=Join-Path $root ".tools/research/arm64-jni-$version"
if(Test-Path -LiteralPath $out){throw 'Research version reserved; never overwrite'}
New-Item -ItemType Directory "$out/classes","$out/dex","$out/native/lib/arm64-v8a"|Out-Null
$clang="$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe"
foreach($name in @('fixture','canary')){
 & $clang --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -fPIC -shared -static-libstdc++ '-Wl,-z,max-page-size=16384' -fvisibility=hidden "$PSScriptRoot/$name.cpp" -o "$out/native/lib/arm64-v8a/lib$name.so"
 if($LASTEXITCODE){throw "Native $name failed"}
}
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Manifest failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" "$PSScriptRoot/ProbeInstrumentation.java"
if($LASTEXITCODE){throw 'Java failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Dex failed'}
& "$JavaHome/bin/jar.exe" uf "$out/unsigned.apk" -C "$out/dex" classes.dex
if($LASTEXITCODE){throw 'Dex addition failed'}
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/native" lib
if($LASTEXITCODE){throw 'Native packaging failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Alignment failed'}
$apk="$out/probe.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass env:SEED_LENS_STORE_PASS --key-pass env:SEED_LENS_KEY_PASS --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Signing failed'}
$cert=& "$bt/apksigner.bat" verify --print-certs $apk
if($LASTEXITCODE -or !($cert -match '9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e')){throw 'Certificate mismatch'}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Signed alignment failed'}
Get-FileHash $apk
Write-Output "ARM64_RESEARCH_APK=$apk"
