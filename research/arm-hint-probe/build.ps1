param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$SigningKey,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedCertificateSha256,
 [string]$StorePassEnv='SEED_LENS_LAB_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_LAB_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'External research key required'}
foreach($name in @($StorePassEnv,$KeyPassEnv)){
 if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}
}
[xml]$manifest=Get-Content "$PSScriptRoot/AndroidManifest.xml" -Raw
$version=[string]$manifest.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid research version'}
$out="$root/.tools/research/arm-hint-probe-$version"
if(Test-Path $out){throw 'Research version reserved; never overwrite'}
New-Item -ItemType Directory "$out/classes","$out/dex","$out/native/lib/arm64-v8a"|Out-Null
& "$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe" --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -fPIC -shared -static-libstdc++ '-Wl,-z,max-page-size=16384' -fvisibility=hidden "$PSScriptRoot/hint.cpp" -landroid -o "$out/native/lib/arm64-v8a/libarmhint.so"
if($LASTEXITCODE){throw 'ARM64 instruction library compile failed'}
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Research manifest failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" "$PSScriptRoot/HintInstrumentation.java"
if($LASTEXITCODE){throw 'Research Java compile failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Research class jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Research dex failed'}
& "$JavaHome/bin/jar.exe" uf "$out/unsigned.apk" -C "$out/dex" classes.dex
if($LASTEXITCODE){throw 'Research dex packaging failed'}
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/native" lib
if($LASTEXITCODE){throw 'Research native packaging failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Research alignment failed'}
$apk="$out/seedlens-arm-hint-probe-$version.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Research signing failed'}
$cert=& "$bt/apksigner.bat" verify --print-certs $apk
if($LASTEXITCODE -or !($cert -match $ExpectedCertificateSha256)){throw 'Research certificate mismatch'}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Final alignment failed'}
$listing=& "$bt/aapt.exe" list -v $apk
if($LASTEXITCODE -or !($listing -match 'Stored.*lib/arm64-v8a/libarmhint.so') -or ($listing -match 'lib/x86_64/')){throw 'Probe must remain ARM64-only'}
Get-FileHash $apk,"$out/native/lib/arm64-v8a/libarmhint.so"
Write-Output 'RESEARCH_ONLY: no Root, game operations, instruction substitution or platform qualification claim'
