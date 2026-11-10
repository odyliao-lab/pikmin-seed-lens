param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$PythonPath,
 [Parameter(Mandatory)][string]$SigningKey,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedCertificateSha256,
 [string]$StorePassEnv='SEED_LENS_LAB_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_LAB_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){
 throw 'Explicit external research key required'
}
foreach($name in @($StorePassEnv,$KeyPassEnv)){
 if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){
  throw 'Missing research signing environment'
 }
}
[xml]$manifest=Get-Content "$PSScriptRoot/AndroidManifest.xml" -Raw
$version=[string]$manifest.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$' -or $manifest.manifest.package -cne 'dev.ody.seedlensarmsignalprobe'){
 throw 'Unexpected research manifest'
}
$out="$root/.tools/research/arm-signal-probe-$version"
if(Test-Path -LiteralPath $out){throw 'Research version reserved; never overwrite'}
& $PythonPath -X utf8 -m unittest discover -s $PSScriptRoot -p test_records.py
if($LASTEXITCODE){throw 'Own-record parser/refusal checks failed'}
New-Item -ItemType Directory "$out/classes","$out/dex","$out/native/lib/arm64-v8a","$out/platform-src","$out/platform-tests"|Out-Null
# Compile the same pure ELF reader used by Root research in this separate package.
# No Root endpoint or runner is included in this APK.
foreach($name in @('ProcessElf.java','ProcessElfTest.java')){
 $source=Get-Content -LiteralPath "$root/research/root-probe/$name" -Raw
 if(([regex]::Matches($source,'(?m)^package dev\.ody\.seedlensrootprobe;\r?$')).Count -ne 1){throw 'Shared ELF reader package mismatch'}
 $source=$source.Replace('package dev.ody.seedlensrootprobe;','package dev.ody.seedlensarmsignalprobe;')
 [IO.File]::WriteAllText("$out/platform-src/$name",$source,[Text.UTF8Encoding]::new($false))
}
& "$JavaHome/bin/javac.exe" --release 8 -d "$out/platform-tests" "$out/platform-src/ProcessElf.java" "$out/platform-src/ProcessElfTest.java"
if($LASTEXITCODE){throw 'Own process ELF test compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/platform-tests" dev.ody.seedlensarmsignalprobe.ProcessElfTest
if($LASTEXITCODE){throw 'Own process ELF checks failed'}
& "$NdkPath/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe" --target=aarch64-linux-android28 -std=c++20 -O2 -Wall -Wextra -Werror -fPIC -shared -static-libstdc++ '-Wl,-z,max-page-size=16384' -fvisibility=hidden "$PSScriptRoot/signal_probe.cpp" "$PSScriptRoot/context.S" -landroid -o "$out/native/lib/arm64-v8a/libarmsignal.so"
if($LASTEXITCODE){throw 'ARM64 signal observation compile failed'}
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Signal research manifest failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" "$PSScriptRoot/SignalInstrumentation.java" "$out/platform-src/ProcessElf.java"
if($LASTEXITCODE){throw 'Signal research Java compile failed'}
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
$apk="$out/seedlens-arm-signal-probe-$version.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Research signing failed'}
$cert=& "$bt/apksigner.bat" verify --print-certs $apk
if($LASTEXITCODE -or !($cert -match ('^Signer #1 certificate SHA-256 digest: '+$ExpectedCertificateSha256.ToLowerInvariant()+'$'))){
 throw 'Research certificate mismatch'
}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Final research alignment failed'}
$listing=& "$bt/aapt.exe" list -v $apk
if($LASTEXITCODE -or !($listing -match 'Stored.*lib/arm64-v8a/libarmsignal.so') -or ($listing -match 'lib/x86_64/')){
 throw 'Signal probe must remain ARM64-only'
}
$badging=& "$bt/aapt2.exe" dump badging $apk
if($LASTEXITCODE -or !($badging -match "^package: name='dev.ody.seedlensarmsignalprobe' versionCode='$($manifest.manifest.versionCode)' versionName='$version'")){
 throw 'Final research version identity mismatch'
}
Get-FileHash -LiteralPath $apk,"$out/native/lib/arm64-v8a/libarmsignal.so"
Write-Output 'RESEARCH_ONLY: own fault observation/exit, no context modification, Root, game or platform qualification'
