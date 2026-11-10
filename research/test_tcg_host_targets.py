import unittest
from tcg_host_targets import select_target, validate_target_argv


class HostTargetTests(unittest.TestCase):
    def args(self, target):
        return [b'qemu', b'-avd', target.avd, b'-port', target.port, b'-no-accel']

    def test_distinct_fixed_targets(self):
        old=select_target('seedlens-api31-tcg-vector-save-v1')
        new=select_target('seedlens-api34-tcg-vector-save-v1')
        self.assertNotEqual(old.avd,new.avd)
        self.assertNotEqual(old.port,new.port)
        self.assertNotEqual(old.mapper,new.mapper)
        self.assertNotEqual(old.mapper_sha256,new.mapper_sha256)
        validate_target_argv(self.args(old),old)
        validate_target_argv(self.args(new),new)

    def test_unknown_marker_refused(self):
        for marker in [None,'', 'seedlens-api35-tcg-vector-save-v1', 'seedlens-api34-tcg-vector-save-v1 ']:
            with self.subTest(marker=marker),self.assertRaises(ValueError):
                select_target(marker)

    def test_cross_target_refused(self):
        old=select_target('seedlens-api31-tcg-vector-save-v1')
        new=select_target('seedlens-api34-tcg-vector-save-v1')
        for args,target in [(self.args(old),new),(self.args(new),old)]:
            with self.subTest(target=target.avd),self.assertRaises(ValueError):
                validate_target_argv(args,target)

    def test_ambiguous_or_incomplete_target_refused(self):
        target=select_target('seedlens-api34-tcg-vector-save-v1')
        valid=self.args(target)
        for args in [valid+[b'-avd',target.avd],valid+[b'-port',target.port],
                     valid[:3],valid+[b'-port'],valid[0:3]+[b'-port']]:
            with self.subTest(args=args),self.assertRaises(ValueError):
                validate_target_argv(args,target)

    def test_software_conflicts_refused(self):
        target=select_target('seedlens-api34-tcg-vector-save-v1')
        valid=self.args(target)
        for args in [valid[:-1],valid+[b'-no-accel'],valid+[b'-accel',b'off'],
                     valid+[b'-enable-kvm']]:
            with self.subTest(args=args),self.assertRaises(ValueError):
                validate_target_argv(args,target)

    def test_single_thread_has_distinct_explicit_scope(self):
        old=select_target('seedlens-api34-tcg-vector-save-v1')
        new=select_target('seedlens-api34-tcg-single-vector-v1')
        self.assertEqual(old.avd,new.avd)
        self.assertEqual(old.port,new.port)
        self.assertNotEqual(old.mapper,new.mapper)
        self.assertNotEqual(old.mapper_sha256,new.mapper_sha256)
        valid=self.args(new)+[b'-qemu',b'-accel',b'tcg,thread=single']
        validate_target_argv(valid,new)
        for argv,target in [(valid,old),(self.args(new),new),
                            (self.args(new)+[b'-accel',b'tcg,thread=single',b'-qemu'],new)]:
            with self.subTest(argv=argv),self.assertRaises(ValueError):
                validate_target_argv(argv,target)

    def test_single_thread_refuses_ambiguous_or_other_modes(self):
        target=select_target('seedlens-api34-tcg-single-vector-v1')
        prefix=self.args(target)+[b'-qemu',b'-accel']
        invalid=[prefix,prefix+[b'off'],prefix+[b'kvm'],prefix+[b'tcg'],
                 prefix+[b'tcg,thread=multi'],prefix+[b'tcg,thread=single,invalid=1'],
                 prefix+[b'tcg,thread=single',b'-qemu'],
                 prefix+[b'tcg,thread=single',b'-accel',b'tcg,thread=single'],
                 prefix+[b'tcg,thread=single',b'-enable-kvm']]
        for argv in invalid:
            with self.subTest(argv=argv),self.assertRaises(ValueError):
                validate_target_argv(argv,target)

    def test_single_thread_still_refuses_other_avd_or_port(self):
        target=select_target('seedlens-api34-tcg-single-vector-v1')
        for option,value in [(b'-avd',b'SeedLens_API31_ARM64_Linux'),(b'-port',b'5586')]:
            argv=self.args(target)+[b'-qemu',b'-accel',b'tcg,thread=single']
            argv[argv.index(option)+1]=value
            with self.subTest(option=option),self.assertRaises(ValueError):
                validate_target_argv(argv,target)

    def test_aosp_is_separate_target_and_artifact(self):
        google=select_target('seedlens-api34-tcg-single-vector-v1')
        aosp=select_target('seedlens-api34-aosp-tcg-single-vector-v1')
        self.assertEqual(aosp.avd,b'SeedLens_API34_AOSP_ARM64_Linux')
        self.assertEqual(aosp.port,b'5590')
        self.assertNotEqual(aosp.mapper,google.mapper)
        self.assertNotEqual(aosp.mapper_sha256,google.mapper_sha256)
        valid=self.args(aosp)+[b'-qemu',b'-accel',b'tcg,thread=single']
        validate_target_argv(valid,aosp)
        for argv,target in [(valid,google),(self.args(google)+[b'-qemu',b'-accel',b'tcg,thread=single'],aosp)]:
            with self.subTest(argv=argv),self.assertRaises(ValueError):
                validate_target_argv(argv,target)

    def test_aosp_rejects_cross_ports_and_modes(self):
        aosp=select_target('seedlens-api34-aosp-tcg-single-vector-v1')
        valid=self.args(aosp)+[b'-qemu',b'-accel',b'tcg,thread=single']
        for position,value in [(valid.index(b'-port')+1,b'5588'),(valid.index(b'-avd')+1,b'SeedLens_API34_ARM64_Linux'),(len(valid)-1,b'kvm')]:
            argv=list(valid);argv[position]=value
            with self.subTest(argv=argv),self.assertRaises(ValueError):
                validate_target_argv(argv,aosp)


if __name__=='__main__':
    unittest.main()
