import unittest

from tcg_host_targets import (SCALAR_TARGETS, TARGETS, select_scalar_target,
                              select_target, validate_scalar_preload,
                              validate_scalar_argv as validate_target_argv)


class ScalarTargetTests(unittest.TestCase):
    marker = 'seedlens-api34-aosp-tcg-single-scalar-v1'

    def target(self):
        return select_scalar_target(self.marker)

    def args(self):
        t = self.target()
        return [b'qemu', b'-avd', t.avd, b'-port', t.port, b'-no-accel',
                b'-cores', b'2', b'-qemu', b'-accel', b'tcg,thread=single',
                b'-smp', b'2']

    def test_scalar_and_vector_markers_never_cross_authorize(self):
        for marker in SCALAR_TARGETS:
            with self.subTest(marker=marker), self.assertRaises(ValueError):
                select_target(marker)
        for marker in TARGETS:
            with self.subTest(marker=marker), self.assertRaises(ValueError):
                select_scalar_target(marker)
        for marker in [None, '', [], self.marker + ' ',
                       'seedlens-api34-tcg-single-scalar-v1']:
            with self.subTest(marker=marker), self.assertRaises(ValueError):
                select_scalar_target(marker)

    def test_exact_existing_aosp_mapper_and_two_guest_cpus(self):
        t = self.target()
        self.assertEqual(t.avd, b'SeedLens_API34_AOSP_ARM64_Linux')
        self.assertEqual(t.port, b'5590')
        self.assertEqual(t.required_guest_cores, 2)
        self.assertTrue(t.single_thread_tcg)
        self.assertFalse(t.multi_thread_tcg)
        vector = select_target('seedlens-api34-aosp-tcg-single-vector-v1')
        self.assertEqual(t.mapper, vector.mapper)
        self.assertEqual(t.mapper_sha256, vector.mapper_sha256)
        validate_target_argv(self.args(), t)
        validate_target_argv(self.args() + [b'-machine', b'virt'], t)

    def test_wrong_avd_port_and_legacy_target_refused(self):
        for option, value in [(b'-avd', b'SeedLens_API31_ARM64_Linux'),
                              (b'-avd', b'SeedLens_API34_ARM64_Linux'),
                              (b'-port', b'5586'), (b'-port', b'5588')]:
            args = self.args()
            args[args.index(option) + 1] = value
            with self.subTest(option=option, value=value), self.assertRaises(ValueError):
                validate_target_argv(args, self.target())
        legacy = select_scalar_target('seedlens-api31-tcg-scalar-host-v1')
        with self.assertRaises(ValueError):
            validate_target_argv(self.args(), legacy)

    def test_missing_ambiguous_or_wrong_resources_refused(self):
        for option in (b'-cores', b'-smp'):
            for value in (b'0', b'1', b'3', b'2,cores=1'):
                args = self.args()
                args[args.index(option) + 1] = value
                with self.subTest(option=option, value=value), self.assertRaises(ValueError):
                    validate_target_argv(args, self.target())
            args = self.args()
            at = args.index(option)
            del args[at:at+2]
            with self.assertRaises(ValueError):
                validate_target_argv(args, self.target())
            with self.assertRaises(ValueError):
                validate_target_argv(self.args() + [option, b'2'], self.target())
        args = self.args()
        args[6:8], args[11:13] = args[11:13], args[6:8]
        with self.assertRaises(ValueError):
            validate_target_argv(args, self.target())

    def test_other_modes_and_parser_scope_refused(self):
        for mode in (b'tcg', b'tcg,thread=multi', b'kvm', b'off',
                     b'tcg,thread=single,invalid=1'):
            args = self.args()
            args[args.index(b'-accel')+1] = mode
            with self.subTest(mode=mode), self.assertRaises(ValueError):
                validate_target_argv(args, self.target())
        for extra in ([b'-qemu'], [b'-enable-kvm'], [b'-no-accel'],
                      [b'-avd', self.target().avd], [b'-port', b'5590'],
                      [b'-accel', b'tcg,thread=single']):
            with self.subTest(extra=extra), self.assertRaises(ValueError):
                validate_target_argv(self.args() + extra, self.target())
        args = self.args()
        args[8:11] = [b'-accel', b'tcg,thread=single', b'-qemu']
        with self.assertRaises(ValueError):
            validate_target_argv(args, self.target())

    def test_only_single_pinned_mapper_preload_allowed(self):
        t = self.target()
        mapper = str(t.mapper)
        validate_scalar_preload(mapper, None, t)
        for preload in (None, '', mapper + ':', mapper + ':' + mapper,
                        mapper + ':/opt/seedlens-lab/host-mmu-save-v1/libseed-tcg-mmu-save.so',
                        str(select_scalar_target('seedlens-api31-tcg-scalar-host-v1').mapper)):
            with self.subTest(preload=preload), self.assertRaises(ValueError):
                validate_scalar_preload(preload, None, t)
        for marker in ('', 'seedlens-api34-aosp-tcg-single-vector-v1', 'wrong'):
            with self.subTest(marker=marker), self.assertRaises(ValueError):
                validate_scalar_preload(mapper, marker, t)

    def test_frontend_identity_after_backend_boundary_refused(self):
        for option, count in ((b'-avd', 2), (b'-port', 2), (b'-no-accel', 1)):
            args = self.args()
            at = args.index(option)
            moved = args[at:at+count]
            del args[at:at+count]
            args += moved
            with self.subTest(option=option), self.assertRaises(ValueError):
                validate_target_argv(args, self.target())

    def test_existing_api31_scalar_argv_retained(self):
        t = select_scalar_target('seedlens-api31-tcg-scalar-host-v1')
        args = [b'qemu', b'-avd', t.avd, b'-port', t.port, b'-no-accel',
                b'-qemu', b'-machine', b'virt', b'-smp', b'2']
        validate_target_argv(args, t)
        validate_scalar_preload(str(t.mapper), None, t)
        with self.assertRaises(ValueError):
            validate_target_argv(args, self.target())


if __name__ == '__main__':
    unittest.main()
