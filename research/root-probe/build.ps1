param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$SigningKey,
 [Parameter(Mandatory)][string]$NativeRunner,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedRunnerSha256,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedCertificateSha256,
 [ValidateSet('x86_64','arm64-v8a')][string]$Abi='x86_64',
 [string]$StorePassEnv='SEED_LENS_LAB_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_LAB_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$runner=(Resolve-Path -LiteralPath $NativeRunner).Path
if((Get-FileHash -LiteralPath $runner).Hash -ine $ExpectedRunnerSha256){throw 'Explicit native runner digest mismatch'}
$elf=[IO.File]::ReadAllBytes($runner)
if($elf.Length -lt 4096 -or $elf.Length -gt 33554432 -or
   [BitConverter]::ToString($elf,0,7) -cne '7F-45-4C-46-02-01-01' -or
   [BitConverter]::ToString($elf,16,4) -cne $(if($Abi -eq 'arm64-v8a'){'03-00-B7-00'}else{'03-00-3E-00'})){throw 'Research PM runner ELF64 PIE must match explicit ABI'}
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Explicit external research key required'}
foreach($name in @($StorePassEnv,$KeyPassEnv)){
 if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}
}
[xml]$manifest=Get-Content "$PSScriptRoot/AndroidManifest.xml" -Raw
$version=[string]$manifest.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid research version'}
$out="$root/.tools/research/root-probe-$version"
if(Test-Path $out){throw 'Research version reserved; preserve artifacts and bump version/code'}
New-Item -ItemType Directory "$out/classes","$out/dex","$out/native/lib/$Abi","$out/assets" | Out-Null
Copy-Item -LiteralPath $runner -Destination "$out/native/lib/$Abi/libseed_probe_runner.so"
[IO.File]::WriteAllText("$out/assets/runner.sha256",$ExpectedRunnerSha256.ToLowerInvariant()+"`n",[Text.Encoding]::ASCII)
[IO.File]::WriteAllText("$out/assets/runner.abi",$Abi+"`n",[Text.Encoding]::ASCII)
$bt="$SdkPath/build-tools/35.0.0";$jar="$SdkPath/platforms/android-35/android.jar";$env:JAVA_HOME=$JavaHome
New-Item -ItemType Directory "$out/platform-tests" | Out-Null
& "$JavaHome/bin/javac.exe" --release 8 -d "$out/platform-tests" "$PSScriptRoot/ProcessElf.java" "$PSScriptRoot/ProcessElfTest.java" "$PSScriptRoot/ProbePlatform.java" "$PSScriptRoot/ProbePlatformTest.java" "$PSScriptRoot/RootOnlyPlatform.java" "$PSScriptRoot/RootOnlyPlatformTest.java" "$PSScriptRoot/Api34PmResearch.java" "$PSScriptRoot/Api34PmResearchTest.java"
if($LASTEXITCODE){throw 'Research platform test compile failed'}
& "$JavaHome/bin/java.exe" -cp "$out/platform-tests" dev.ody.seedlensrootprobe.ProcessElfTest
if($LASTEXITCODE){throw 'Research process ELF checks failed'}
& "$JavaHome/bin/java.exe" -cp "$out/platform-tests" dev.ody.seedlensrootprobe.ProbePlatformTest
if($LASTEXITCODE){throw 'Research platform admission/refusal checks failed'}
& "$JavaHome/bin/java.exe" -cp "$out/platform-tests" dev.ody.seedlensrootprobe.RootOnlyPlatformTest
if($LASTEXITCODE){throw 'Self-only Root scope/refusal checks failed'}
& "$JavaHome/bin/java.exe" -cp "$out/platform-tests" dev.ody.seedlensrootprobe.Api34PmResearchTest
if($LASTEXITCODE){throw 'Explicit API34 PM scope/refusal checks failed'}
& "$bt/aapt2.exe" link -I $jar --manifest "$PSScriptRoot/AndroidManifest.xml" -A "$out/assets" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Root probe manifest failed'}
& "$JavaHome/bin/javac.exe" -encoding UTF-8 -source 8 -target 8 -classpath $jar -d "$out/classes" "$PSScriptRoot/RootInstrumentation.java" "$PSScriptRoot/RootEndpoint.java" "$PSScriptRoot/PMCanary.java" "$PSScriptRoot/ProbePlatform.java" "$PSScriptRoot/ProcessElf.java" "$PSScriptRoot/RootOnlyPlatform.java" "$PSScriptRoot/Api34PmResearch.java"
if($LASTEXITCODE){throw 'Root probe Java compile failed'}
& "$JavaHome/bin/jar.exe" cf "$out/classes.jar" -C "$out/classes" .
if($LASTEXITCODE){throw 'Class jar failed'}
& "$bt/d8.bat" --lib $jar --min-api 28 --output "$out/dex" "$out/classes.jar"
if($LASTEXITCODE){throw 'Root probe dex failed'}
& "$JavaHome/bin/jar.exe" uf "$out/unsigned.apk" -C "$out/dex" classes.dex
if($LASTEXITCODE){throw 'Dex packaging failed'}
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/native" lib
if($LASTEXITCODE){throw 'PM runner packaging failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Alignment failed'}
$apk="$out/seedlens-root-probe-$version.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Research signing failed'}
$certificate=& "$bt/apksigner.bat" verify --print-certs $apk
if($LASTEXITCODE -or !($certificate -match $ExpectedCertificateSha256)){throw 'Research signing identity mismatch'}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Final alignment failed'}
Get-FileHash $apk
Write-Output "ROOT_PROBE_APK=$apk; research only, not a product dependency"
