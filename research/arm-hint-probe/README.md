# ARM64 SEVL/WFE isolation probe

This separate laboratory package reproduces a game startup instruction boundary
without modifying, loading into, or calling the game. It requests no Root or
network permissions and never intercepts SIGILL. Product remains one APK.

Build with explicit SDK/JDK17/NDK27 paths, an external research keystore, expected
certificate SHA256 and password environment variable names. `build.ps1` reserves
each research version; changed bytes require a higher version/code. It verifies
the signature, stored ARM64-only native library and 16KB APK alignment.

On a freshly identified, authorized API31 or API35 laboratory guest:

```
adb -s <exact-authorized-serial> shell am instrument -w -e expected_api <31-or-35> -e mode baseline dev.ody.seedlensarmhintprobe/.HintInstrumentation
adb -s <exact-authorized-serial> shell am instrument -w -e expected_api <31-or-35> -e mode sevl dev.ody.seedlensarmhintprobe/.HintInstrumentation
adb -s <exact-authorized-serial> shell am instrument -w -e expected_api <31-or-35> -e mode pair dev.ody.seedlensarmhintprobe/.HintInstrumentation
```

Each command starts a distinct disposable App process. `baseline` confirms JNI
and the actual native API without executing either instruction. `sevl` executes
only SEVL; `pair` executes SEVL followed by WFE. Version 0.0.2 also executes the
same signal/alarm setup and cancellation in `baseline`, isolating the watchdog
from the instruction difference. Before the instruction, Java
durably records the phase in its private files. Native code sets a three-second
alarm with default disposition: a hang or illegal instruction ends only this
process. No outcome is promoted to game or platform qualification.

Instrumentation -1 plus the matching returned receipt proves only that mode
returned. `shortMsg=Process crashed` / instrumentation 0 is a failure, even when
ADB exits 0. Preserve the durable BEFORE marker, crash evidence and exact APK
SHA; do not substitute instructions or install another translator to call it a
pass. Actual results: [API31 startup boundary](../../docs/API31_GAME_STARTUP_BOUNDARY.md).
