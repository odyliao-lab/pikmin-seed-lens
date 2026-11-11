"""Seed Lens report ingress. Loopback only; put a TLS reverse proxy in front."""
import argparse
import hashlib
import ipaddress
import json
import os
from pathlib import Path
import re
import secrets
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

MAX_BYTES = 16384
RETENTION_SECONDS = 29 * 86400
ID = re.compile(r"[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}")
FACTS = {
    'app_version', 'app_code', 'app_signer', 'android_api', 'android_release',
    'abi', 'manufacturer', 'game_version', 'game_code', 'root', 'bridge',
    'selinux', 'native_files', 'last_phase', 'last_exit',
}
ISSUES = {'app_wont_open', 'root_problem', 'cannot_enable', 'text_unchanged', 'restore_problem', 'other'}
EVENTS = {'ENABLE_BEGIN', 'ENABLE_ON', 'ENABLE_WAITING', 'ENABLE_REFUSED', 'ENABLE_UNKNOWN',
          'ENABLE_UNCERTAIN', 'ENABLE_RESTART_REQUIRED', 'ENABLE_UNSUPPORTED', 'ENABLE_UNVERIFIED',
          'ENABLE_OFF', 'GROUP_SAVED', 'ALL_OFF_BEGIN', 'ALL_OFF_SAVED', 'ALL_OFF_FAILED', 'LOADER_QUERY_FAILED'} | {'LOADER_EXIT_'+str(code) for code in (0,1,2,3,4,6,22,23,24,25,26,27,28,42,43)}
ENUMS = {
 'abi': {'arm64-v8a', 'x86_64', 'armeabi-v7a', 'x86', 'other'},
 'manufacturer': {'google','samsung','xiaomi','oneplus','oppo','vivo','sony','asus','motorola','huawei','other'},
 'root': {'not_checked','granted','unavailable','denied_or_incompatible','timeout','error'},
 'bridge': {'libnb.so','libndk_translation.so','none','other'},
 'selinux': {'enforcing','permissive','disabled'},
 'native_files': {'match','mismatch','unreadable'},
 'last_phase': {'UNVERIFIED','WAITING','ON','OFF','REFUSED','UNKNOWN','UNCERTAIN','RESTART_REQUIRED','UNSUPPORTED'},
 'last_exit': {'none','java_crash','native_crash','anr','low_memory','user_or_system','other','unavailable'},
}

def unique_object(pairs):
    result = {}
    for k, v in pairs:
        if k in result:
            raise ValueError('duplicate key')
        result[k] = v
    return result


def validate(raw):
    if not isinstance(raw, bytes) or len(raw) > MAX_BYTES:
        raise ValueError('size')
    try:
        data = json.loads(raw.decode('utf-8'), object_pairs_hook=unique_object)
    except RecursionError:
        raise ValueError('nested JSON') from None
    if type(data) is not dict or set(data) != {'schema','report_id','issue','facts','events'}:
        raise ValueError('schema')
    if type(data['schema']) is not int or data['schema'] != 1:
        raise ValueError('schema version')
    if type(data['report_id']) is not str or not ID.fullmatch(data['report_id']):
        raise ValueError('id')
    if type(data['issue']) is not str or data['issue'] not in ISSUES:
        raise ValueError('issue')
    f = data['facts']
    if type(f) is not dict or set(f) != FACTS:
        raise ValueError('facts')
    for key, value in f.items():
        if type(value) is not str or len(value) > 80:
            raise ValueError('value')
        if value == 'unknown':
            continue
        if key in {'app_version','game_version','android_release'}:
            ok = bool(re.fullmatch(r'[0-9]{1,4}(\.[0-9]{1,4}){0,3}', value)) or key == 'game_version' and value == 'missing'
        elif key in {'app_code','game_code','android_api'}:
            ok = bool(re.fullmatch(r'[0-9]{1,10}', value))
        elif key == 'app_signer':
            ok = bool(re.fullmatch(r'[a-f0-9]{64}', value))
        else:
            ok = value in ENUMS[key]
        if not ok:
            raise ValueError('unexpected field content')
    events = data['events']
    if type(events) is not list or len(events) > 16 or any(type(e) is not str or e not in EVENTS for e in events):
        raise ValueError('events')
    return data


class Store:
    def __init__(self, directory, max_reports=1000, per_minute=5):
        path = Path(directory).absolute()
        if path.is_symlink():
            raise ValueError('symlink data directory')
        path.mkdir(mode=0o700, parents=True, exist_ok=True)
        self.path = path.resolve()
        if os.name != 'nt':
            if self.path.stat().st_uid != os.getuid() or self.path.stat().st_mode & 0o077:
                raise ValueError('data directory must be owned and mode 0700')
        self.max_reports, self.per_minute = max_reports, per_minute
        self.lock, self.salt, self.rates = threading.Lock(), secrets.token_bytes(32), {}
        self.purge()

    def purge(self):
        cutoff = time.time() - RETENTION_SECONDS
        for file in self.path.glob('*.json'):
            if ID.fullmatch(file.stem) and not file.is_symlink() and file.stat().st_mtime < cutoff:
                file.unlink()

    def accept(self, raw, address):
        report = validate(raw)
        canonical = json.dumps(report, sort_keys=True, separators=(',', ':')).encode('utf-8')
        key = hashlib.sha256(self.salt + address.encode('ascii')).digest()
        with self.lock:
            now = time.monotonic()
            self.rates = {k: v for k, v in self.rates.items() if now - v[0] < 60}
            entry = self.rates.get(key, (now, 0))
            if entry[1] >= self.per_minute or key not in self.rates and len(self.rates) >= 1024:
                return 429, 'rate_limited', report['report_id']
            self.rates[key] = (entry[0], entry[1] + 1)
            self.purge()
            target = self.path / (report['report_id'] + '.json')
            if target.exists() or target.is_symlink():
                if target.is_symlink() or target.stat().st_size > MAX_BYTES:
                    return 409, 'id_conflict', report['report_id']
                if target.read_bytes() == canonical:
                    return 200, 'accepted', report['report_id']
                return 409, 'id_conflict', report['report_id']
            if sum(1 for f in self.path.glob('*.json')) >= self.max_reports:
                return 503, 'storage_limit', report['report_id']
            fd = os.open(target, os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, 'O_NOFOLLOW', 0), 0o600)
            try:
                with os.fdopen(fd, 'wb') as out:
                    out.write(canonical)
                    out.flush()
                    os.fsync(out.fileno())
            except BaseException:
                target.unlink(missing_ok=True)
                raise
            if os.name != 'nt':
                directory_fd = os.open(self.path, os.O_RDONLY | os.O_DIRECTORY)
                try:
                    os.fsync(directory_fd)
                finally:
                    os.close(directory_fd)
            return 201, 'accepted', report['report_id']


def handler(store, trust_proxy=False, web=None):
    class Receiver(BaseHTTPRequestHandler):
        server_version = 'SeedLensReceiver'
        sys_version = ''
        def setup(self):
            super().setup()
            self.connection.settimeout(5)
            self.seedlens_trust_proxy = trust_proxy
        def log_message(self, *_):
            pass
        def reply(self, status, payload):
            body = json.dumps(payload, separators=(',', ':')).encode('utf-8')
            self.send_response(status)
            self.send_header('Content-Type','application/json')
            self.send_header('Content-Length',str(len(body)))
            self.send_header('Cache-Control','no-store')
            self.send_header('X-Content-Type-Options','nosniff')
            self.end_headers()
            self.wfile.write(body)
        def do_GET(self):
            if web and web.dispatch(self):
                return
            self.reply(404, {'status':'rejected','code':'not_found'})
        def do_POST(self):
            if web and web.dispatch(self):
                return
            if self.path != '/v1/reports':
                self.reply(404, {'status':'rejected','code':'not_found'}); return
            if self.headers.get('X-Seed-Lens-Consent') != 'preview-v1':
                self.reply(400, {'status':'rejected','code':'consent_required'}); return
            lengths = self.headers.get_all('Content-Length') or []
            if len(lengths) != 1 or not re.fullmatch(r'[0-9]{1,6}', lengths[0]) or self.headers.get('Transfer-Encoding'):
                self.reply(411, {'status':'rejected','code':'length_required'}); return
            size = int(lengths[0])
            if not 1 <= size <= MAX_BYTES:
                self.reply(413, {'status':'rejected','code':'size_limit'}); return
            if self.headers.get('Content-Type','').split(';')[0].strip() != 'application/json':
                self.reply(415, {'status':'rejected','code':'json_required'}); return
            try:
                raw = self.rfile.read(size)
                if len(raw) != size:
                    raise ValueError('short request')
                address = self.client_address[0]
                if trust_proxy:
                    address = str(ipaddress.ip_address(self.headers.get('X-Real-IP','')))
                status, state, report_id = store.accept(raw, address)
                self.reply(status, {'status':'accepted','report_id':report_id} if state == 'accepted' else {'status':'rejected','code':state})
            except (ValueError, UnicodeError, json.JSONDecodeError):
                self.reply(400, {'status':'rejected','code':'invalid_report'})
            except (TimeoutError, OSError):
                self.reply(503, {'status':'rejected','code':'unavailable'})
    return Receiver


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--data-dir', required=True)
    parser.add_argument('--port', type=int, default=8109)
    parser.add_argument('--trust-loopback-proxy', action='store_true')
    parser.add_argument('--public-origin', help='Enable upload/admin website, e.g. https://reports.example.org')
    parser.add_argument('--local-http', action='store_true', help='Loopback preview only; never production')
    args = parser.parse_args()
    if hasattr(os, 'getuid') and os.getuid() == 0:
        parser.error('Run as an unprivileged dedicated service account, never root')
    store = Store(args.data_dir)
    web = None
    if args.public_origin:
        from web import Web
        web = Web(store, args.public_origin, os.environ.get('SEEDLENS_ADMIN_HASH', ''), args.local_http)
    elif args.local_http:
        parser.error('--local-http requires --public-origin')
    server = HTTPServer(('127.0.0.1', args.port), handler(store, args.trust_loopback_proxy, web))
    server.timeout = 3600
    try:
        while True:
            server.handle_request()
            store.purge()
            if web:
                web.reports()  # Expire associated triage metadata even without admin visits.
    finally:
        server.server_close()

if __name__ == '__main__':
    main()
