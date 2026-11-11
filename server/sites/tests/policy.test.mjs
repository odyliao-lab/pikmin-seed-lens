import {test} from 'node:test';
import assert from 'node:assert/strict';
import {validate, contact, parseUnique, canonical, postAllowed} from '../lib/reports/policy.mjs';
import {FACTS} from '../lib/reports/contract.mjs';
const report=()=>({schema:1,report_id:'00000000-0000-4000-8000-000000000001',issue:'root_problem',facts:Object.fromEntries(FACTS.map(k=>[k,'unknown'])),events:['ENABLE_REFUSED']});
test('native envelope has no browser Origin dependency; never permits cross-origin or admin',()=>{
 const req=headers=>new Request('https://reports.example.invalid/api/submit',{method:'POST',headers});
 const native={'X-Seed-Lens-Client':'android-v1','Content-Type':'application/json; charset=utf-8'};
 assert.equal(postAllowed(req(native),'/api/submit'),true);
 assert.equal(postAllowed(req({}),'/api/submit'),false);
 assert.equal(postAllowed(req({...native,Origin:'https://evil.invalid'}),'/api/submit'),false);
 assert.equal(postAllowed(req(native),'/admin/api/reports/id'),false);
 assert.equal(postAllowed(req({...native,'Content-Type':'text/plain'}),'/api/submit'),false);
 assert.equal(postAllowed(req({Origin:'https://reports.example.invalid'}),'/api/submit'),true);
});
test('closed report contract preserves unknown and rejects email inside diagnostic',()=>{
 assert.deepEqual(validate(report()),report());let bad=report();bad.email='synthetic@example.invalid';assert.throws(()=>validate(bad));
 bad=report();bad.facts.account='private';assert.throws(()=>validate(bad));bad=report();bad.facts.root='absent';assert.throws(()=>validate(bad));
 bad=report();bad.schema=true;assert.throws(()=>validate(bad));bad=report();bad.events=['arbitrary text'];assert.throws(()=>validate(bad));
});
test('optional email supports omission and rejects injection and malformed values',()=>{
 for(const v of [undefined,'','  '])assert.equal(contact(v),'');
 assert.equal(contact(' user+fix@example.invalid '),'user+fix@example.invalid');
 for(const v of [null,[],{},'a@b','name <a@example.invalid>','a@example.invalid\r\nBcc:x@evil.invalid','a..b@example.invalid','.a@example.invalid','a.@example.invalid','a'.repeat(65)+'@example.invalid'])assert.throws(()=>contact(v));
});
test('duplicate keys including escaped equivalents and overdeep JSON rejected',()=>{
 assert.deepEqual(parseUnique(JSON.stringify(report())),report());
 for(const v of ['{"a":1,"a":2}','{"a":1,"\\u0061":2}','[1,]','{"a":1,}','['.repeat(40)+'0'+']'.repeat(40)])assert.throws(()=>parseUnique(v));
 assert.equal(canonical({z:2,a:{b:1}}),canonical({a:{b:1},z:2}));
});
