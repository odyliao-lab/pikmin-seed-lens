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


if __name__=='__main__':
    unittest.main()
