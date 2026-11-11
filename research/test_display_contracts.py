"""Cross-check actual standalone consumers against the pinned original gateways."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

class DisplayContracts(unittest.TestCase):
    def test_eight_independently_recorded_gateway_contracts(self):
        evidence = json.loads((ROOT/'docs/DISPLAY_SOURCE_CONTRACTS.json').read_text())
        self.assertEqual(evidence['revision'], 'a02b58f35295948cbd29e467ce5ccd7ffe33161a')
        members = re.findall(r'\{"([^"]+)","([^"]+)","([^"]+)"\}', (ROOT/'native/renderer/display_groups.h').read_text())
        self.assertEqual(len(members), 8)
        java = (ROOT/'app/src/dev/ody/seedpreview/DisplayGroups.java').read_text()
        java_members = java.split('MEMBERS={',1)[1].split('};',1)[0]
        self.assertEqual(re.findall(r'"([^"]+)"',java_members), [m[0] for m in members])
        renderer = (ROOT/'native/renderer/extended_display.cpp').read_text()
        for (key,control,report), original in zip(members,evidence['members']):
            self.assertEqual(original['source_control'],'/data/local/tmp/pikmin-'+key+'.txt')
            self.assertEqual(control,'seed-lens-'+key+'.txt')
            self.assertEqual(report,'seed_lens_'+pathlib.PurePosixPath(original['source_report']).name)
            if key=='seed-preview':
                contract=(ROOT/'native/renderer/display_contract.h').read_text()
                self.assertIn(control,contract);self.assertIn(report,contract)
                self.assertIn('seed_lens::seed_control',renderer)
            else:
                self.assertIn('/data/local/tmp/'+control,renderer)
                self.assertIn('/files/'+report,renderer)
        self.assertEqual(members[-1][0],'nectar-stock-preview')

    def test_no_borrowed_control_or_rpc_implementation(self):
        files=list((ROOT/'native/renderer').glob('*_adapter.inc'))+[ROOT/'native/renderer/extended_display.cpp',ROOT/'native/renderer/standalone_context.inc']
        for p in files:
            source=p.read_text()
            self.assertNotIn('/data/local/tmp/pikmin-',source,p.name)
            self.assertNotIn('SendGenericRpc',source,p.name)
            self.assertNotIn('nectar_rpc',source,p.name)
        context=(ROOT/'native/renderer/standalone_context.inc').read_text()
        for guard in ['PlayerId','get_PlayerInfo','inventoryManager','original_logout','expected_account','epoch','revision']:
            self.assertIn(guard,context)

    def test_exact_standalone_build_binding(self):
        loader=(ROOT/'app/loader.cpp').read_text()
        renderer=(ROOT/'native/renderer/extended_display.cpp').read_text()
        self.assertIn('seed_root::fresh_display_status',loader)
        self.assertEqual(renderer.count('seed_lens::display_build'),8)
        self.assertIn('SEED_DISPLAY_BUILD_ID=$displayBuildId',(ROOT/'app/build.ps1').read_text())

if __name__=='__main__':unittest.main()
