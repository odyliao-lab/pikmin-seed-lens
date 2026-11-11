param(
 [Parameter(Mandatory)][string]$SdkPath,
 [Parameter(Mandatory)][string]$JavaHome,
 [Parameter(Mandatory)][string]$NdkPath,
 [Parameter(Mandatory)][string]$CmakePath,
 [Parameter(Mandatory)][string]$NinjaPath,
 [Parameter(Mandatory)][string]$ProbeApk,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedProbeSha256,
 [Parameter(Mandatory)][string]$SigningKey,
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-fA-F]{64}$')][string]$ExpectedCertificateSha256,
 [string]$StorePassEnv='SEED_LENS_LAB_STORE_PASS',
 [string]$KeyPassEnv='SEED_LENS_LAB_KEY_PASS'
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$key=(Resolve-Path -LiteralPath $SigningKey).Path
if($key.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'External lab key required'}
foreach($name in @($StorePassEnv,$KeyPassEnv)){
 if($name -notmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or ![Environment]::GetEnvironmentVariable($name)){throw 'Missing signing environment'}
}
if((Get-FileHash -LiteralPath $ProbeApk).Hash -ine $ExpectedProbeSha256){throw 'Existing immutable probe SHA mismatch'}
$bt="$SdkPath/build-tools/35.0.0";$env:JAVA_HOME=$JavaHome
$certificate=& "$bt/apksigner.bat" verify --print-certs $ProbeApk
if($LASTEXITCODE -or !($certificate -match $ExpectedCertificateSha256)){throw 'Probe lab certificate mismatch'}
[xml]$probe=Get-Content "$PSScriptRoot/AndroidManifest.api31.xml" -Raw
$badging=& "$bt/aapt.exe" dump badging $ProbeApk
if($LASTEXITCODE -or !($badging -match "^package: name='dev.ody.seedlensprobe' versionCode='$($probe.manifest.versionCode)' versionName='$($probe.manifest.versionName)'")){throw 'Exact signed probe version mismatch'}
[xml]$carrier=Get-Content "$PSScriptRoot/AndroidManifest.carrier-api32.xml" -Raw
$version=[string]$carrier.manifest.versionName
if($version -notmatch '^0\.[0-9]+\.[0-9]+$'){throw 'Invalid carrier version'}
# Same reservation namespace as API31 builds: never overwrite a carrier version.
$reserved="$root/.tools/research/api31-lab-signed/carrier-$version"
if(Test-Path $reserved){throw 'Signed carrier version reserved; bump manifest name/code'}
New-Item -ItemType Directory $reserved | Out-Null
$out=Join-Path $root ('.tools/research/api32-lab-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory "$out/payload","$out/carrier/lib/x86_64" | Out-Null
$zip=[IO.Compression.ZipFile]::OpenRead((Resolve-Path $ProbeApk))
try{
 $entries=@($zip.Entries | Where-Object FullName -ceq 'assets/bridge/libcanary.so')
 if($entries.Count -ne 1){throw 'Canary entry ambiguous'}
 [IO.Compression.ZipFileExtensions]::ExtractToFile($entries[0],"$out/payload/libcanary.so")
}finally{$zip.Dispose()}
$build="$out/x64-build"
$common=@('-G','Ninja',"-DCMAKE_MAKE_PROGRAM=$NinjaPath","-DCMAKE_TOOLCHAIN_FILE=$NdkPath/build/cmake/android.toolchain.cmake",'-DANDROID_ABI=x86_64','-DANDROID_PLATFORM=android-28','-DANDROID_STL=c++_static','-DCMAKE_BUILD_TYPE=Release',"-DSEED_CANARY_LIBRARY=$out/payload/libcanary.so",'-S',$PSScriptRoot,'-B',$build)
& $CmakePath @common
if($LASTEXITCODE){throw 'API32 research configure failed'}
& $CmakePath --build $build --parallel 4 --target seedprobe_api32_lab_host
if($LASTEXITCODE){throw 'API32 host build failed'}
$hostLibrary="$out/carrier/lib/x86_64/libseedprobe_api32_lab_host.so"
Copy-Item -LiteralPath "$build/libseedprobe_api32_lab_host.so" -Destination $hostLibrary
& $CmakePath @common "-DSEED_API32_LAB_HOST_LIBRARY=$hostLibrary"
if($LASTEXITCODE){throw 'API32 PM verifier configure failed'}
& $CmakePath --build $build --parallel 4 --target api32-canary-root api32-app-root
if($LASTEXITCODE){throw 'API32 PM verifier build failed'}
Copy-Item -LiteralPath "$build/api32-canary-root" -Destination "$out/api32-canary-root"
Copy-Item -LiteralPath "$build/api32-app-root" -Destination "$out/api32-app-root"
& "$bt/aapt2.exe" link -I "$SdkPath/platforms/android-35/android.jar" --manifest "$PSScriptRoot/AndroidManifest.carrier-api32.xml" -o "$out/unsigned.apk"
if($LASTEXITCODE){throw 'Carrier manifest failed'}
& "$JavaHome/bin/jar.exe" u0f "$out/unsigned.apk" -C "$out/carrier" lib
if($LASTEXITCODE){throw 'Carrier native packaging failed'}
& "$bt/zipalign.exe" -P 16 -f 4 "$out/unsigned.apk" "$out/aligned.apk"
if($LASTEXITCODE){throw 'Carrier alignment failed'}
$apk="$out/carrier-$version-signed-research-only.apk"
& "$bt/apksigner.bat" sign --ks $key --ks-pass "env:$StorePassEnv" --key-pass "env:$KeyPassEnv" --out $apk "$out/aligned.apk"
if($LASTEXITCODE){throw 'Carrier signing failed'}
$certificate=& "$bt/apksigner.bat" verify --print-certs $apk
if($LASTEXITCODE -or !($certificate -match $ExpectedCertificateSha256)){throw 'Carrier certificate mismatch'}
& "$bt/zipalign.exe" -c -P 16 4 $apk
if($LASTEXITCODE){throw 'Final carrier alignment failed'}
$listing=& "$bt/aapt.exe" list -v $apk
if($LASTEXITCODE -or !($listing -match 'Stored.*lib/x86_64/libseedprobe_api32_lab_host.so')){throw 'Carrier missing stored host'}
Copy-Item -LiteralPath $apk -Destination "$reserved/carrier-$version-signed-research-only.apk"
Get-FileHash $apk,$hostLibrary,"$out/api32-canary-root","$out/api32-app-root"
Write-Output "API32_LAB_BUILD=$out"
