"""Fixed Linux research targets. A matching tuple is not user authorization.

This module is never part of an Android APK or its admission policy.
"""
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class HostTarget:
    avd: bytes
    port: bytes
    mapper: Path
    mapper_sha256: str
    single_thread_tcg: bool = False
    required_guest_cores: int | None = None
    multi_thread_tcg: bool = False


TARGETS = {
    'seedlens-api31-tcg-vector-save-v1': HostTarget(
        b'SeedLens_API31_ARM64_Linux', b'5586',
        Path('/opt/seedlens-lab/host-helper-v1/libseedlens-tcg-selection-v1.so'),
        'bc30eb3904692cd475cef5eda8d0127979b57b2451f451434542a39f28e1e795'),
    'seedlens-api34-tcg-vector-save-v1': HostTarget(
        b'SeedLens_API34_ARM64_Linux', b'5588',
        Path('/opt/seedlens-lab/host-helper-api34-v1/libseedlens-api34-tcg-selection-v1.so'),
        '1b702320855a24203f8fb6b081050965566617fae7630758998b6803e60125a7'),
    'seedlens-api34-tcg-single-vector-v1': HostTarget(
        b'SeedLens_API34_ARM64_Linux', b'5588',
        Path('/opt/seedlens-lab/host-helper-api34-single-v1/libseedlens-api34-tcg-single-selection-v1.so'),
        '0aa0e6e0f23048592ac113e14fdfb0f7c6673268e971af46e5ac1839b8f1122e',
        True),
    'seedlens-api34-aosp-tcg-single-vector-v1': HostTarget(
        b'SeedLens_API34_AOSP_ARM64_Linux', b'5590',
        Path('/opt/seedlens-lab/host-helper-api34-aosp-single-v1/libseedlens-api34-aosp-tcg-single-selection-v1.so'),
        'f4b738fdafbffdb3c8f0aec7c0e16e5da6234a33e9fd19ba978452f937ceab88',
        True),
    'seedlens-api34-aosp-tcg-onecore-vector-v1': HostTarget(
        b'SeedLens_API34_AOSP_ARM64_Linux', b'5590',
        Path('/opt/seedlens-lab/host-helper-api34-aosp-onecore-v1/libseedlens-api34-aosp-tcg-onecore-selection-v1.so'),
        '59b63525581cbde87ce018bf65483e010441379a1f126b434bc4e50038f1643f',
        True, 1),
    'seedlens-api34-aosp-tcg-multi-vector-v1': HostTarget(
        b'SeedLens_API34_AOSP_ARM64_Linux', b'5590',
        Path('/opt/seedlens-lab/host-helper-api34-aosp-multi-v1/libseedlens-api34-aosp-tcg-multi-selection-v1.so'),
        'd329efe1dd2ea9ca086105240431136bc90362ef0569f20366e94bdf825b692a',
        required_guest_cores=2, multi_thread_tcg=True),
}


def select_target(marker):
    try:
        return TARGETS[marker]
    except (KeyError, TypeError):
        raise ValueError('explicit fixed-target marker required') from None


def validate_target_argv(argv, target):
    for option, expected in ((b'-avd', target.avd), (b'-port', target.port)):
        if argv.count(option) != 1:
            raise ValueError('ambiguous target option')
        at = argv.index(option)
        if at + 1 >= len(argv) or argv[at + 1] != expected:
            raise ValueError('wrong target')
    if argv.count(b'-no-accel') != 1 or b'-enable-kvm' in argv:
        raise ValueError('explicit software CPU required')
    if target.single_thread_tcg or target.multi_thread_tcg:
        if target.single_thread_tcg and target.multi_thread_tcg:
            raise ValueError('conflicting TCG scheduling scopes')
        if argv.count(b'-qemu') != 1 or argv.count(b'-accel') != 1:
            raise ValueError('one explicit backend scheduling option required')
        at=argv.index(b'-accel')
        mode=b'tcg,thread=multi' if target.multi_thread_tcg else b'tcg,thread=single'
        if at != argv.index(b'-qemu')+1 or at+1 >= len(argv) or argv[at+1] != mode:
            raise ValueError('exact backend-only TCG scheduling required')
    elif b'-accel' in argv:
        raise ValueError('existing targets refuse accelerator overrides')
    if target.required_guest_cores is not None:
        for option in (b'-cores',b'-smp'):
            if argv.count(option)!=1:
                raise ValueError('one explicit frontend/backend CPU count required')
            at=argv.index(option)
            if at+1>=len(argv) or argv[at+1]!=str(target.required_guest_cores).encode():
                raise ValueError('wrong guest CPU count')
            if (option==b'-cores') != (at<argv.index(b'-qemu')):
                raise ValueError('CPU count in wrong parser scope')
