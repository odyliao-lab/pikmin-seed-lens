"""Offline evidence only. Does not patch sources, authorize a profile or access devices.

Read exact full method signatures and map RVAs through executable ELF PT_LOAD
segments. A dump is untrusted, not proof of runtime compatibility. Retain the
input hashes and manually review field types/offsets and actual UI acceptance.
"""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

HOOKS = {
    'inventory': ('Niantic.Ichigo.Game.AnnotatedMap.Panels.PikminOrSeedPicker', 'PikminOrSeedPickerItem',
                  'public void UpdateData(Indexed<PikminOrSeed> update) { }'),
    'text': ('Niantic.Ichigo.UI', 'IchigoText', 'public override void set_text(string value) { }'),
    'canvas': ('UnityEngine.UI', 'CanvasUpdateRegistry', 'private void PerformUpdate() { }'),
    'expedition': ('Niantic.Ichigo.Game.Expedition.ListPanel', 'ExpeditionListItem',
                   'public void UpdateData(Indexed<IExpeditionData> indexedData) { }'),
    'detail': ('Niantic.Ichigo.Game.Expedition.DetailPanel', 'ExpeditionDetailPageV2',
               'private void DoUpdate(IExpeditionData newTask) { }'),
}
CLASSES = set((v[0], v[1]) for v in HOOKS.values()) | {
    ('Niantic.Ichigo.ModelControllers', 'PikminOrSeed'),
    ('Niantic.Ichigo.Inventory', 'PikminSeedInventoryItem'),
    ('Ichigo.Proto', 'IchigoInventoryItemProto'), ('Ichigo.Proto', 'PikminSeedProto'),
    ('Ichigo.Proto', 'PikminTypeProto'),
    ('Niantic.Ichigo.Game.Expedition.Data', 'ExpeditionItemData'),
    ('Niantic.Ichigo.ModelControllers.PikminVariations', 'PikminVariationCatalog'),
    ('Niantic.Ichigo.Game.Pikmins', 'PikminPictureBookTrackerV2'), ('UnityEngine.UI', 'Text'),
}
TYPE = re.compile(r'\b(?:class|struct|interface|enum)\s+(.+?)\s*(?::\s*.+?)?\s*// TypeDefIndex:')
RVA = re.compile(r'^\s*// RVA: 0x([0-9A-Fa-f]+)\b')
FIELD = re.compile(r'^\s*(.+?); // 0x([0-9A-Fa-f]+)\s*$')

def sha(path):
    with Path(path).open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()

def parse_dump(path):
    namespace = name = ''; pending = None; selected = {}; hooks = {k: [] for k in HOOKS}
    with Path(path).open(encoding='utf-8-sig', errors='strict') as f:
        for line_number, line in enumerate(f, 1):
            if line.startswith('// Namespace:'):
                namespace = line.partition(':')[2].strip(); name = ''; pending = None
            elif '// TypeDefIndex:' in line and not line.startswith((' ', '\t')):
                match = TYPE.search(line); name = match[1] if match else ''; pending = None
                if (namespace, name) in CLASSES:
                    key = namespace + '.' + name
                    if key in selected: raise ValueError('duplicate class: ' + key)
                    selected[key] = {'line': line_number, 'fields': []}
            match = RVA.match(line)
            if match:
                pending = int(match[1], 16); continue
            if (namespace, name) in CLASSES:
                field = FIELD.match(line)
                if field:
                    selected[namespace + '.' + name]['fields'].append({'declaration': field[1], 'offset': int(field[2], 16)})
                if pending is not None:
                    for key, (ns, cls, signature) in HOOKS.items():
                        if (namespace, name, line.strip()) == (ns, cls, signature):
                            hooks[key].append({'rva': pending, 'line': line_number, 'signature': signature})
            if line.strip() and not line.lstrip().startswith('['): pending = None
    if set(selected) != {ns + '.' + cls for ns, cls in CLASSES}: raise ValueError('required classes missing')
    if any(len(entries) != 1 for entries in hooks.values()): raise ValueError('missing/ambiguous exact hook signature')
    return {k: v[0] for k, v in hooks.items()}, selected

def prefix(path, rva, length=24):
    with Path(path).open('rb') as f:
        header = f.read(64)
        if len(header) != 64 or header[:7] != b'\x7fELF\x02\x01\x01': raise ValueError('requires ELF64 little endian')
        if struct.unpack_from('<H', header, 16)[0] != 3 or struct.unpack_from('<H', header, 18)[0] != 183:
            raise ValueError('requires AArch64 shared ELF')
        phoff = struct.unpack_from('<Q', header, 32)[0]
        phsize, phnum = struct.unpack_from('<HH', header, 54)
        size = Path(path).stat().st_size
        if phsize != 56 or not 1 <= phnum <= 1024 or phoff + phsize * phnum > size: raise ValueError('invalid program headers')
        matches = []
        for i in range(phnum):
            f.seek(phoff + i * phsize)
            kind, flags, offset, va, _, filesz, memsz, _ = struct.unpack('<IIQQQQQQ', f.read(56))
            if filesz > memsz or offset + filesz > size: raise ValueError('invalid segment bounds')
            if kind == 1 and flags & 1 and va <= rva and rva + length <= va + filesz:
                matches.append(offset + rva - va)
        if len(matches) != 1: raise ValueError('RVA not uniquely file-backed executable')
        f.seek(matches[0]); data = f.read(length)
        if len(data) != length: raise ValueError('short prefix')
        return matches[0], data

def evidence(dump, elf, metadata):
    hooks, fields = parse_dump(dump)
    for value in hooks.values():
        offset, raw = prefix(elf, value['rva'])
        value['file_offset'] = offset
        value['rva_hex'] = hex(value['rva'])
        value['prefix_words_le'] = [f'0x{x:08x}' for x in struct.unpack('<6I', raw)]
    return {'schema': 1, 'status': 'OFFLINE_CANDIDATE_NOT_DEVICE_ACCEPTED',
            'inputs': {k: {'size': Path(v).stat().st_size, 'sha256': sha(v)}
                       for k, v in [('dump', dump), ('elf', elf), ('metadata', metadata)]},
            'hooks': hooks, 'layouts': fields}

def main():
    p = argparse.ArgumentParser(description=__doc__)
    for key in ('dump', 'elf', 'metadata', 'output'): p.add_argument('--' + key, type=Path, required=True)
    a = p.parse_args()
    if a.output.resolve() in {v.resolve() for v in (a.dump, a.elf, a.metadata)}: raise ValueError('output overlaps input')
    result = evidence(a.dump, a.elf, a.metadata)
    with a.output.open('x', encoding='utf-8') as f: json.dump(result, f, indent=2, ensure_ascii=False)
    print(json.dumps({'status': result['status'], 'hooks': result['hooks'], 'layout_classes': len(result['layouts'])}))

if __name__ == '__main__': main()
