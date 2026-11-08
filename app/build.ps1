[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$CmakePath,
 [Parameter(Mandatory)][string]$NinjaPath,
 [switch]$Unsigned,
 [string]$SigningKey='',
 [string]$StorePassEnv='SEED_LENS_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$dep="$root/third_party/AndKittyInjector"
if(!$Unsigned){
 if(!$SigningKey -or !(Test-Path -LiteralPath $SigningKey -PathType Leaf)){throw 'Explicit external signing key required; do not store keys in this repository'}
 $key=(Resolve-Path -LiteralPath $SigningKey).Path
 if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Signing key must be outside the repository'}
 foreach($secretName in @($StorePassEnv,$KeyPassEnv)){
  if($secretName -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($secretName)){throw 'Signing password environment variable missing/invalid'}
 }
}
$pins=@(@($dep,'9ece8bc0f05344ab23e882a38728ab91f70bfed0'),@("$dep/KittyMemoryEx",'be4cdc9a79e3e1b93357e8d30608e84a3518f5d3'))
foreach($pin in $pins){
 $head=& git -C $pin[0] rev-parse HEAD
 if($LASTEXITCODE -ne 0 -or $head -cne $pin[1]){throw 'Dependency commit mismatch; see README, do not use latest implicitly'}
 $dirty=& git -C $pin[0] status --porcelain
 if($LASTEXITCODE -ne 0 -or $dirty){throw 'Dependency source must be clean'}
}
$manifest=[xml](Get-Content "$PSScriptRoot/AndroidManifest.xml" -Raw)
$version=[string]$manifest.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid standalone version'}
$kind=if($Unsigned){'unsigned'}else{'signed'}
$out="$root/.tools/build/$version/$kind"
$apk="$out/pikmin-seed-lens-$version-$kind.apk"
if(Test-Path $apk){throw 'Delivered artifact immutable; bump manifest and output version for changes'}
New-Item -ItemType Directory -Force "$out/assets","$out/classes","$out/dex"|Out-Null
New-Item -ItemType Directory -Force "$out/tests"|Out-Null
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/Compatibility.java" "$PSScriptRoot/tests/CompatibilityTest.java"
if($LASTEXITCODE){throw 'Compatibility test compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.CompatibilityTest
if($LASTEXITCODE){throw 'Compatibility tests failed'}
foreach($abi in @('arm64-v8a','x86_64')){
 $nativeBuild="$root/.tools/native-build/$abi"
 & $CmakePath -G Ninja "-DCMAKE_MAKE_PROGRAM=$NinjaPath" "-DCMAKE_TOOLCHAIN_FILE=$NdkPath/build/cmake/android.toolchain.cmake" "-DANDROID_ABI=$abi" -DANDROID_PLATFORM=android-28 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release -S $PSScriptRoot -B $nativeBuild
 if($LASTEXITCODE){throw 'Native configure failed'}
 & $CmakePath --build $nativeBuild --parallel 4
 if($LASTEXITCODE){throw 'Native build failed'}
 Copy-Item -LiteralPath "$nativeBuild/seed-root" -Destination "$out/assets/seed-root-$abi"
}
Copy-Item -LiteralPath "$root/.tools/native-build/arm64-v8a/libseed_display.so" -Destination "$out/assets/libseed_display.so"
Copy-Item -LiteralPath "$dep/LICENSE" -Destination "$out/assets/AndKittyInjector-LICENSE.txt"
Copy-Item -LiteralPath "$dep/KittyMemoryEx/LICENSE" -Destination "$out/assets/KittyMemoryEx-LICENSE.txt"
Copy-Item -LiteralPath "$root/third_party/RENDERER_NOTICES.md" -Destination "$out/assets/Renderer-THIRD-PARTY-NOTICES.txt"
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" -A "$out/assets" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Resource link failed'}
$sources=@(Get-ChildItem "$PSScriptRoot/src" -Recurse -Filter *.java|ForEach-Object FullName)
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" $sources
if($LASTEXITCODE){throw 'Java compile failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Dex failed'}
& "$JavaHome/bin/jar.exe" uf "$out/unsigned.apk" -C "$out/dex" classes.dex
if($LASTEXITCODE){throw 'APK dex addition failed'}
& "$bt/zipalign.exe" -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Alignment failed'}
if($Unsigned){
 Copy-Item -LiteralPath "$out/aligned.apk" -Destination $apk
}else{
 & "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
 if($LASTEXITCODE){throw 'Signing failed'}
 $cert=& "$bt/apksigner.bat" verify --print-certs $apk
 if($LASTEXITCODE -ne 0 -or !($cert -match '9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e')){throw 'Certificate mismatch'}
}
Get-FileHash $apk
Write-Output "Build completed ($kind). Not evidence of root load, UI restoration or multi-version compatibility."
