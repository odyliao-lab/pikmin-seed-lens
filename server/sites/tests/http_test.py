import http.client,json,uuid,unittest
HOST='127.0.0.1:5189';ORIGIN='http://'+HOST; RUN=str(uuid.uuid4())
FACTS='app_version app_code app_signer android_api android_release abi manufacturer game_version game_code root bridge selinux native_files last_phase last_exit'.split()
def report():return dict(schema=1,report_id=str(uuid.uuid4()),issue='root_problem',facts=dict.fromkeys(FACTS,'unknown'),events=['ENABLE_REFUSED'])
class Live(unittest.TestCase):
 def request(self,path,data=None,headers=None):
  h={'Origin':ORIGIN,'Content-Type':'application/json','CF-Connecting-IP':'synthetic-'+RUN+'-'+str(getattr(self,'ip',1))};h.update(headers or {})
  c=http.client.HTTPConnection(HOST,timeout=10);c.request('POST' if data is not None else 'GET',path,json.dumps(data) if data is not None else None,h);r=c.getresponse();body=r.read();out=(r.status,dict(r.getheaders()),body);c.close();return out
 def owner(self):
  c=http.client.HTTPConnection(HOST);c.request('GET','/signin-with-chatgpt?return_to=%2Fadmin');r=c.getresponse();cookies=[v.split(';')[0] for k,v in r.getheaders() if k.lower()=='set-cookie'];r.read();c.close();return {'Cookie':'; '.join(cookies)}
 def test_01_email_omitted_and_present(self):
  self.ip=21
  for email in ['','synthetic-fix@example.invalid']:
   p=report();body={'report':p,'email':email};headers={'X-Seed-Lens-Consent':'preview-v1'}
   a=self.request('/api/submit',body,headers);self.assertEqual(a[0],201,a[2]);self.assertEqual(json.loads(a[2])['report_id'],p['report_id'])
   self.assertEqual(self.request('/api/submit',body,headers)[0],200)
   path='/admin/api/reports/'+p['report_id']
   self.assertEqual(self.request(path)[0],401)
   a=self.request(path,headers=self.owner());self.assertEqual(a[0],200,a[2]);self.assertEqual(json.loads(a[2])['contact_email'],email)
   self.assertNotIn(b'contact_email',self.request(path+'/download',headers=self.owner())[2])
 def test_02_privacy_and_consent(self):
  self.ip=22;p=report();body={'report':p,'email':''}
  self.assertEqual(self.request('/api/submit',body)[0],400)
  self.assertEqual(self.request('/api/submit',body,{'X-Seed-Lens-Consent':'preview-v1','Origin':'https://evil.invalid'})[0],403)
  self.assertEqual(self.request('/api/submit',{'report':p,'email':'bad\r\n@example.invalid'},{'X-Seed-Lens-Consent':'preview-v1'})[0],400)
  self.assertEqual(self.request('/admin/api/reports',headers={'oai-authenticated-user-id':'other','oai-authenticated-user-email':'other@example.invalid'})[0],401)
  self.assertEqual(self.request('/admin/api/reports')[0],401)
 def test_03_rate_and_native(self):
  self.ip=23;p=report();h={'X-Seed-Lens-Consent':'preview-v1'}
  self.assertEqual(self.request('/v1/reports',p,h)[0],201)
  for _ in range(4):self.assertEqual(self.request('/v1/reports',p,h)[0],200)
  self.assertEqual(self.request('/v1/reports',p,h)[0],429)
 def test_04_status_and_filters(self):
  self.ip=24;p=report();p['facts']['app_version']='0.11.2'
  self.request('/api/submit',{'report':p,'email':''},{'X-Seed-Lens-Consent':'preview-v1'})
  path='/admin/api/reports/'+p['report_id'];h=self.owner()
  self.assertEqual(self.request(path,{'state':'resolved'},h)[0],200)
  a=self.request('/admin/api/reports?state=resolved&app=0.11.2',headers=h);self.assertEqual(a[0],200,a[2]);self.assertTrue(any(r['report_id']==p['report_id'] for r in json.loads(a[2])['reports']))
  self.assertEqual(self.request(path,{'state':'new'},{**h,'Origin':'https://evil.invalid'})[0],403)
 def test_05_app_upload_without_browser_origin(self):
  p=report();body={'report':p,'email':''}
  def native(path,data,extra=None):
   h={'Content-Type':'application/json','X-Seed-Lens-Consent':'preview-v1','CF-Connecting-IP':'synthetic-'+RUN+'-25'};h.update(extra or {})
   c=http.client.HTTPConnection(HOST,timeout=10);c.request('POST',path,json.dumps(data),h);r=c.getresponse();result=(r.status,r.read());c.close();return result
  self.assertEqual(native('/api/submit',body)[0],403)
  a=native('/api/submit',body,{'X-Seed-Lens-Client':'android-v1'})
  self.assertEqual(a[0],201,a[1]);self.assertEqual(json.loads(a[1])['report_id'],p['report_id'])
  self.assertEqual(native('/api/submit',body,{'X-Seed-Lens-Client':'android-v1'})[0],200)
  self.assertEqual(native('/api/submit',body,{'X-Seed-Lens-Client':'android-v1','Origin':'https://evil.invalid'})[0],403)
  self.assertEqual(native('/admin/api/reports/'+p['report_id'],{'state':'resolved'},{'X-Seed-Lens-Client':'android-v1'})[0],403)
  self.assertEqual(self.request('/admin/api/reports/'+p['report_id'],headers={'X-Seed-Lens-Client':'android-v1'})[0],401)
if __name__=='__main__':unittest.main()
