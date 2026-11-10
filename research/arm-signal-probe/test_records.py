"""Host parser checks only; generated records are not Android signal evidence."""
import unittest
from read_records import RECORD, inspect


def fixture(mode=2, terminal=3, **changes):
    fixed = dict(magic=0x534c5331, version=1, bytes=112, phase=1, mode=mode, api=34,
                 signal=0, code=0, pid=1234, uid=10123,
                 expected_pc=0 if mode in (0, 1, 6) else 0x1000,
                 saved_pc=0, fault_address=0, saved_sp=0, saved_pstate=0,
                 x19=0, x20=0, flags=0)
    rows = []
    for phase in (1, 2, terminal):
        r = dict(fixed, phase=phase)
        if phase == 3:
            r.update(signal=4, code=1, saved_pc=0x1000, fault_address=0x1000,
                     saved_sp=0x8000, x19=0x534c, x20=0x1357, flags=3)
            r.update(changes)
        rows.append(RECORD.pack(*r.values()))
    return b''.join(rows)


class RecordsTest(unittest.TestCase):
    def read(self, data, mode='udf', **identity):
        return inspect(data, **dict(api=34, mode=mode, pid=1234, uid=10123, **identity))

    def test_instruction_fault_is_observation_never_return_or_game_success(self):
        r = self.read(fixture())
        self.assertEqual(r['outcome'], 'own-fault-context-matched')
        self.assertFalse(r['instructionReturned'])
        self.assertFalse(r['gameOrPlatformQualified'])
        self.assertFalse(r['contextModified'])

    def test_return_only_proves_own_instruction_and_not_fault_context(self):
        r = self.read(fixture(mode=3, terminal=4), mode='sevl')
        self.assertEqual(r['outcome'], 'own-instruction-returned')
        self.assertFalse(r['ownFaultContextMatched'])
        self.assertFalse(r['gameOrPlatformQualified'])
        self.assertEqual(self.read(fixture(mode=2, terminal=4))['outcome'], 'unexpected-return')

    def test_synthetic_signal_never_proves_instruction_context(self):
        r = self.read(fixture(mode=1, code=-6), mode='raise')
        self.assertEqual(r['outcome'], 'synthetic-signal-observed')
        self.assertFalse(r['ownFaultContextMatched'])

    def test_fault_inside_raise_mode_does_not_prove_synthetic_delivery(self):
        self.assertEqual(self.read(fixture(mode=1, code=1), mode='raise')['outcome'],
                         'fault-observation-unproven')

    def test_baseline_return_does_not_prove_instruction_or_context(self):
        r=self.read(fixture(mode=0, terminal=4), mode='baseline')
        self.assertEqual(r['outcome'], 'baseline-returned')
        self.assertFalse(r['instructionReturned'])
        self.assertFalse(r['ownFaultContextMatched'])

    def test_wrong_pc_address_registers_stack_and_signal_code_are_not_proof(self):
        for changes in ({'saved_pc':0x1004}, {'fault_address':0x2000}, {'x19':0},
                        {'x20':0}, {'saved_sp':0}, {'saved_sp':0x8001}, {'code':128}):
            with self.subTest(changes=changes):
                self.assertFalse(self.read(fixture(**changes))['ownFaultContextMatched'])
        self.assertEqual(self.read(fixture(flags=1))['outcome'], 'fault-observation-unproven')

    def test_pair_can_identify_only_its_two_known_instruction_addresses(self):
        self.assertTrue(self.read(fixture(mode=4, saved_pc=0x1004, fault_address=0x1004),
                                  mode='pair')['ownFaultContextMatched'])
        self.assertFalse(self.read(fixture(mode=4, saved_pc=0x1008, fault_address=0x1008),
                                   mode='pair')['ownFaultContextMatched'])

    def test_truncation_missing_terminal_and_schema_corruption(self):
        for data in (b'', fixture()[:-1], b'\0'*112, fixture()+fixture()):
            with self.subTest(length=len(data)), self.assertRaises(ValueError): self.read(data)
        self.assertEqual(self.read(fixture()[:224])['outcome'], 'incomplete')

    def test_cross_invocation_records_cannot_supply_a_terminal(self):
        good=fixture(); wrong=bytearray(good); wrong[224+32:224+40]=(9999).to_bytes(8,'little')
        with self.assertRaises(ValueError): self.read(wrong)
        for field,value in (('api',31),('uid',10124),('pid',1235)):
            args=dict(api=34,mode='udf',uid=10123,pid=1234);args[field]=value
            with self.subTest(field=field), self.assertRaises(ValueError): inspect(good,**args)

    def test_reordering_and_unknown_flags_refuse(self):
        good=fixture()
        for data in (good[112:224]+good[:112]+good[224:],fixture(flags=7)):
            with self.assertRaises(ValueError): self.read(data)


    def test_wfe_is_independent_from_pair_and_has_one_fault_address(self):
        result = self.read(fixture(mode=5, terminal=4), mode='wfe')
        self.assertTrue(result['instructionReturned'])
        self.assertFalse(result['gameOrPlatformQualified'])
        self.assertTrue(self.read(fixture(mode=5), mode='wfe')['ownFaultContextMatched'])
        self.assertFalse(self.read(fixture(mode=5, saved_pc=0x1004,
                                          fault_address=0x1004),
                                   mode='wfe')['ownFaultContextMatched'])

    def test_armed_alarm_receipt_never_proves_timeout_or_a_native_return(self):
        data=fixture(mode=6)
        result=self.read(data[:224], mode='alarm')
        self.assertEqual(result['outcome'], 'timeout-control-armed-unconfirmed')
        self.assertFalse(result['instructionReturned'])
        self.assertFalse(result['gameOrPlatformQualified'])
        self.assertEqual(self.read(data[:112], mode='alarm')['outcome'], 'incomplete')
        self.assertEqual(self.read(fixture(mode=6, terminal=4), mode='alarm')['outcome'],
                         'unexpected-return')


    def test_own_resume_terminal_is_separate_from_hardware_siginfo_match(self):
        fault=fixture(mode=7, flags=7, code=-6, fault_address=0xff4)
        terminal=fixture(mode=7, terminal=4)[224:]
        result=self.read(fault+terminal, mode='resume-udf')
        self.assertEqual(result['outcome'], 'own-resume-native-roundtrip-observed')
        self.assertTrue(result['contextModified'])
        self.assertTrue(result['nativeCaseReturned'])
        self.assertFalse(result['instructionReturned'])
        self.assertFalse(result['ownFaultContextMatched'])
        self.assertFalse(result['hardwareSignalCodeAndAddressMatched'])
        self.assertFalse(result['gameOrPlatformQualified'])

    def test_resume_intent_alone_remains_unknown_and_guard_refusal_is_no_write(self):
        result=self.read(fixture(mode=7, flags=7), mode='resume-udf')
        self.assertEqual(result['outcome'], 'own-resume-attempt-unconfirmed')
        self.assertIsNone(result['contextModified'])
        self.assertFalse(result['nativeCaseReturned'])
        refused=self.read(fixture(mode=8, x19=0x534d), mode='resume-bad-marker')
        self.assertEqual(refused['outcome'], 'own-resume-guard-refused')
        self.assertFalse(refused['contextModified'])
        self.assertFalse(refused['contextWriteRequested'])

    def test_forged_resume_terminal_or_unsafe_write_intent_refuse(self):
        terminal=fixture(mode=7, terminal=4)[224:]
        for changes in ({'saved_pc':0x1004}, {'x19':0}, {'x20':0},
                        {'saved_sp':0x8001}, {'flags':4}, {'signal':11}, {'code':128}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                self.read(fixture(mode=7, **dict({'flags':7}, **changes))+terminal,
                          mode='resume-udf')
        with self.assertRaises(ValueError):
            self.read(fixture(mode=7)+terminal, mode='resume-udf')
        bad=fixture(mode=8)+fixture(mode=8, terminal=4)[224:]
        with self.assertRaises(ValueError): self.read(bad, mode='resume-bad-marker')
        with self.assertRaises(ValueError): self.read(fixture(flags=7))

    def test_resume_without_fault_or_replayed_fault_cannot_supply_roundtrip(self):
        result=self.read(fixture(mode=7, terminal=4), mode='resume-udf')
        self.assertEqual(result['outcome'], 'unexpected-return')
        fault=fixture(mode=7, flags=7)
        for data in (fault+fault[224:], fault+fixture(mode=7, terminal=4)[224:]*2):
            with self.assertRaises(ValueError): self.read(data, mode='resume-udf')


if __name__ == '__main__': unittest.main()
