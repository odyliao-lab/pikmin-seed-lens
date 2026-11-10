"""Decode own-App records. A recorded fault never means execution succeeded."""
import argparse
import json
import pathlib
import struct

RECORD = struct.Struct('<7Ii10Q')
FIELDS = ('magic', 'version', 'bytes', 'phase', 'mode', 'api', 'signal', 'code',
          'pid', 'uid', 'expected_pc', 'saved_pc', 'fault_address', 'saved_sp',
          'saved_pstate', 'x19', 'x20', 'flags')
MODES = {'baseline': 0, 'raise': 1, 'udf': 2, 'sevl': 3, 'pair': 4}


def inspect(data, *, api, mode, pid, uid):
    if api not in (31, 34, 35) or mode not in MODES or pid <= 0 or uid < 10000:
        raise ValueError('Exact own-App expected identity required')
    if not data or len(data) % RECORD.size:
        raise ValueError('Empty or truncated native record')
    records = [dict(zip(FIELDS, RECORD.unpack_from(data, offset)))
               for offset in range(0, len(data), RECORD.size)]
    if len(records) > 3:
        raise ValueError('Duplicate or extra terminal record')
    for r in records:
        if (r['magic'], r['version'], r['bytes']) != (0x534c5331, 1, 112):
            raise ValueError('Unknown record schema')
        if (r['api'], r['mode'], r['pid'], r['uid']) != (api, MODES[mode], pid, uid):
            raise ValueError('Record identity does not match this invocation')
        if r['flags'] & ~3:
            raise ValueError('Unknown observation flags')
        if mode in ('baseline', 'raise'):
            if r['expected_pc']:
                raise ValueError('Synthetic/baseline mode has an instruction address')
        elif not r['expected_pc'] or r['expected_pc'] % 4:
            raise ValueError('Own instruction address is missing or unaligned')
        if r['expected_pc'] != records[0]['expected_pc']:
            raise ValueError('Instruction address changed across phases')
        if r['phase'] != 3 and any(r[k] for k in
                ('signal', 'code', 'saved_pc', 'fault_address', 'saved_sp',
                 'saved_pstate', 'x19', 'x20', 'flags')):
            raise ValueError('Non-fault phase carries fault state')
    phases = [r['phase'] for r in records]
    result = {'phases': phases, 'records': records,
              'gameOrPlatformQualified': False, 'contextModified': False}
    if phases in ([1], [1, 2]):
        return dict(result, outcome='incomplete', instructionReturned=False,
                    ownFaultContextMatched=False)
    if phases == [1, 2, 4]:
        outcome = ('baseline-returned' if mode == 'baseline' else
                   'unexpected-return' if mode in ('raise', 'udf') else
                   'own-instruction-returned')
        return dict(result, outcome=outcome, instructionReturned=mode in ('sevl', 'pair'),
                    ownFaultContextMatched=False)
    if phases != [1, 2, 3]:
        raise ValueError('Native phases are out of order or duplicated')
    r = records[-1]
    if r['signal'] != 4 or r['flags'] != 3:
        return dict(result, outcome='fault-observation-unproven', instructionReturned=False,
                    ownFaultContextMatched=False)
    if mode == 'raise':
        synthetic = r['code'] in (-6, 0)
        return dict(result, outcome=('synthetic-signal-observed' if synthetic else
                                     'fault-observation-unproven'), instructionReturned=False,
                    ownFaultContextMatched=False)
    expected_pcs = {r['expected_pc']}
    if mode == 'pair':
        expected_pcs.add(r['expected_pc'] + 4)
    checks = {
        'instructionMode': mode in ('udf', 'sevl', 'pair'),
        'instructionSignalCode': r['code'] in (1, 2),
        'ownFaultPc': r['saved_pc'] in expected_pcs,
        'signalAddressMatchesPc': r['fault_address'] == r['saved_pc'],
        'calleeSavedMarkers': r['x19'] == 0x534c and r['x20'] == 0x1357,
        'stackAligned': r['saved_sp'] != 0 and r['saved_sp'] % 16 == 0,
    }
    matched = all(checks.values())
    return dict(result, outcome=('own-fault-context-matched' if matched else
                                'fault-observed-context-unproven'),
                checks=checks, instructionReturned=False, ownFaultContextMatched=matched)


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('path', type=pathlib.Path)
    p.add_argument('--api', type=int, required=True)
    p.add_argument('--mode', choices=MODES, required=True)
    p.add_argument('--pid', type=int, required=True)
    p.add_argument('--uid', type=int, required=True)
    args = p.parse_args()
    decoded = inspect(args.path.read_bytes(), api=args.api, mode=args.mode,
                      pid=args.pid, uid=args.uid)
    print(json.dumps(decoded, indent=2))
    raise SystemExit(2 if decoded['outcome'] in ('incomplete', 'unexpected-return',
                     'fault-observation-unproven', 'fault-observed-context-unproven') else 0)
