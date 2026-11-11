'use strict';
const $ = id => document.getElementById(id);
let raw = null, reportId = null, busy = false;
function ready() { $('send').disabled = busy || !raw || !$('consent').checked; }
$('file').addEventListener('change', async () => {
  raw = null; reportId = null; $('consent').checked = false; $('json').textContent = '選擇檔案後顯示';
  $('result').textContent = '未上傳任何資料。'; ready();
  const file = $('file').files[0];
  if (!file) return;
  $('filename').textContent = file.name;
  try {
    if (file.size > 16384 || file.size < 1) throw new Error('請選擇不超過 16 KiB 的診斷 JSON。');
    const bytes = new Uint8Array(await file.arrayBuffer());
    const text = new TextDecoder('utf-8', {fatal: true}).decode(bytes);
    const value = JSON.parse(text);
    if (!value || value.schema !== 1 || typeof value.report_id !== 'string' || !value.facts || !Array.isArray(value.events)) throw new Error('這不是透視鏡診斷格式，請重新匯出。');
    // Keep original bytes: server independently checks duplicate keys and closed schema.
    $('json').textContent = JSON.stringify(value, null, 2); $('preview').open = true;
    raw = bytes; reportId = value.report_id;
  } catch (e) { $('result').textContent = e.message || '無法讀取檔案。'; }
  ready();
});
$('consent').addEventListener('change', ready);
$('send').addEventListener('click', async () => {
  if (!raw || !$('consent').checked || busy) return;
  if (!$('email').checkValidity()) { $('email').reportValidity(); return; }
  const bytes = raw, id = reportId;
  busy = true; $('file').disabled = true; $('consent').disabled = true; $('email').disabled = true; ready(); $('result').textContent = '正在上傳…';
  const timeout = new AbortController(); const timer = setTimeout(() => timeout.abort(), 20000);
  try {
    const response = await fetch('/api/submit', {method: 'POST', headers: {'Content-Type': 'application/json', 'X-Seed-Lens-Consent': 'preview-v1'}, body: '{"report":'+new TextDecoder().decode(bytes)+',"email":'+JSON.stringify($('email').value.trim())+'}', credentials: 'omit', redirect: 'error', signal: timeout.signal});
    const data = await response.json();
    if (!response.ok || data.status !== 'accepted' || data.report_id !== id) {
      const errors = {invalid_email: 'Email 格式不正確，請更正或留空。',rate_limited: '上傳太頻繁，請稍後再試。', storage_limit: '接收空間已滿，請聯絡維護者。', invalid_report: '格式不符，請從 App 重新匯出診斷。', id_conflict: '編號與既有內容衝突，請重新產生報告。'};
      throw new Error(errors[data.code] || '未收到有效收據，無法確認上傳結果。');
    }
    $('result').textContent = '已收到報告。回報編號：' + id;
    raw = null; $('consent').checked = false;
  } catch (e) { $('result').textContent = e.name === 'AbortError' ? '連線逾時，結果未知。沒有自動重送；稍後可用同一檔案重試。' : (e.message || '連線中斷，結果未知；沒有自動重送。'); }
  finally { clearTimeout(timer); busy = false; $('file').disabled = false; $('consent').disabled = false; $('email').disabled = false; ready(); }
});
