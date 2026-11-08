import pathlib
import struct
import tempfile
import unittest
import profile_evidence as p

class EvidenceTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory(); self.addCleanup(self.tmp.cleanup)
        self.root=pathlib.Path(self.tmp.name)
    def elf(self, **changes):
        b=bytearray(512);b[:7]=b'\x7fELF\x02\x01\x01'
        struct.pack_into('<HH', b, 16, 3, changes.get('machine',183))
        struct.pack_into('<Q',b,32,64);struct.pack_into('<HH',b,54,56,changes.get('count',1))
        seg=(1,changes.get('flags',5),256,0x1000,0,changes.get('filesz',128),256,4096)
        struct.pack_into('<IIQQQQQQ',b,64,*seg)
        if changes.get('count')==2:struct.pack_into('<IIQQQQQQ',b,120,*seg)
        b[256:280]=bytes(range(24));dest=self.root/'input.so';dest.write_bytes(b);return dest
    def test_translation_not_file_offset(self):
        self.assertEqual(p.prefix(self.elf(),0x1000),(256,bytes(range(24))))
    def test_nonexec(self):
        with self.assertRaises(ValueError):p.prefix(self.elf(flags=4),0x1000)
    def test_bss(self):
        with self.assertRaises(ValueError):p.prefix(self.elf(),0x1080)
    def test_wrong_arch(self):
        with self.assertRaises(ValueError):p.prefix(self.elf(machine=62),0x1000)
    def test_overflow_segment(self):
        with self.assertRaises(ValueError):p.prefix(self.elf(filesz=1000),0x1000)
    def test_ambiguous_mapping(self):
        with self.assertRaises(ValueError):p.prefix(self.elf(count=2),0x1000)
    def dump(self, duplicate=False, wrong_signature=False):
        lines=[]
        for ns,cls in sorted(p.CLASSES):
            lines += ['// Namespace: '+ns, 'public class '+cls+' // TypeDefIndex: 1', '{', '\tprivate int value; // 0x18']
            for _,(hns,hcls,sig) in p.HOOKS.items():
                if (ns,cls)==(hns,hcls):
                    if wrong_signature and cls=='ExpeditionDetailPageV2':sig=sig.replace('IExpeditionData','IExpeditionItemData')
                    lines += ['\t// RVA: 0x1000 Offset: 0x100 VA: 0x1000', '\t'+sig]
                    if duplicate:lines += ['\t// RVA: 0x1004 Offset: 0x104 VA: 0x1004','\t'+sig]
            lines += ['}']
        out=self.root/'dump.cs';out.write_text('\n'.join(lines),encoding='utf-8');return out
    def test_full_signature(self):
        hooks,fields=p.parse_dump(self.dump());self.assertEqual(len(hooks),5);self.assertEqual(len(fields),14)
    def test_ambiguous_method(self):
        with self.assertRaises(ValueError):p.parse_dump(self.dump(duplicate=True))
    def test_wrong_overload(self):
        with self.assertRaises(ValueError):p.parse_dump(self.dump(wrong_signature=True))

if __name__=='__main__':unittest.main()
