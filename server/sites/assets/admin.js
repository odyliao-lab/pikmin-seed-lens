'use strict';
const $ = id => document.getElementById(id);
const issues = {app_wont_open:'App 無法開啟',root_problem:'Root 問題',cannot_enable:'無法啟用',text_unchanged:'文字未變更',restore_problem:'還原問題',other:'其他'};
const states = {new:'待處理',investigating:'處理中',resolved:'已處理'};
const labels = {app_version:'App 版本',app_code:'App 代碼',app_signer:'App 簽章',android_api:'Android API',android_release:'Android 版本',abi:'ABI',manufacturer:'製造商',game_version:'遊戲版本',game_code:'遊戲代碼',root:'Root',bridge:'NativeBridge',selinux:'SELinux',native_files:'原生檔案',last_phase:'最近狀態',last_exit:'退出分類'};
let csrf = '', page = 1, selected = '', current = [];
function message(text) { $('message').textContent = text; }
function auth(yes) { $('login').hidden = yes; $('dashboard').hidden = !yes; $('logout').hidden = !yes; if (!yes) {csrf = ''; selected = ''; current = []; $('reports').replaceChildren(); $('facts').replaceChildren(); $('events').textContent = ''; $('contact-email').textContent = ''; $('report-detail').hidden = true;} }
async function api(path, body) {
  const response = await fetch(path, {method: body ? 'POST' : 'GET', headers: body ? {'Content-Type':'application/json'} : {}, body: body ? JSON.stringify(body) : undefined, credentials:'same-origin', redirect:'error'});
  if (response.status === 401 || response.status === 403) { auth(false); throw new Error('請重新登入。'); }
  if (!response.ok) throw new Error(response.status === 429 ? '嘗試次數過多，請 15 分鐘後再試。' : '操作失敗（' + response.status + '），請稍後重試。');
  return response.json();
}
function node(tag, text, cls) { const n = document.createElement(tag); if (text !== undefined) n.textContent = text; if (cls) n.className = cls; return n; }
async function load() {
  const query = new URLSearchParams(new FormData($('filters'))); query.set('page', page);
  const data = await api('/admin/api/reports?' + query);
  current = data.reports;
  for (const state of Object.keys(states)) $(state + '-count').textContent = data.counts[state];
  $('total').textContent = data.total + ' 份'; $('page').textContent = '第 ' + page + ' 頁';
  $('prev').disabled = page <= 1; $('next').disabled = page * 25 >= data.total;
  $('reports').replaceChildren();
  for (const row of current) {
    const b = node('button', undefined, 'report'); b.type = 'button'; b.setAttribute('aria-current', String(row.report_id === selected));
    const top = node('div', undefined, 'report-top'); top.append(node('strong', issues[row.issue]), node('span', states[row.state], 'badge ' + row.state));
    b.append(top, node('small', 'App ' + row.facts.app_version + ' · 遊戲 ' + row.facts.game_version + ' · API ' + row.facts.android_api), node('small', new Date(row.received_at * 1000).toLocaleString('zh-TW') + ' · ' + row.report_id.slice(0,8)));
    b.addEventListener('click', () => detail(row).catch(e => message(e.message))); $('reports').append(b);
  }
  if (!current.length) $('reports').append(node('p', '沒有符合條件的回報。', 'muted'));
  $('empty').hidden = false; $('report-detail').hidden = true; selected = '';
}
async function detail(row) {
  const report = await api('/admin/api/reports/' + row.report_id);
  selected = row.report_id; $('empty').hidden = true; $('report-detail').hidden = false;
  $('report-id').textContent = selected; $('received').textContent = '接收時間：' + new Date(row.received_at * 1000).toLocaleString('zh-TW');
  $('contact-email').textContent = report.contact_email ? '修正通知 Email：' + report.contact_email + '（未驗證地址）' : '未提供通知 Email';
  $('facts').replaceChildren(); for (const [k,v] of Object.entries(report.facts)) $('facts').append(node('dt', labels[k] || k), node('dd', v === 'unknown' ? '未知' : v));
  $('events').textContent = report.events.length ? report.events.join('\n') : '無事件紀錄'; $('triage').value = row.state;
  $('download').href = '/admin/api/reports/' + selected + '/download';
  for (const [i,b] of [...$('reports').children].entries()) b.setAttribute('aria-current', String(current[i]?.report_id === selected));
}
function run(action) { return async e => { if (e) e.preventDefault(); message(''); try { await action(); } catch (err) { message(err.message); } }; }
$('filters').addEventListener('submit', run(async () => {page = 1; await load();}));
$('refresh').addEventListener('click', run(load));
$('prev').addEventListener('click', run(async () => {page--; await load();}));
$('next').addEventListener('click', run(async () => {page++; await load();}));
$('save').addEventListener('click', run(async () => { if (!selected) return; await api('/admin/api/reports/' + selected, {state:$('triage').value}); await load(); message('已儲存處理狀態。'); }));
run(async () => { const data = await api('/admin/api/session'); csrf = ''; auth(true); await load(); })();
