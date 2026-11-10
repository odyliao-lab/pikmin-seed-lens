"""Linux HOST research: append one fixed APEX concurrency property to a copy.

The official SDK ramdisk prefix and all effective entries except build.prop
remain unchanged. No API/security/timeout property or init executable is edited.
Never an Android APK dependency, stock-image or device-qualification claim.
"""
import argparse
import ctypes
import ctypes.util
import hashlib
import json
from pathlib import Path
import struct

SDK_SHA256 = '5c4a40a87ea671396a683a30fde67a3e51c63a21d332a9c5a18ecf20c53e6185'
AOSP_SDK_SHA256 = '91d2259c141607b5fda903afa4d0a6912389753f40e24015aed74db19fd6bd6c'
MAGIC = bytes.fromhex('02214c18')
NAME = 'system/etc/ramdisk/build.prop'
PROPERTY = b'apexd.config.boot_activation.threads=1\n'
MAX_BLOCK = 8 * 1024**2
MAX_ARCHIVE = 64 * 1024**2


def lz4_library():
    name = ctypes.util.find_library('lz4')
    if not name:
        raise RuntimeError('Linux host liblz4 required')
    lib = ctypes.CDLL(name)
    lib.LZ4_decompress_safe.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.LZ4_decompress_safe.restype = ctypes.c_int
    lib.LZ4_compressBound.argtypes = [ctypes.c_int]
    lib.LZ4_compressBound.restype = ctypes.c_int
    lib.LZ4_compress_default.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.LZ4_compress_default.restype = ctypes.c_int
    return lib


def decode_frames(data, lib):
    offset = 0
    result = bytearray()
    while offset < len(data):
        if data[offset:offset + 4] == MAGIC:
            offset += 4
        if offset + 4 > len(data):
            raise ValueError('truncated LZ4 block size')
        size = struct.unpack_from('<I', data, offset)[0]
        offset += 4
        if size == 0:
            continue
        if size > MAX_BLOCK + 65536 or offset + size > len(data):
            raise ValueError('invalid LZ4 block')
        source = ctypes.create_string_buffer(data[offset:offset + size])
        target = ctypes.create_string_buffer(MAX_BLOCK)
        count = lib.LZ4_decompress_safe(source, target, size, MAX_BLOCK)
        if count <= 0:
            raise ValueError('LZ4 decode failed')
        result.extend(target.raw[:count])
        if len(result) > MAX_ARCHIVE:
            raise ValueError('ramdisk too large')
        offset += size
    return bytes(result)


def cpio_entries(data):
    offset = 0
    entries = {}
    while offset < len(data):
        while offset < len(data) and data[offset] == 0:
            offset += 1
        if offset == len(data):
            break
        if offset + 110 > len(data) or data[offset:offset + 6] != b'070701':
            raise ValueError('unsupported cpio header')
        fields = [int(data[offset + 6 + 8*i:offset + 14 + 8*i], 16) for i in range(13)]
        offset += 110
        size, namesize = fields[6], fields[11]
        if not 1 <= namesize <= 4096 or offset + namesize > len(data):
            raise ValueError('invalid cpio name')
        raw_name = data[offset:offset + namesize]
        if raw_name[-1] != 0 or b'\0' in raw_name[:-1]:
            raise ValueError('invalid cpio name terminator')
        name = raw_name[:-1].decode('utf-8')
        if name.startswith('/') or '..' in name.split('/'):
            raise ValueError('unsafe cpio path')
        offset = (offset + namesize + 3) & ~3
        if size > MAX_ARCHIVE or offset + size > len(data):
            raise ValueError('invalid cpio content size')
        content = data[offset:offset + size]
        offset = (offset + size + 3) & ~3
        if name != 'TRAILER!!!':
            entries[name] = (fields, content)
    return entries


def encode_entry(name, fields, content):
    fields = list(fields)
    encoded_name = name.encode() + b'\0'
    fields[6], fields[11] = len(content), len(encoded_name)
    result = bytearray(b'070701' + ''.join(f'{value:08x}' for value in fields).encode())
    result.extend(encoded_name)
    result.extend(bytes((-len(result)) % 4))
    result.extend(content)
    result.extend(bytes((-len(result)) % 4))
    return bytes(result)


def build(source, output, *, aosp_research=False):
    source, output = source.resolve(strict=True), output.resolve()
    if output.is_relative_to(source.parent) or output.exists():
        raise ValueError('new output outside SDK image directory required')
    original = source.read_bytes()
    expected_sha = AOSP_SDK_SHA256 if aosp_research else SDK_SHA256
    if hashlib.sha256(original).hexdigest() != expected_sha or not original.startswith(MAGIC):
        raise ValueError('explicit official API34 ARM64 image-family pin mismatch')
    lib = lz4_library()
    before = cpio_entries(decode_frames(original, lib))
    fields, content = before[NAME]
    if fields[1] != 0o100644 or fields[2:4] != [0, 0] or fields[4] != 1:
        raise ValueError('unexpected property entry metadata')
    if b'ro.bootimage.build.version.sdk=34\n' not in content or PROPERTY.split(b'=')[0] in content:
        raise ValueError('unexpected source properties')
    changed = content + (b'' if content.endswith(b'\n') else b'\n') + PROPERTY
    trailer = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 11, 0]
    archive = encode_entry(NAME, fields, changed) + encode_entry('TRAILER!!!', trailer, b'')
    archive += bytes((-len(archive)) % 512)
    compressed = ctypes.create_string_buffer(lib.LZ4_compressBound(len(archive)))
    buffer = ctypes.create_string_buffer(archive)
    size = lib.LZ4_compress_default(buffer, compressed, len(archive), len(compressed))
    if size <= 0:
        raise ValueError('LZ4 encode failed')
    result = original + MAGIC + struct.pack('<I', size) + compressed.raw[:size]
    after = cpio_entries(decode_frames(result, lib))
    differences = [name for name in before if before[name] != after.get(name)]
    if set(after) != set(before) or differences != [NAME] or after[NAME][1] != changed:
        raise ValueError('unexpected effective ramdisk difference')
    # Only the file-size field differs; preserve original UID/GID/mode/time.
    metadata = list(after[NAME][0])
    metadata[6] = fields[6]
    if metadata != fields or result[:len(original)] != original:
        raise ValueError('original prefix or property metadata changed')
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open('xb') as stream:
        stream.write(result)
    record = {'sdkRamdiskSha256': expected_sha, 'privateRamdiskSha256': hashlib.sha256(result).hexdigest(),
              'aospHostResearch': aosp_research,
              'originalCompressedPrefixUnchanged': True, 'effectiveEntries': len(after),
              'onlyChangedEntry': NAME, 'onlyAddedProperty': PROPERTY.decode().strip(),
              'guestPropertyObserved': False, 'androidQualified': False, 'rootInstalled': False}
    with output.with_suffix('.receipt.json').open('x') as stream:
        json.dump(record, stream, indent=2)
    return record


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-ramdisk', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--aosp-research', action='store_true',
                        help='Select only the separately authorized official AOSP ARM64 rev4 pin')
    args = parser.parse_args()
    print(json.dumps(build(args.sdk_ramdisk, args.output, aosp_research=args.aosp_research)))
