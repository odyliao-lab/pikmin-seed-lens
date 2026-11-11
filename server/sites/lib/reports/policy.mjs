import { FACTS, ISSUES, EVENTS, ENUMS } from './contract.mjs';
export const ID = /^[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}$/;
export const STATES = ['new','investigating','resolved'];
export const AGE = 29*86400;
// Public ingress is not authenticated by this header. It selects a native wire
// contract; browsers cannot add it cross-origin without a denied CORS preflight.
export function postAllowed(request,path) {
  const origin=request.headers.get('Origin');
  if(origin!==null)return origin===new URL(request.url).origin;
  return path==='/v1/reports'||path==='/api/submit'&&request.headers.get('X-Seed-Lens-Client')==='android-v1'&&request.headers.get('Content-Type')?.split(';')[0].trim()==='application/json';
}
const exact = (o, keys) => o && typeof o === 'object' && !Array.isArray(o) && Object.keys(o).sort().join('|') === [...keys].sort().join('|');
export function validate(report) {
  if (!exact(report,['schema','report_id','issue','facts','events']) || report.schema !== 1 || !ID.test(report.report_id) || !ISSUES.includes(report.issue) || !exact(report.facts, FACTS)) throw Error('invalid_report');
  for (const [key,value] of Object.entries(report.facts)) {
    if (typeof value !== 'string' || value.length > 80) throw Error('invalid_report');
    if (value === 'unknown') continue;
    let valid;
    if (['app_version','game_version','android_release'].includes(key)) valid = /^\d{1,4}(\.\d{1,4}){0,3}$/.test(value) || key === 'game_version' && value === 'missing';
    else if (['app_code','game_code','android_api'].includes(key)) valid = /^\d{1,10}$/.test(value);
    else if (key === 'app_signer') valid = /^[a-f0-9]{64}$/.test(value);
    else valid = ENUMS[key]?.includes(value);
    if (!valid) throw Error('invalid_report');
  }
  if (!Array.isArray(report.events) || report.events.length > 16 || report.events.some(e => !EVENTS.includes(e))) throw Error('invalid_report');
  if (new TextEncoder().encode(JSON.stringify(report)).length > 16384) throw Error('size_limit');
  return report;
}
export function contact(value) {
  if (value === undefined || value === '') return '';
  if (typeof value !== 'string') throw Error('invalid_email');
  value = value.trim();
  // ASCII mailbox only; no display names, line breaks, URL or header injection.
  if (!value) return '';
  if (value.length > 254 || !/^[A-Za-z0-9.!#$%&'*+\/= ?^_`{|}~-]+@[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?(?:\.[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?)+$/.test(value) || /\s/.test(value) || value.split('@')[0].length > 64 || value.startsWith('.') || value.includes('..') || value.split('@')[0].endsWith('.')) throw Error('invalid_email');
  return value;
}
export function canonical(o) {
  if (Array.isArray(o)) return '['+o.map(canonical).join(',')+']';
  if (o && typeof o === 'object') return '{'+Object.keys(o).sort().map(k=>JSON.stringify(k)+':'+canonical(o[k])).join(',')+'}';
  return JSON.stringify(o);
}
// Reject duplicate keys, including escaped equivalents, before JSON.parse.
export function parseUnique(text) {
  let i=0, depth=0;
  function ws(){while(/\s/.test(text[i] || '') && i<text.length)i++;}
  function string(){const start=i++;while(i<text.length){const c=text[i++];if(c==='"')return JSON.parse(text.slice(start,i));if(c==='\\')i++;}throw Error('invalid_report');}
  function value(){ws();if(++depth>32)throw Error('invalid_report');const c=text[i];
    if(c==='{'){i++;const seen=new Set();ws();if(text[i]!=='}')while(true){ws();if(text[i]!=='"')throw Error('invalid_report');const k=string();if(seen.has(k))throw Error('invalid_report');seen.add(k);ws();if(text[i++]!==':')throw Error('invalid_report');value();ws();if(text[i]!==',')break;i++;}if(text[i++]!=='}')throw Error('invalid_report');}
    else if(c==='['){i++;ws();if(text[i]!==']')while(true){value();ws();if(text[i]!==',')break;i++;}if(text[i++]!==']')throw Error('invalid_report');}
    else if(c==='"')string();else {const start=i;while(i<text.length&&!/[\s,}\]]/.test(text[i]))i++;if(start===i)throw Error('invalid_report');JSON.parse(text.slice(start,i));}depth--;}
  value();ws();if(i!==text.length)throw Error('invalid_report');return JSON.parse(text);
}
