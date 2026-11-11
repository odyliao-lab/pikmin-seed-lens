# Isolated clipboard capability research

Development-only ARM64 probe; never bundled as an extra installation prerequisite. JNI helper is the same header linked into the standalone payload. No game hook, game data, provider/location writes, network permission or real coordinates are used by self-test.

0.0.1 was built but action-bar overlap prevented clicking its button. 0.0.2 fixed layout and confirmed API35 main-thread copy/readback. 0.0.3 adds a detached native worker and confirms API32 OFF=0, INVALID=2, COPY_READBACK=1, WORKER=3. 0.0.4 repeats all four results on API35 and adds a read-only fingerprint button for independent verification after the product has copied; this button neither loads the native probe nor writes the clipboard. Previously generated APKs remain under ignored .tools/research/.

Only install on freshly verified, authorized isolated MuMu targets. Record actual API/ABI; success is not a game-feature qualification. Uninstall before testing product copy, and only reinstall as a passive verification receiver afterward if needed. Never publish clipboard contents, screenshots or report bodies. Private evidence is outside Git.

Build using build.ps1 with explicit SDK/JDK17/NDK r27d and external keystore. Passwords come from SEED_LENS_STORE_PASS / SEED_LENS_KEY_PASS. This does not install a product dependency or change Root/SELinux.
