"""Create a minimal, deterministic research-only newc ramdisk; never patch SDK images."""
import argparse
import gzip
import hashlib
from pathlib import Path
import stat


def entry(name, mode, data=b"", major=0, minor=0):
    label = name.encode("ascii") + b"\0"
    fields = [1, mode, 0, 0, 1, 0, len(data), 0, 0, major, minor, len(label), 0]
    record = b"070701" + b"".join(f"{value:08x}".encode("ascii") for value in fields) + label
    record += b"\0" * (-len(record) % 4)
    record += data + b"\0" * (-len(data) % 4)
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    binary = args.binary.read_bytes()
    if binary[:6] != b"\x7fELF\x02\x01" or int.from_bytes(binary[18:20], "little") != 183:
        raise SystemExit("AArch64 ELF64 required")
    if args.output.exists():
        raise SystemExit("Refusing to overwrite a prior research ramdisk")
    archive = entry("dev", stat.S_IFDIR | 0o755)
    archive += entry("dev/console", stat.S_IFCHR | 0o600, major=5, minor=1)
    archive += entry("init", stat.S_IFREG | 0o755, binary)
    archive += entry("TRAILER!!!", 0)
    archive += b"\0" * (-len(archive) % 512)
    payload = gzip.compress(archive, mtime=0)
    with args.output.open("xb") as output:
        output.write(payload)
    print(f"research_ramdisk bytes={len(payload)} sha256={hashlib.sha256(payload).hexdigest()}")


if __name__ == "__main__":
    main()
