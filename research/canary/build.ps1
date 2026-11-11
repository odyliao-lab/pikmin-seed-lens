param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$SigningKey,
 [string]$StorePassEnv='SEED_LENS_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'External key required'}
foreach($name in @($StorePassEnv,$KeyPassEnv)){
 if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}
}
$out=Join-Path $root ('.tools/research/canary-'+(Get-Date -Format 'yyyyMMdd-HHmmss'))
if(Test-Path $out){throw 'Do not overwrite research builds'}
New-Item -ItemType Directory -Force "$out/classes","$out/dex","$out/native/lib/arm64-v8a","$out/payload/assets/bridge"|Out-Null
$clang="$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe"
foreach($name in @('fixture','canary')){
 $library=if($name -eq 'fixture'){"$out/native/lib/arm64-v8a/libfixture.so"}else{"$out/payload/assets/bridge/libcanary.so"}
 & $clang --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -fPIC -shared -static-libstdc++ '-Wl,-z,max-page-size=16384' -fvisibility=hidden "$PSScriptRoot/$name.cpp" -o $library
 if($LASTEXITCODE){throw "Native $name failed"}
 $dynamic=& "$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-readelf.exe" -d $library
 if($LASTEXITCODE -or ($dynamic -match 'libc\+\+_shared')){throw 'Unpackaged C++ runtime dependency'}
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
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/native" lib -C "$out/payload" assets
if($LASTEXITCODE){throw 'Packaging failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Alignment failed'}
$apk="$out/probe.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Signing failed'}
& "$bt/apksigner.bat" verify $apk
if($LASTEXITCODE){throw 'Verify failed'}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Signed APK alignment failed'}
$listing=& "$bt/aapt.exe" list -v $apk
if($LASTEXITCODE -or !($listing -match 'Stored.*assets/bridge/libcanary.so') -or ($listing -match 'lib/arm64-v8a/libcanary.so')){throw 'Canary must be stored only in APK assets'}
Get-FileHash -Algorithm SHA256 $apk
Write-Output "RESEARCH_APK=$apk"
