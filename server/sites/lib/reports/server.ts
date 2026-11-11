import upload from '../../assets/upload.html?raw';
import admin from '../../assets/admin.html?raw';
import css from '../../assets/site.css?raw';
import uploadJs from '../../assets/upload.js?raw';
import adminJs from '../../assets/admin.js?raw';
import { validate, contact, canonical, parseUnique, ID, AGE, STATES } from './policy.mjs';
type Env = {DB?: D1Database; ADMIN_EMAIL?: string; RATE_KEY?: string};
type User = {email:string;userId:string}|null;
const headers = {'Cache-Control':'no-store','X-Content-Type-Options':'nosniff','Referrer-Policy':'no-referrer','X-Frame-Options':'DENY','Content-Security-Policy':"default-src 'none'; script-src 'self'; style-src 'self'; connect-src 'self'; base-uri 'none'; frame-ancestors 'none'; form-action 'self'"};
function response(status:number, body:unknown, kind='application/json; charset=utf-8', extra={}) {return new Response(typeof body==='string'?body:JSON.stringify(body),{status,headers:{...headers,'Content-Type':kind,...extra}});}
async function hash(text:string) {return [...new Uint8Array(await crypto.subtle.digest('SHA-256',new TextEncoder().encode(text)))].map(b=>b.toString(16).padStart(2,'0')).join('');}
async function body(r:Request) {
 if (!r.headers.get('Content-Type')?.startsWith('application/json')) throw Error('json_required');
 const reader=r.body?.getReader(); if(!reader)throw Error('invalid_report');
 const chunks:Uint8Array[]=[];let size=0;
 const limit=new URL(r.url).pathname==='/v1/reports'?16384:18432;
 while(true){const {value,done}=await reader.read();if(done)break;size+=value.length;if(size>limit){await reader.cancel();throw Error('size_limit');}chunks.push(value);}
 const bytes=new Uint8Array(size);let p=0;for(const c of chunks){bytes.set(c,p);p+=c.length;}
 try{return parseUnique(new TextDecoder('utf-8',{fatal:true}).decode(bytes));}catch{throw Error('invalid_report');}
}
export async function handle(r:Request, env:Env, user:User) {
 const url=new URL(r.url), path=url.pathname, now=Math.floor(Date.now()/1000);
 const owner=Boolean(env.ADMIN_EMAIL && user?.email.toLowerCase()===env.ADMIN_EMAIL.toLowerCase());
 try {
  if(r.method==='GET') {
   const assets:Record<string,[string,string]>={'/':[upload,'text/html'],'/admin':[admin,'text/html'],'/site.css':[css,'text/css'],'/upload.js':[uploadJs,'text/javascript'],'/admin.js':[adminJs,'text/javascript']};
   if(assets[path] && !url.search){const [data,kind]=assets[path];return response(200,data,kind+'; charset=utf-8');}
  }
  if(r.method==='POST' && r.headers.get('Origin')!==url.origin && !(path==='/v1/reports'&&!r.headers.has('Origin')))return response(403,{code:'origin'});
  if(path.startsWith('/admin/api/')&&!owner)return response(user?403:401,{code:'owner_required'});
  if(path==='/admin/api/session'&&r.method==='GET')return response(200,{owner:true});
  if(!['/api/submit','/v1/reports'].includes(path)&&!path.startsWith('/admin/api/'))return response(404,{code:'not_found'});
  const db=env.DB;
  if(!db)return response(503,{code:'unavailable'});
  // Logical expiry always applies to reads. Physical cleanup runs on each request.
  await db.batch([db.prepare('DELETE FROM contacts WHERE id IN (SELECT id FROM reports WHERE created<=?)').bind(now-AGE),db.prepare('DELETE FROM reports WHERE created<=?').bind(now-AGE),db.prepare('DELETE FROM rates WHERE until<=?').bind(now)]);
  if((path==='/api/submit'||path==='/v1/reports')&&r.method==='POST'&&!url.search){
   if(r.headers.get('X-Seed-Lens-Consent')!=='preview-v1')return response(400,{code:'consent_required'});
   const data=await body(r);let report,email='';
   if(path==='/api/submit') {if(!data||Object.keys(data).sort().join('|')!=='email|report')throw Error('invalid_report');report=validate(data.report);email=contact(data.email);} else report=validate(data);
   if(!env.RATE_KEY)return response(503,{code:'unavailable'});
   const ip=r.headers.get('CF-Connecting-IP')||'unknown';const bucket=await hash(env.RATE_KEY+'|'+ip+'|'+Math.floor(now/60));
   const rate=await db.prepare('INSERT INTO rates(key,count,until) SELECT ?,1,? WHERE (SELECT count(*) FROM rates)<1024 ON CONFLICT(key) DO UPDATE SET count=count+1 WHERE count<5 RETURNING count').bind(bucket,now+60).first();
   if(!rate)return response(429,{code:'rate_limited'});
   const payload=canonical(report),digest=await hash(payload+'\n'+email);
   const stored=await db.prepare('SELECT digest FROM reports WHERE id=?').bind(report.report_id).first<{digest:string}>();
   if(stored)return response(stored.digest===digest?200:409,stored.digest===digest?{status:'accepted',report_id:report.report_id}:{code:'id_conflict'});
   const inserted=await db.batch([db.prepare('INSERT INTO reports(id,payload,digest,created,state) SELECT ?,?,?,?,\'new\' WHERE (SELECT count(*) FROM reports)<1000 ON CONFLICT(id) DO NOTHING').bind(report.report_id,payload,digest,now),db.prepare('INSERT INTO contacts(id,email) SELECT ?,? WHERE ?<>\'\' AND EXISTS(SELECT 1 FROM reports WHERE id=? AND digest=?) ON CONFLICT(id) DO NOTHING').bind(report.report_id,email,email,report.report_id,digest)]);
   if(!inserted[0].meta.changes){const existing=await db.prepare('SELECT digest FROM reports WHERE id=?').bind(report.report_id).first<{digest:string}>();return response(existing?(existing.digest===digest?200:409):503,existing?.digest===digest?{status:'accepted',report_id:report.report_id}:{code:existing?'id_conflict':'storage_limit'});}
   return response(201,{status:'accepted',report_id:report.report_id});
  }
  if(owner&&path.startsWith('/admin/api/')){
   if(path==='/admin/api/reports'&&r.method==='GET') {
    const q=url.searchParams; if([...q.keys()].some(k=>!['state','issue','app','game','api','page'].includes(k)))throw Error('invalid_request');
    const page=Number(q.get('page')||1);if(!Number.isInteger(page)||page<1||page>1000)throw Error('invalid_request');
    const all=await db.prepare('SELECT id,payload,created,state FROM reports WHERE created>? ORDER BY created DESC,id DESC LIMIT 1000').bind(now-AGE).all<{id:string;payload:string;created:number;state:string}>();
    let rows=all.results.map(x=>({...JSON.parse(x.payload),received_at:x.created,state:x.state}));
    const counts=Object.fromEntries(STATES.map((s:string)=>[s,rows.filter(x=>x.state===s).length]));
    for(const k of ['state','issue'])if(q.get(k))rows=rows.filter(x=>x[k]===q.get(k));
    for(const [key,field] of [['app','app_version'],['game','game_version'],['api','android_api']])if(q.get(key))rows=rows.filter(x=>x.facts[field]===q.get(key));
    return response(200,{reports:rows.slice((page-1)*25,page*25),total:rows.length,counts,page});
   }
   const match=path.match(/^\/admin\/api\/reports\/([^/]+)(\/download)?$/);
   if(match&&ID.test(match[1])&&!url.search){
    const row=await db.prepare('SELECT payload,state,created FROM reports WHERE id=? AND created>?').bind(match[1],now-AGE).first<{payload:string;state:string;created:number}>();
    if(!row)return response(404,{code:'not_found'});
    if(r.method==='GET'){if(match[2])return response(200,JSON.parse(row.payload),'application/json; charset=utf-8',{'Content-Disposition':'attachment; filename="'+match[1]+'.json"'});
     const c=await db.prepare('SELECT email FROM contacts WHERE id=?').bind(match[1]).first<{email:string}>();
     return response(200,{...JSON.parse(row.payload),contact_email:c?.email||'',state:row.state,received_at:row.created});}
    if(r.method==='POST'&&!match[2]) {const data=await body(r);if(Object.keys(data).join()!=='state'||!STATES.includes(data.state))throw Error('invalid_request');await db.prepare('UPDATE reports SET state=? WHERE id=?').bind(data.state,match[1]).run();return response(200,{state:data.state});}
   }
  }
  return response(404,{code:'not_found'});
 } catch(e) {const code=e instanceof Error?e.message:'';return response(['size_limit'].includes(code)?413:['invalid_report','invalid_email','invalid_request','json_required'].includes(code)?400:503,{code:['size_limit','invalid_report','invalid_email','invalid_request','json_required'].includes(code)?code:'unavailable'});}
}
