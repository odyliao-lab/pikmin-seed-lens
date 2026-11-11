import copy
import http.client
import json
import os
from pathlib import Path
import sys
import tempfile
import threading
import time
import unittest
from http.server import HTTPServer
import receiver

FIXTURE = None
if '--fixture' in sys.argv:
    at = sys.argv.index('--fixture')
    FIXTURE = Path(sys.argv[at+1]).read_bytes()
    del sys.argv[at:at+2]


def sample():
    if FIXTURE is not None:
        return json.loads(FIXTURE)
    return {'schema':1, 'report_id':'11111111-1111-4111-8111-111111111111',
            'issue':'cannot_enable', 'facts':{k:'unknown' for k in receiver.FACTS}, 'events':[]}


def raw(value):
    return json.dumps(value).encode('utf-8')


class ReceiverTest(unittest.TestCase):
    def test_java_producer_contract(self):
        data = receiver.validate(FIXTURE if FIXTURE is not None else raw(sample()))
        self.assertEqual(set(data['facts']), receiver.FACTS)
        self.assertEqual(data['report_id'], '11111111-1111-4111-8111-111111111111')

    def test_no_private_or_unexpected_fields(self):
        for value in ['private@example.invalid', 'Bearer SECRET', '/data/user/0/player', '192.168.1.4']:
            data = sample(); data['facts']['root'] = value
            with self.subTest(value=value), self.assertRaises(ValueError):
                receiver.validate(raw(data))
        data = sample(); data['account'] = 'not permitted'
        with self.assertRaises(ValueError): receiver.validate(raw(data))
        data = sample(); data['facts']['token'] = 'not permitted'
        with self.assertRaises(ValueError): receiver.validate(raw(data))

    def test_invalid_ids_and_events(self):
        for value in ['../outside', '123', '11111111-1111-1111-1111-111111111111']:
            data = sample(); data['report_id'] = value
            with self.assertRaises(ValueError): receiver.validate(raw(data))
        for events in [['RAW_LOG'], ['ENABLE_ON']*17, [123], 'ENABLE_ON']:
            data = sample(); data['events'] = events
            with self.assertRaises(ValueError): receiver.validate(raw(data))

    def test_duplicate_keys_types_and_size(self):
        for body in [b'{"schema":1,"schema":1}', b'[]', b'null', b'{}', b'\xff', b' '*16385, b'['*1500+b'0'+b']'*1500]:
            with self.assertRaises(ValueError): receiver.validate(body)
        data = sample(); data['schema'] = True
        with self.assertRaises(ValueError): receiver.validate(raw(data))
        data = sample(); data['facts']['app_code'] = 41
        with self.assertRaises(ValueError): receiver.validate(raw(data))

    def test_private_storage_and_deduplication(self):
        with tempfile.TemporaryDirectory() as directory:
            store = receiver.Store(directory, per_minute=100)
            self.assertEqual(store.accept(raw(sample()), '127.0.0.1')[:2], (201,'accepted'))
            self.assertEqual(store.accept(raw(sample()), '127.0.0.1')[:2], (200,'accepted'))
            changed = sample(); changed['issue'] = 'other'
            self.assertEqual(store.accept(raw(changed), '127.0.0.1')[0], 409)
            files = list(Path(directory).glob('*.json')); self.assertEqual(len(files), 1)
            if os.name != 'nt': self.assertEqual(files[0].stat().st_mode & 0o777, 0o600)
            self.assertNotIn(b'127.0.0.1', files[0].read_bytes())
            self.assertNotIn('127.0.0.1', repr(store.rates))

    def test_rate_storage_limits_and_expiry(self):
        with tempfile.TemporaryDirectory() as directory:
            store = receiver.Store(directory, max_reports=1, per_minute=2)
            store.accept(raw(sample()), '127.0.0.1')
            second = sample(); second['report_id'] = '22222222-2222-4222-8222-222222222222'
            self.assertEqual(store.accept(raw(second), '127.0.0.1')[0], 503)
            self.assertEqual(store.accept(raw(second), '127.0.0.1')[0], 429)
            file = next(Path(directory).glob('*.json'))
            old = time.time()-receiver.RETENTION_SECONDS-1; os.utime(file, (old,old));store.purge()
            self.assertFalse(file.exists())

    def test_live_http_consent_and_no_read_api(self):
        with tempfile.TemporaryDirectory() as directory:
            store = receiver.Store(directory, per_minute=100)
            server = HTTPServer(('127.0.0.1',0), receiver.handler(store))
            thread = threading.Thread(target=server.serve_forever, daemon=True); thread.start()
            def request(method, path, body=None, headers=None):
                conn = http.client.HTTPConnection('127.0.0.1', server.server_port, timeout=3)
                try:
                    conn.request(method,path,body=body,headers=headers or {})
                    response = conn.getresponse(); return response.status, json.loads(response.read())
                finally: conn.close()
            try:
                body = raw(sample()); headers = {'Content-Type':'application/json','X-Seed-Lens-Consent':'preview-v1'}
                self.assertEqual(request('POST','/v1/reports',body,{'Content-Type':'application/json'})[0],400)
                self.assertEqual(list(Path(directory).glob('*.json')), [])
                self.assertEqual(request('GET','/v1/reports')[0],404)
                self.assertEqual(request('POST','/v1/reports?token=x',body,headers)[0],404)
                status, receipt = request('POST','/v1/reports',body,headers)
                self.assertEqual(status,201)
                self.assertEqual(receipt,{'status':'accepted','report_id':sample()['report_id']})
                self.assertEqual(request('POST','/v1/reports',body,headers)[0],200)
                self.assertEqual(request('GET','/'+sample()['report_id']+'.json')[0],404)
                self.assertEqual(request('POST','/v1/reports',b'',{**headers,'Content-Length':'16385'})[0],413)
                self.assertEqual(request('POST','/v1/reports',body,{**headers,'Content-Type':'text/plain'})[0],415)
            finally:
                server.shutdown();server.server_close();thread.join(timeout=3)

if __name__ == '__main__': unittest.main()
