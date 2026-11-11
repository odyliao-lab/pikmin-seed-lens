[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$CmakePath,
 [Parameter(Mandatory)][string]$NinjaPath,
 [switch]$Unsigned,
 [switch]$Candidate,
 [switch]$Api31Candidate,
 [switch]$Api34Candidate,
 [switch]$Api31Qualification,
 [string]$SigningKey='',
 [string]$StorePassEnv='SEED_LENS_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_KEY_PASS'
)
$ErrorActionPreference='Stop'
if($Candidate -and !$Unsigned){throw 'Candidate builds are unsigned and must not be installed or delivered'}
if($Api31Candidate -and (!$Candidate -or !$Unsigned)){throw 'API31 integration requires -Candidate -Unsigned; no signed delivery before qualification'}
if($Api34Candidate -and (!$Candidate -or !$Unsigned)){throw 'API34 integration requires -Candidate -Unsigned; no signed API34 delivery'}
if($Api31Qualification -and ($Unsigned -or $Candidate -or $Api31Candidate -or $Api34Candidate)){throw 'API31 device qualification requires a new signed product version; do not mix unsigned candidate modes'}
$api31Enabled=[bool]($Api31Candidate -or $Api31Qualification)
$api34Enabled=[bool]$Api34Candidate
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
$displayBuildId="$version-code$($manifest.manifest.versionCode)"
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid standalone version'}
if($Api31Qualification -and ([long]$manifest.manifest.versionCode -le 12 -or [version]$version -le [version]'0.3.1')){throw 'API31 qualification must retain 0.3.1 and use a higher version/code'}
$kind=if($Unsigned){'unsigned'}else{'signed'}
$out=if($Candidate){"$root/.tools/candidates/$([Guid]::NewGuid().ToString('N'))"}else{"$root/.tools/build/$version/$kind"}
$apk="$out/pikmin-seed-lens-$version-$kind.apk"
if(Test-Path $apk){throw 'Delivered artifact immutable; bump manifest and output version for changes'}
New-Item -ItemType Directory -Force "$out/assets","$out/classes","$out/dex"|Out-Null
New-Item -ItemType Directory -Force "$out/tests"|Out-Null
# Generate only in this unique output directory; never mutate checked-in Java
# or reuse another variant's generated source. One input selects Java AND native.
$checkedInCompatibility=(Resolve-Path "$PSScriptRoot/src/dev/ody/seedpreview/Compatibility.java").Path
$compatibilityText=[IO.File]::ReadAllText($checkedInCompatibility)
$declaration='static final boolean API31_CANDIDATE=false;'
if([regex]::Matches($compatibilityText,[regex]::Escape($declaration)).Count -ne 1){throw 'Compatibility default declaration changed; refuse an ambiguous candidate'}
$javaFlag=if($api31Enabled){'true'}else{'false'}
$declaration34='static final boolean API34_CANDIDATE=false;'
if([regex]::Matches($compatibilityText,[regex]::Escape($declaration34)).Count -ne 1){throw 'Compatibility API34 default declaration changed; refuse an ambiguous candidate'}
$javaFlag34=if($api34Enabled){'true'}else{'false'}
New-Item -ItemType Directory "$out/generated"|Out-Null
$compatibilitySource="$out/generated/Compatibility.java"
[IO.File]::WriteAllText($compatibilitySource,$compatibilityText.Replace($declaration,"static final boolean API31_CANDIDATE=$javaFlag;").Replace($declaration34,"static final boolean API34_CANDIDATE=$javaFlag34;"),[Text.UTF8Encoding]::new($false))
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -d "$out/tests" $compatibilitySource "$PSScriptRoot/tests/CompatibilityTest.java" "$PSScriptRoot/src/dev/ody/seedpreview/InstalledNative.java" "$PSScriptRoot/tests/InstalledNativeTest.java"
if($LASTEXITCODE){throw 'Compatibility test compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.CompatibilityTest $javaFlag $javaFlag34
if($LASTEXITCODE){throw 'Compatibility tests failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.InstalledNativeTest
if($LASTEXITCODE){throw 'Installed native path/digest tests failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/GameSession.java" "$PSScriptRoot/tests/GameSessionTest.java"
if($LASTEXITCODE){throw 'Game session test compile failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/RootCommand.java" "$PSScriptRoot/tests/RootCommandTest.java"
if($LASTEXITCODE){throw 'Root command test compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.RootCommandTest
if($LASTEXITCODE){throw 'Root command selection tests failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.GameSessionTest
if($LASTEXITCODE){throw 'Game session tests failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -classpath "$out/tests" -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/LensContext.java" "$PSScriptRoot/src/dev/ody/seedpreview/LensStatus.java" "$PSScriptRoot/tests/LensStatusTest.java" "$PSScriptRoot/tests/LensContextTest.java"
if($LASTEXITCODE){throw 'Lens presentation protocol compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.LensStatusTest
if($LASTEXITCODE){throw 'Lens presentation protocol checks failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.LensContextTest
if($LASTEXITCODE){throw 'Lens boot/process binding checks failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -classpath "$out/tests" -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/DisplayGroups.java" "$PSScriptRoot/tests/DisplayGroupsTest.java"
if($LASTEXITCODE){throw 'Display group protocol compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.DisplayGroupsTest
if($LASTEXITCODE){throw 'Display group protocol checks failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -classpath "$out/tests" -d "$out/tests" "$PSScriptRoot/src/dev/ody/seedpreview/DisplayOverview.java" "$PSScriptRoot/tests/DisplayOverviewTest.java"
if($LASTEXITCODE){throw 'Display overview compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.DisplayOverviewTest
if($LASTEXITCODE){throw 'Display overview checks failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -d "$out/tests" "$PSScriptRoot/tests/ApkNativeLayout.java" "$PSScriptRoot/tests/ApkNativeLayoutTest.java"
if($LASTEXITCODE){throw 'APK layout checker compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.ApkNativeLayoutTest
if($LASTEXITCODE){throw 'APK layout checker tests failed'}
foreach($abi in @('arm64-v8a','x86_64')){
 $variant=if($api31Enabled -and $api34Enabled){'api31-api34-candidate'}elseif($api34Enabled){'api34-candidate'}elseif($api31Enabled){'api31-candidate'}else{''}
 $nativeBuild=if($variant){"$root/.tools/native-build/$variant/$abi"}else{"$root/.tools/native-build/$abi"}
 $api31Flag=if($api31Enabled){'ON'}else{'OFF'}
 $api34Flag=if($api34Enabled){'ON'}else{'OFF'}
 # Build and copy ARM first. The bridge hashes THIS immutable build output,
 # never an earlier installed renderer or a stale CMake cache entry.
 $rendererPin=if($abi -eq 'x86_64'){"$out/assets/libseed_display.so"}else{''}
 & $CmakePath -G Ninja "-DCMAKE_MAKE_PROGRAM=$NinjaPath" "-DCMAKE_TOOLCHAIN_FILE=$NdkPath/build/cmake/android.toolchain.cmake" "-DANDROID_ABI=$abi" "-DSEED_DISPLAY_BUILD_ID=$displayBuildId" "-DSEED_BRIDGE_RENDERER=$rendererPin" "-DSEED_API31_CANDIDATE=$api31Flag" "-DSEED_API34_CANDIDATE=$api34Flag" -DANDROID_PLATFORM=android-28 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release -S $PSScriptRoot -B $nativeBuild
 if($LASTEXITCODE){throw 'Native configure failed'}
 if(!(Get-Content "$nativeBuild/CMakeCache.txt" -Raw).Contains("SEED_DISPLAY_BUILD_ID:STRING=$displayBuildId")){throw 'Native display build identity mismatch'}
 if(!(Get-Content "$nativeBuild/CMakeCache.txt" -Raw).Contains("SEED_API31_CANDIDATE:BOOL=$api31Flag")){throw 'Native candidate setting mismatch'}
 if(!(Get-Content "$nativeBuild/CMakeCache.txt" -Raw).Contains("SEED_API34_CANDIDATE:BOOL=$api34Flag")){throw 'Native API34 candidate setting mismatch'}
 & $CmakePath --build $nativeBuild --parallel 4
 if($LASTEXITCODE){throw 'Native build failed'}
 New-Item -ItemType Directory -Force "$out/native/lib/$abi"|Out-Null
 Copy-Item -LiteralPath "$nativeBuild/seed-root" -Destination "$out/native/lib/$abi/libseed_loader.so"
 $digest=(Get-FileHash -Algorithm SHA256 "$nativeBuild/seed-root").Hash.ToLowerInvariant()
 [IO.File]::WriteAllText("$out/assets/seed-loader-$abi.sha256",$digest+"`n",[Text.Encoding]::ASCII)
 if($abi -eq 'arm64-v8a'){
  Copy-Item -LiteralPath "$nativeBuild/libseed_display.so" -Destination "$out/assets/libseed_display.so"
  Copy-Item -LiteralPath "$nativeBuild/libseed_display.so" -Destination "$out/native/lib/$abi/libseed_display.so"
 }else{
  Copy-Item -LiteralPath "$nativeBuild/libseed_bridge.so" -Destination "$out/native/lib/$abi/libseed_bridge.so"
  $hasApi31Adapter=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$out/native/lib/$abi/libseed_bridge.so")).Contains('API31_REQUIRED')
  if($hasApi31Adapter -ne $api31Enabled){throw 'Packaged bridge disagrees with Java candidate setting'}
  $hasApi34Adapter=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$out/native/lib/$abi/libseed_bridge.so")).Contains('API34_EXACT_RESEARCH_TUPLE_REQUIRED')
  if($hasApi34Adapter -ne $api34Enabled){throw 'Packaged bridge disagrees with Java API34 candidate setting'}
  Copy-Item -LiteralPath "$nativeBuild/seed-bridge-renderer.sha256" -Destination "$out/assets/seed-bridge-renderer.sha256"
  $bridgeDigest=(Get-FileHash -Algorithm SHA256 "$nativeBuild/libseed_bridge.so").Hash.ToLowerInvariant()
  [IO.File]::WriteAllText("$out/assets/seed-bridge-x86_64.sha256",$bridgeDigest+"`n",[Text.Encoding]::ASCII)
 }
}
Copy-Item -LiteralPath "$dep/LICENSE" -Destination "$out/assets/AndKittyInjector-LICENSE.txt"
Copy-Item -LiteralPath "$dep/KittyMemoryEx/LICENSE" -Destination "$out/assets/KittyMemoryEx-LICENSE.txt"
Copy-Item -LiteralPath "$root/third_party/RENDERER_NOTICES.md" -Destination "$out/assets/Renderer-THIRD-PARTY-NOTICES.txt"
# Windows aapt2 writes nested asset separators as literal backslashes. Keep
# this asset flat; the raw ZIP checker rejects backslash entries before signing.
Copy-Item -LiteralPath "$PSScriptRoot/assets/art/forest-lens-mascot-v1.png" -Destination "$out/assets/forest-lens-mascot-v1.png"
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
& "$bt/aapt2.exe" compile --dir "$PSScriptRoot/res" -o "$out/resources.zip"
if($LASTEXITCODE){throw 'Launcher resource compilation failed'}
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" "$out/resources.zip" -A "$out/assets" -0 so -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Resource link failed'}
$sources=@(Get-ChildItem "$PSScriptRoot/src" -Recurse -Filter *.java|Where-Object {$_.FullName -ine $checkedInCompatibility}|ForEach-Object FullName)+@($compatibilitySource)
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" $sources
if($LASTEXITCODE){throw 'Java compile failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Dex failed'}
& "$JavaHome/bin/jar.exe" uf "$out/unsigned.apk" -C "$out/dex" classes.dex
if($LASTEXITCODE){throw 'APK dex addition failed'}
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/native" lib
if($LASTEXITCODE){throw 'APK installed native library addition failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Alignment failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.ApkNativeLayout "$out/aligned.apk"
if($LASTEXITCODE){throw 'Native APK layout/digest failed before signing'}
if($Unsigned){
 Copy-Item -LiteralPath "$out/aligned.apk" -Destination $apk
}else{
 & "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
 if($LASTEXITCODE){throw 'Signing failed'}
 $cert=& "$bt/apksigner.bat" verify --print-certs $apk
 if($LASTEXITCODE -ne 0 -or !($cert -match '9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e')){throw 'Certificate mismatch'}
}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Final APK alignment failed'}
& "$JavaHome/bin/java.exe" -cp "$out/tests" dev.ody.seedpreview.ApkNativeLayout $apk
if($LASTEXITCODE){throw 'Native APK layout/digest validation failed; artifact must not be delivered'}
Get-FileHash $apk
if($Candidate){Write-Output 'NOT_FOR_INSTALL_OR_DELIVERY: candidate retains development manifest only'}
if($Api31Candidate){Write-Output 'API31_JAVA_AND_NATIVE_CANDIDATE: exact backend guards retained; product/game acceptance pending'}
if($Api34Candidate){Write-Output 'API34_JAVA_AND_NATIVE_CANDIDATE: unsigned only; original game startup and product acceptance remain unproven'}
if($Api31Qualification){Write-Output 'API31_SIGNED_ISOLATED_QUALIFICATION_ONLY: not a support claim or public release; nine-combination acceptance pending'}
Write-Output "Build completed ($kind). Not evidence of root load, UI restoration or multi-version compatibility."
