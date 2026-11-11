"""Same-origin upload page and single-maintainer dashboard. No third-party runtime."""
import getpass
import hashlib
import hmac
import json
import os
from pathlib import Path
import re
import secrets
import sqlite3
import time
from contextlib import contextmanager
from http.cookies import SimpleCookie, CookieError
from urllib.parse import parse_qs, urlsplit

from receiver import ID, ISSUES, MAX_BYTES, RETENTION_SECONDS, validate

STATES = {'new', 'investigating', 'resolved'}
ASSETS = Path(__file__).with_name('web')


def password_hash(password, salt=None):
    if len(password) < 16 or len(password) > 256:
        raise ValueError('Password must contain 16–256 characters')
    salt = salt or secrets.token_hex(16)
    digest = hashlib.scrypt(password.encode(), salt=bytes.fromhex(salt), n=16384, r=8, p=1).hex()
    return 'scrypt$' + salt + '$' + digest


class Web:
    def __init__(self, store, origin, admin_hash, local_http=False):
        parsed = urlsplit(origin)
        if parsed.path or parsed.query or parsed.fragment or parsed.username or parsed.password or not parsed.hostname:
            raise ValueError('Use an origin without path, user info, query or fragment')
        if local_http:
            if parsed.scheme != 'http' or parsed.hostname not in {'127.0.0.1', 'localhost'}:
                raise ValueError('HTTP preview must use loopback origin')
        elif parsed.scheme != 'https':
            raise ValueError('HTTPS is required')
        if not re.fullmatch(r'scrypt\$[a-f0-9]{32}\$[a-f0-9]{128}', admin_hash):
            raise ValueError('Set SEEDLENS_ADMIN_HASH using web.py; no default credentials')
        self.store, self.origin, self.admin_hash = store, origin, admin_hash
        self.secure = not local_http
        self.sessions, self.login_rates = {}, {}
        self.rate_salt = secrets.token_bytes(32)
        self.cookie_name = '__Host-seedlens' if self.secure else 'seedlens-local'
        db_path = store.path / 'triage.sqlite3'
        if db_path.is_symlink():
            raise ValueError('symlink database')
        fd = os.open(db_path, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600) if not db_path.exists() else None
        if fd is not None:
            os.close(fd)
        if os.name != 'nt' and (db_path.stat().st_mode & 0o077 or db_path.stat().st_uid != os.getuid()):
            raise ValueError('Database must be owned and mode 0600')
        self.db_path = db_path
        with self.db() as db:
            db.execute('CREATE TABLE IF NOT EXISTS triage (id TEXT PRIMARY KEY, state TEXT NOT NULL)')

    @contextmanager
    def db(self):
        # Journal is transient and inherits database permissions. No payloads/credentials in DB.
        connection = sqlite3.connect(self.db_path)
        try:
            with connection:
                yield connection
        finally:
            connection.close()

    def reports(self):
        self.store.purge()
        with self.db() as db:
            states = dict(db.execute('SELECT id,state FROM triage'))
            result = []
            for path in self.store.path.glob('*.json'):
                if not ID.fullmatch(path.stem) or path.is_symlink() or path.stat().st_size > MAX_BYTES:
                    continue
                try:
                    data = validate(path.read_bytes())
                    if data['report_id'] != path.stem:
                        continue
                    result.append({'report_id': path.stem, 'received_at': int(path.stat().st_mtime),
                                   'state': states.get(path.stem, 'new'), 'issue': data['issue'], 'facts': data['facts']})
                except (ValueError, OSError, UnicodeError):
                    continue
            existing = {r['report_id'] for r in result}
            db.executemany('DELETE FROM triage WHERE id=?', [(i,) for i in states if i not in existing])
        return sorted(result, key=lambda r: (r['received_at'], r['report_id']), reverse=True)

    def session(self, request):
        now = time.monotonic()
        self.sessions = {k: v for k, v in self.sessions.items() if now < v['until']}
        try:
            cookie = SimpleCookie(request.headers.get('Cookie', ''))
            token = cookie[self.cookie_name].value if self.cookie_name in cookie else ''
        except CookieError:
            token = ''
        return self.sessions.get(hashlib.sha256(token.encode()).digest())

    def send(self, r, status, body, kind='application/json; charset=utf-8', cookie=None, download=None):
        if not isinstance(body, bytes):
            body = json.dumps(body, ensure_ascii=False).encode()
        r.send_response(status)
        for key, value in {
            'Content-Type': kind, 'Content-Length': str(len(body)), 'Cache-Control': 'no-store',
            'X-Content-Type-Options': 'nosniff', 'Referrer-Policy': 'no-referrer',
            'X-Frame-Options': 'DENY', 'Permissions-Policy': 'camera=(), microphone=(), geolocation=()',
            'Content-Security-Policy': "default-src 'none'; script-src 'self'; style-src 'self'; connect-src 'self'; img-src 'self'; base-uri 'none'; frame-ancestors 'none'; form-action 'self'",
        }.items():
            r.send_header(key, value)
        if self.secure:
            r.send_header('Strict-Transport-Security', 'max-age=31536000')
        if cookie:
            r.send_header('Set-Cookie', cookie)
        if download:
            r.send_header('Content-Disposition', 'attachment; filename="' + download + '"')
        r.end_headers()
        r.wfile.write(body)

    def body(self, r, limit=2048):
        lengths = r.headers.get_all('Content-Length') or []
        if len(lengths) != 1 or not re.fullmatch(r'[0-9]{1,6}', lengths[0]) or r.headers.get('Transfer-Encoding'):
            raise ValueError('length')
        size = int(lengths[0])
        if not 1 <= size <= limit or r.headers.get('Content-Type', '').split(';')[0] != 'application/json':
            raise ValueError('body')
        raw = r.rfile.read(size)
        if len(raw) != size:
            raise ValueError('short')
        data = json.loads(raw)
        if type(data) is not dict:
            raise ValueError('object')
        return data

    def dispatch(self, r):
        url = urlsplit(r.path)
        path = url.path
        # Existing native upload protocol remains unchanged; browser uploads require exact Origin.
        if path == '/v1/reports':
            if r.headers.get('Origin') is not None and r.headers.get('Origin') != self.origin:
                self.send(r, 403, {'code': 'origin'}); return True
            return False
        if r.command == 'GET' and not url.query:
            names = {'/': ('upload.html', 'text/html'), '/admin': ('admin.html', 'text/html'),
                     '/site.css': ('site.css', 'text/css'), '/upload.js': ('upload.js', 'text/javascript'),
                     '/admin.js': ('admin.js', 'text/javascript')}
            if path in names:
                name, kind = names[path]
                self.send(r, 200, (ASSETS / name).read_bytes(), kind + '; charset=utf-8'); return True
        if not path.startswith('/admin/api/'):
            return False
        try:
            if r.command == 'POST' and r.headers.get('Origin') != self.origin:
                self.send(r, 403, {'code': 'origin'}); return True
            if path == '/admin/api/login' and r.command == 'POST' and not url.query:
                address = r.client_address[0]
                # Do not trust spoofable proxy headers unless receiver was explicitly configured.
                if getattr(r, 'seedlens_trust_proxy', False):
                    import ipaddress
                    address = str(ipaddress.ip_address(r.headers.get('X-Real-IP', '')))
                now = time.monotonic()
                self.login_rates = {k: v for k, v in self.login_rates.items() if now - v[0] < 900}
                key = hashlib.sha256(self.rate_salt + address.encode()).digest()
                started, attempts = self.login_rates.get(key, (now, 0))
                if attempts >= 5 or key not in self.login_rates and len(self.login_rates) >= 1024:
                    self.send(r, 429, {'code': 'login_rate_limited'}); return True
                self.login_rates[key] = (started, attempts + 1)
                data = self.body(r)
                password = data.get('password')
                if type(password) is not str or not 16 <= len(password) <= 256 or not hmac.compare_digest(password_hash(password, self.admin_hash.split('$')[1]), self.admin_hash):
                    self.send(r, 401, {'code': 'login_failed'}); return True
                self.sessions = {k: v for k, v in self.sessions.items() if now < v['until']}
                if len(self.sessions) >= 128:
                    self.send(r, 503, {'code': 'session_limit'}); return True
                token, csrf = secrets.token_urlsafe(32), secrets.token_urlsafe(32)
                self.sessions[hashlib.sha256(token.encode()).digest()] = {'until': now + 8*3600, 'csrf': csrf}
                cookie = self.cookie_name + '=' + token + '; Path=/; HttpOnly; SameSite=Strict; Max-Age=28800' + ('; Secure' if self.secure else '')
                self.send(r, 200, {'csrf': csrf}, cookie=cookie); return True
            session = self.session(r)
            if session is None:
                self.send(r, 401, {'code': 'login_required'}); return True
            if r.command == 'POST' and not hmac.compare_digest(r.headers.get('X-CSRF-Token', ''), session['csrf']):
                self.send(r, 403, {'code': 'csrf'}); return True
            if path == '/admin/api/session' and r.command == 'GET' and not url.query:
                self.send(r, 200, {'csrf': session['csrf']}); return True
            if path == '/admin/api/logout' and r.command == 'POST' and not url.query:
                self.sessions = {k: v for k, v in self.sessions.items() if v is not session}
                self.send(r, 200, {'status': 'logged_out'}, cookie=self.cookie_name + '=; Path=/; HttpOnly; SameSite=Strict; Max-Age=0' + ('; Secure' if self.secure else '')); return True
            reports = self.reports()
            if path == '/admin/api/reports' and r.command == 'GET':
                q = parse_qs(url.query)
                if set(q) - {'state','issue','app','game','api','page'} or any(len(v) != 1 for v in q.values()):
                    raise ValueError('query')
                page = int(q.get('page', ['1'])[0])
                if page < 1 or page > 1000:
                    raise ValueError('page')
                counts = {state: sum(r['state'] == state for r in reports) for state in STATES}
                for key, field in [('state', 'state'), ('issue', 'issue')]:
                    if q.get(key, [''])[0]:
                        reports = [row for row in reports if row[field] == q[key][0]]
                for key, field in [('app','app_version'), ('game','game_version'), ('api','android_api')]:
                    if q.get(key, [''])[0]:
                        reports = [row for row in reports if row['facts'][field] == q[key][0]]
                self.send(r, 200, {'reports': reports[(page-1)*25:page*25], 'total': len(reports), 'counts': counts, 'page': page}); return True
            match = re.fullmatch(r'/admin/api/reports/(' + ID.pattern + r')(/download)?', path)
            if match and not url.query:
                report_id = match[1]
                if not any(row['report_id'] == report_id for row in reports):
                    self.send(r, 404, {'code': 'not_found'}); return True
                if r.command == 'GET':
                    data = validate((self.store.path / (report_id + '.json')).read_bytes())
                    self.send(r, 200, data, download=(report_id + '.json') if match[2] else None); return True
                if r.command == 'POST' and not match[2]:
                    data = self.body(r)
                    if set(data) != {'state'} or data['state'] not in STATES:
                        raise ValueError('state')
                    with self.db() as db:
                        db.execute('INSERT INTO triage VALUES (?,?) ON CONFLICT(id) DO UPDATE SET state=excluded.state', (report_id, data['state']))
                    self.send(r, 200, {'state': data['state']}); return True
            self.send(r, 404, {'code': 'not_found'})
        except (ValueError, TypeError, UnicodeError, RecursionError):
            self.send(r, 400, {'code': 'invalid_request'})
        except (OSError, sqlite3.Error):
            self.send(r, 503, {'code': 'unavailable'})
        return True


if __name__ == '__main__':
    # Interactive only: password never appears in command history or source.
    first = getpass.getpass('New admin password (16–256 characters): ')
    second = getpass.getpass('Repeat password: ')
    if first != second:
        raise SystemExit('Passwords differ')
    print(password_hash(first))
