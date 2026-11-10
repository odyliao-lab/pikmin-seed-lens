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


TARGETS = {
    'seedlens-api31-tcg-vector-save-v1': HostTarget(
        b'SeedLens_API31_ARM64_Linux', b'5586',
        Path('/opt/seedlens-lab/host-helper-v1/libseedlens-tcg-selection-v1.so'),
        'bc30eb3904692cd475cef5eda8d0127979b57b2451f451434542a39f28e1e795'),
    'seedlens-api34-tcg-vector-save-v1': HostTarget(
        b'SeedLens_API34_ARM64_Linux', b'5588',
        Path('/opt/seedlens-lab/host-helper-api34-v1/libseedlens-api34-tcg-selection-v1.so'),
        '1b702320855a24203f8fb6b081050965566617fae7630758998b6803e60125a7'),
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
    if argv.count(b'-no-accel') != 1 or b'-accel' in argv or b'-enable-kvm' in argv:
        raise ValueError('explicit software CPU required')
