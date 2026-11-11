"""Real loopback HTTP checks; synthetic reports only, no production credentials."""
import http.client
import json
import os
from pathlib import Path
import tempfile
import threading
import time
import unittest
from http.server import HTTPServer
from receiver import FACTS, Store, handler, RETENTION_SECONDS
from web import Web, password_hash


def report(number=1):
    facts = dict.fromkeys(FACTS, 'unknown')
    facts.update(app_version='0.11.2', game_version='157.0', android_api='35', root='unavailable')
    return {'schema': 1, 'report_id': '00000000-0000-4000-8000-' + str(number).zfill(12),
            'issue': 'root_problem', 'facts': facts, 'events': ['ENABLE_REFUSED']}


class WebTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.password = 'synthetic-test-only-password'
        cls.hashed = password_hash(cls.password)

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.store = Store(self.tmp.name, per_minute=200)
        self.server = HTTPServer(('127.0.0.1', 0), handler(self.store))
        self.origin = 'http://127.0.0.1:' + str(self.server.server_port)
        self.web = Web(self.store, self.origin, self.hashed, True)
        self.server.RequestHandlerClass = handler(self.store, web=self.web)
        self.thread = threading.Thread(target=self.server.serve_forever, daemon=True)
        self.thread.start()
        self.cookie = self.csrf = ''

    def tearDown(self):
        self.server.shutdown(); self.server.server_close(); self.thread.join(); self.tmp.cleanup()

    def request(self, method, path, data=None, headers=None, browser=True):
        h = {'Origin': self.origin} if browser else {}
        if self.cookie:
            h['Cookie'] = self.cookie
        if self.csrf:
            h['X-CSRF-Token'] = self.csrf
        body = json.dumps(data).encode() if data is not None else None
        if body:
            h['Content-Type'] = 'application/json'
        h.update(headers or {})
        c = http.client.HTTPConnection('127.0.0.1', self.server.server_port, timeout=5)
        c.request(method, path, body, h)
        response = c.getresponse(); raw = response.read()
        result = (response.status, dict(response.getheaders()), raw)
        c.close()
        return result

    def login(self):
        status, headers, raw = self.request('POST', '/admin/api/login', {'password': self.password})
        self.assertEqual(status, 200)
        self.cookie = headers['Set-Cookie'].split(';')[0]
        self.csrf = json.loads(raw)['csrf']
        self.assertIn('HttpOnly', headers['Set-Cookie'])
        self.assertIn('SameSite=Strict', headers['Set-Cookie'])

    def upload(self, number=1):
        return self.request('POST', '/v1/reports', report(number), {'X-Seed-Lens-Consent': 'preview-v1'})

    def test_public_pages_and_no_public_payload(self):
        self.assertEqual(self.upload()[0], 201)
        for path in ['/', '/admin', '/site.css', '/upload.js', '/admin.js']:
            status, headers, raw = self.request('GET', path)
            self.assertEqual(status, 200)
            self.assertIn("frame-ancestors 'none'", headers['Content-Security-Policy'])
            self.assertEqual(headers['Cache-Control'], 'no-store')
            self.assertNotIn(report()['report_id'].encode(), raw)
        for path in ['/admin/api/reports','/admin/api/reports/'+report()['report_id'], '/admin/api/reports/'+report()['report_id']+'/download']:
            self.assertEqual(self.request('GET', path)[0], 401)
        for path in ['/../receiver.py', '/triage.sqlite3', '/'+report()['report_id']+'.json']:
            self.assertEqual(self.request('GET', path)[0], 404)

    def test_upload_consent_origin_validation_and_dedupe(self):
        self.assertEqual(self.request('POST', '/v1/reports', report())[0], 400)
        self.assertEqual(self.request('POST', '/v1/reports', report(), {'Origin':'https://evil.example','X-Seed-Lens-Consent':'preview-v1'})[0], 403)
        self.assertEqual(self.upload()[0], 201)
        self.assertEqual(self.upload()[0], 200)
        bad = report(2); bad['email'] = 'not-permitted@example.invalid'
        self.assertEqual(self.request('POST','/v1/reports',bad,{'X-Seed-Lens-Consent':'preview-v1'})[0],400)
        # Native client sends no Origin; protocol stays compatible.
        self.assertEqual(self.request('POST','/v1/reports',report(3),{'X-Seed-Lens-Consent':'preview-v1'},False)[0],201)

    def test_login_csrf_filters_status_persistence_download_and_logout(self):
        self.upload(); self.login()
        self.assertEqual(self.request('GET','/admin/api/session')[0],200)
        status, _, raw = self.request('GET','/admin/api/reports?state=new&api=35&game=157.0&app=0.11.2')
        self.assertEqual(status,200); self.assertEqual(json.loads(raw)['total'],1)
        path = '/admin/api/reports/' + report()['report_id']
        self.assertEqual(self.request('POST',path,{'state':'resolved'},{'X-CSRF-Token':'bad'})[0],403)
        self.assertEqual(self.request('POST',path,{'state':'resolved'},{'Origin':'https://evil.example'})[0],403)
        self.assertEqual(self.request('POST',path,{'state':'resolved'})[0],200)
        before = (Path(self.tmp.name)/(report()['report_id']+'.json')).read_bytes()
        self.assertEqual(json.loads(self.request('GET','/admin/api/reports?state=resolved')[2])['total'],1)
        reopened = Web(self.store,self.origin,self.hashed,True)
        self.assertEqual(reopened.reports()[0]['state'],'resolved')
        status, headers, raw = self.request('GET',path+'/download')
        self.assertEqual(status,200); self.assertIn('attachment;', headers['Content-Disposition'])
        self.assertEqual(json.loads(raw), report())
        self.assertEqual((Path(self.tmp.name)/(report()['report_id']+'.json')).read_bytes(),before)
        self.assertEqual(self.request('POST',path,{'state':'invalid'})[0],400)
        self.assertEqual(self.request('GET','/admin/api/reports?page=-1')[0],400)
        self.assertEqual(self.request('POST','/admin/api/logout',{})[0],200)
        self.assertEqual(self.request('GET',path)[0],401)

    def test_retention_and_pagination(self):
        for i in range(1,28): self.upload(i)
        self.login()
        data = json.loads(self.request('GET','/admin/api/reports?page=1')[2])
        self.assertEqual(data['total'],27); self.assertEqual(len(data['reports']),25)
        self.assertEqual(len(json.loads(self.request('GET','/admin/api/reports?page=2')[2])['reports']),2)
        path = '/admin/api/reports/' + report()['report_id']
        self.request('POST',path,{'state':'investigating'})
        file = Path(self.tmp.name)/(report()['report_id']+'.json')
        old = time.time()-RETENTION_SECONDS-1; os.utime(file,(old,old))
        self.web.reports()
        self.assertFalse(file.exists())
        with self.web.db() as db: self.assertEqual(db.execute('SELECT count(*) FROM triage').fetchone()[0],0)
        self.assertEqual(self.request('GET',path)[0],404)

    def test_login_rate_expiry_and_production_settings(self):
        self.assertEqual(self.request('POST','/admin/api/login',{'password':self.password},browser=False)[0],403)
        for _ in range(5): self.assertEqual(self.request('POST','/admin/api/login',{'password':'wrong-password-that-is-long'})[0],401)
        self.assertEqual(self.request('POST','/admin/api/login',{'password':self.password})[0],429)
        self.web.login_rates.clear(); self.login()
        for session in self.web.sessions.values(): session['until'] = 0
        self.assertEqual(self.request('GET','/admin/api/session')[0],401)
        with self.assertRaises(ValueError): Web(self.store,'http://reports.example.org',self.hashed)
        with self.assertRaises(ValueError): Web(self.store,self.origin,'',True)
        production = Web(self.store,'https://reports.example.org',self.hashed)
        self.assertTrue(production.secure); self.assertEqual(production.cookie_name,'__Host-seedlens')


if __name__ == '__main__': unittest.main()
