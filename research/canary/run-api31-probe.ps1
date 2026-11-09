[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$AdbPath,
 [Parameter(Mandatory)][ValidatePattern('^(emulator-[0-9]+|127\.0\.0\.1:[0-9]+)$')][string]$Serial,
 [string]$ExpectedAvdName='',
 [ValidateSet(31,35)][int]$ExpectedApi=31,
 [string]$MumuCliPath='',
 [Parameter(Mandatory)][ValidatePattern('^[0-9a-f-]{36}$')][string]$ExpectedBoot,
 [Parameter(Mandatory)][string]$Runner,
 [Parameter(Mandatory)][ValidatePattern('^/data/local/tmp/[A-Za-z0-9_/-]+$')][string]$DeviceRunner,
 [Parameter(Mandatory)][string]$ProbeApk,
 [Parameter(Mandatory)][string]$CarrierApk,
 [switch]$TimeoutTest
)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$out=Join-Path $root (".tools/lab/api$ExpectedApi-run-"+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $out | Out-Null
function Read-Guest([string]$Command){
 $value=& $AdbPath -s $Serial shell $Command 2>&1
 if($LASTEXITCODE){throw "Read-only guest query failed: $Command"}
 return ($value -join "`n").Trim()
}
function Fresh-Identity {
 if($ExpectedApi -eq 31){
  if($Serial -notmatch '^emulator-[0-9]+$' -or $ExpectedAvdName -cne 'SeedLens_API31_x64'){throw 'Authorized API31 AVD required'}
  $name=& $AdbPath -s $Serial emu avd name
  if($LASTEXITCODE -or $name[0].Trim() -cne $ExpectedAvdName){throw 'Exact AVD identity mismatch'}
 }else{
  if(!$MumuCliPath){throw 'Explicit MuMu CLI required for index5 identity'}
  $info=& $MumuCliPath info --vmindex 5 | ConvertFrom-Json
  if($LASTEXITCODE -or $info.error_code -ne 0 -or $info.index -ne '5' -or
     $info.name -cne 'SeedLens-Isolated-A15' -or !$info.is_android_started -or
     $Serial -cne "$($info.adb_host_ip):$($info.adb_port)"){throw 'Exact authorized index5 identity mismatch'}
 }
 $enforcement=if($ExpectedApi -eq 31){'Enforcing'}else{'Permissive'}
 $bridge=if($ExpectedApi -eq 31){'libndk_translation.so'}else{'libnb.so'}
 if((Read-Guest 'cat /proc/sys/kernel/random/boot_id') -cne $ExpectedBoot -or
    (Read-Guest 'getprop ro.build.version.sdk') -ne "$ExpectedApi" -or (Read-Guest 'id -u') -ne '0' -or
    (Read-Guest 'getprop ro.dalvik.vm.native.bridge') -cne $bridge -or
    (Read-Guest 'getenforce') -cne $enforcement){throw 'Exact API/bridge/boot/root/SELinux mismatch'}
}
function Package-Path([string]$Package){
 $path=Read-Guest "pm path $Package"
 if($path -notmatch '^package:(/data/app/[A-Za-z0-9_+=~/.-]+/base\.apk)$'){throw 'PM source path mismatch'}
 return $Matches[1]
}
Fresh-Identity
$localRunnerSha=(Get-FileHash -LiteralPath $Runner).Hash.ToLowerInvariant()
if(((Read-Guest "sha256sum $DeviceRunner") -split '\s+')[0] -cne $localRunnerSha){throw 'Installed runner SHA mismatch'}
foreach($item in @(@('dev.ody.seedlensprobe',$ProbeApk),@('dev.ody.seedlenshostcarrier',$CarrierApk))){
 $path=Package-Path $item[0]
 $sha=(Get-FileHash -LiteralPath $item[1]).Hash.ToLowerInvariant()
 if(((Read-Guest "sha256sum $path") -split '\s+')[0] -cne $sha){throw 'Installed research APK SHA mismatch'}
}
$carrierBase=Package-Path 'dev.ody.seedlenshostcarrier'
$hostName=if($ExpectedApi -eq 31){'libseedprobe_api31_host.so'}else{'libseedprobe_api35_lab_host.so'}
$hostPath=$carrierBase.Substring(0,$carrierBase.Length-8)+"lib/x86_64/$hostName"
$existing=& $AdbPath -s $Serial shell pidof dev.ody.seedlensprobe 2>$null
if($existing){throw 'Existing probe process; never inherit or retry its attempt'}
$instrumentation=Start-Process -FilePath $AdbPath -ArgumentList @('-s',$Serial,'shell','am','instrument','-w','dev.ody.seedlensprobe/.ProbeInstrumentation') -WindowStyle Hidden -RedirectStandardOutput "$out/instrumentation-out.txt" -RedirectStandardError "$out/instrumentation-err.txt" -PassThru
@{avd=$ExpectedAvdName;serial=$Serial;boot=$ExpectedBoot;instrumentationPid=$instrumentation.Id;timeoutTest=[bool]$TimeoutTest} | ConvertTo-Json | Set-Content "$out/session.json"
Write-Output "LIVE_RUN=$out HOST_INSTRUMENTATION_PID=$($instrumentation.Id)"
$deadline=[DateTime]::UtcNow.AddSeconds(25)
$probePid='';$baseline=''
do {
 $candidate=& $AdbPath -s $Serial shell pidof dev.ody.seedlensprobe 2>$null
 if($candidate -and $candidate.Trim() -match '^[0-9]+$'){
  $probePid=$candidate.Trim()
  $value=& $AdbPath -s $Serial shell "cat /data/user/0/dev.ody.seedlensprobe/files/baseline-$probePid-*.tsv" 2>$null
  if(!$LASTEXITCODE -and ($value -join "`n") -match 'FIXTURE_PASS\s+4'){$baseline=$value -join "`n";break}
 }
 if($instrumentation.HasExited){throw 'Instrumentation exited before baseline readiness'}
 Start-Sleep -Milliseconds 100
} while([DateTime]::UtcNow -lt $deadline)
if(!$baseline){throw 'Baseline readiness deadline; no load and no automatic retry'}
$baseline | Set-Content "$out/baseline-before.txt"
$procStat=Read-Guest "cat /proc/$probePid/stat"
$tail=$procStat.Substring($procStat.LastIndexOf(')')+2)
$start=($tail -split '\s+')[19]
Fresh-Identity
@{pid=$probePid;start=$start;host=$hostPath;boot=$ExpectedBoot;runnerSha256=$localRunnerSha} | ConvertTo-Json | Set-Content "$out/attempt.json"
Write-Output "FIXTURE_READY PID=$probePid START=$start; one load only"
$wrongPath=& $AdbPath -s $Serial shell "$DeviceRunner $probePid $start $ExpectedApi /data/local/tmp/forbidden.so" 2>&1
if($LASTEXITCODE -ne 6){throw 'PM-path negative failed; no load'}
$wrongStart=([uint64]$start+1).ToString()
$stale=& $AdbPath -s $Serial shell "$DeviceRunner $probePid $wrongStart $ExpectedApi $hostPath" 2>&1
if($LASTEXITCODE -ne 5){throw 'Stale-identity negative failed; no load'}
$option=if($TimeoutTest){' --timeout-test'}else{''}
$load=& $AdbPath -s $Serial shell "$DeviceRunner $probePid $start $ExpectedApi $hostPath$option" 2>&1
$loadExit=$LASTEXITCODE
@("EXIT=$loadExit")+$load | Set-Content "$out/load.txt"
Write-Output "LOADER_EXIT=$loadExit; no retry"
$load
$journal=Read-Guest "cat /data/local/tmp/seed-lens-probe-pm-v1/attempt-$ExpectedBoot-$probePid-$start"
$journal | Set-Content "$out/journal.txt"
$finishDeadline=[DateTime]::UtcNow.AddSeconds(105)
while(!$instrumentation.HasExited -and [DateTime]::UtcNow -lt $finishDeadline){Start-Sleep -Milliseconds 250;$instrumentation.Refresh()}
$baseline=Read-Guest "cat /data/user/0/dev.ody.seedlensprobe/files/baseline-$probePid-*.tsv"
$baseline | Set-Content "$out/baseline-after.txt"
$namespace=& $AdbPath -s $Serial shell "cat /data/user/0/dev.ody.seedlensprobe/files/namespace-$probePid.tsv" 2>$null
$namespaceText=$namespace -join "`n"
$namespaceText | Set-Content "$out/namespace.txt"
Fresh-Identity
$stillAlive=& $AdbPath -s $Serial shell "test -e /proc/$probePid" 2>$null
$processGone=$LASTEXITCODE -eq 1
$ok=$false
if($TimeoutTest){
 $ok=$loadExit -eq 42 -and $journal.Contains("v2`t3`tCALLING") -and $journal.Contains("END`tUNKNOWN") -and
     !$journal.Contains('RETURNED_UNVERIFIED') -and $processGone -and $instrumentation.HasExited
 $status=if($ok){'CONTROLLED_UNKNOWN_CONTAINED_SELF_PROBE_ONLY'}else{'TIMEOUT_CASE_NOT_PROVEN_NO_RETRY'}
}else{
 $required=@('WRONG_PACKAGE_REJECTED','EXACT_VERSION_AND_NEGATIVE_CONTROLS_PASS','APK_PAYLOAD_SHA_VERIFIED',
  'MISSING_LIBRARY_REJECTED','MISSING_SYMBOL_REJECTED',
  'CANARY_PASS_4','BAD_KEY_REJECTED','JNI_ONLOAD_THREAD_SINGLETON_PASS')
 $ok=$loadExit -eq 0 -and $journal.Contains("v2`t6`tRETURNED_UNVERIFIED") -and !$journal.Contains("END`tUNKNOWN") -and
     $baseline.Contains("FINISH`tPASS") -and $baseline.Contains('CALL_THREAD_FINISHED') -and $instrumentation.HasExited -and $processGone
 if($instrumentation.HasExited){
  $instrumentationOutput=Get-Content "$out/instrumentation-out.txt" -Raw
  $ok=$ok -and $instrumentation.ExitCode -eq 0 -and $instrumentationOutput.Contains('INSTRUMENTATION_CODE: -1')
 }
 foreach($stage in $required){if(!$namespaceText.Contains($stage)){$ok=$false}}
 $namespaceStage=if($ExpectedApi -eq 31){'API31_BRIDGED_NAMESPACE_CONFIRMED_BY_LOAD'}else{'REAL_BRIDGED_NAMESPACE'}
 if(!$namespaceText.Contains($namespaceStage)){$ok=$false}
 $status=if($ok){"API${ExpectedApi}_SELF_APP_CANARY_PASS_NOT_GAME_ACCEPTANCE"}else{'CANARY_NOT_PROVEN_NO_RETRY'}
}
@{status=$status;loaderExit=$loadExit;pid=$probePid;start=$start;boot=$ExpectedBoot;processGone=$processGone;instrumentationExited=$instrumentation.HasExited} | ConvertTo-Json | Set-Content "$out/result.json"
Get-Content "$out/result.json"
Write-Output "PRIVATE_RECEIPTS=$out"
if(!$ok){exit 1}
