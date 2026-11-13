# 網站相依套件安全修復（2026-11-13）

範圍：`server/sites` 的網站與開發工具依賴，不改 Android APK、遊戲載入器或遊戲相容資格。

## 修復

- #27／#26 已更新 Next 16.4.6（含 ESLint 配套）及 Zod 4.6.7；#31 更新 Vite 8.3.8。
- Cloudflare plugin 1.69.1、Wrangler 4.161.0、workers-types 5.20261110.1 協調升級，帶入 Miniflare 5 的相容配套，更新 undici、ws、sharp 及 esbuild。Miniflare 版本為 upstream 工具鏈依賴的 beta；以完整 Worker／D1 回歸驗證，不能只靠安裝成功。
- 重新解析 Babel、Browserslist、browser mapping、source-map-js、js-yaml、brace-expansion 的安全版本，逐一核對 lockfile 的巢狀副本。
- `@typescript-eslint/typescript-estree → minimatch` scoped override 11.0.1；其他 minimatch 主版本保持原相依契約。
- `@esbuild-kit/core-utils → esbuild` scoped override 0.28.2；保留 Drizzle Kit 0.31.11，實際 CLI 在隔離副本讀取 TS schema／config 並產生新增 migration，原 migration 不變。
- `satori → fflate` scoped override 0.7.5，修復重新 audit 發現的 ZIP64 解析問題。補正常 ZIP 解壓縮與 SVG 產生測試。

這些 scoped override 是目前上游尚未更新依賴範圍的相容修補；移除前需確認父套件已採用安全版本，並保留相關測試。

## 防止回歸

CI 對所有依賴執行 `npm audit --audit-level=low`，遇新警示會阻擋；不排除 dev、不批次 dismiss、不用 `audit fix --force`。Dependabot 加入 `/server/sites` 每週更新，Cloudflare、框架及工具分組；major 更新獨立審核。

npm audit 與 GitHub 警示結果如下；audit 為執行當時 advisory 資料庫的結果，不保證未來無新增警示。

## 原 58 項警示逐項對照

所有編號均為本 repo Dependabot alert，依套件合併列示；沒有 dismiss 或忽略警示。對每個原 advisory 範圍核對 lockfile 所有同名副本，58 項均無殘留命中。

| 套件 | 原警示編號 | 修復後所有版本 |
|---|---|---|
| @babel/core | [#5](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/5) | 7.29.13 |
| baseline-browser-mapping | [#33](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/33) | 2.11.37 |
| brace-expansion | [#16](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/16), [#17](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/17), [#50](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/50), [#51](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/51) | 1.1.22, 5.0.13 |
| browserslist | [#30](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/30) | 4.29.5 |
| esbuild | [#1](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/1), [#3](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/3) | 0.25.12, 0.28.2 |
| js-yaml | [#15](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/15), [#18](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/18), [#29](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/29), [#34](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/34) | 4.3.2 |
| minimatch | [#85](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/85) | 11.0.1, 3.1.5 |
| next | [#53](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/53), [#56](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/56), [#57](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/57), [#58](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/58), [#59](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/59), [#60](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/60), [#61](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/61), [#67](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/67), [#68](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/68), [#69](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/69), [#70](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/70) | 16.4.6 |
| sharp | [#55](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/55) | 0.35.5 |
| source-map-js | [#54](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/54) | 1.2.2 |
| undici | [#8](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/8), [#9](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/9), [#10](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/10), [#11](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/11), [#13](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/13), [#14](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/14), [#24](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/24), [#25](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/25), [#26](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/26), [#27](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/27), [#28](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/28), [#38](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/38), [#39](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/39), [#40](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/40), [#41](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/41), [#44](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/44), [#45](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/45), [#78](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/78), [#81](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/81), [#82](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/82), [#83](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/83), [#84](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/84) | 7.30.1 |
| vite | [#6](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/6), [#7](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/7), [#62](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/62), [#63](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/63), [#64](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/64), [#65](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/65), [#66](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/66) | 8.3.8 |
| ws | [#4](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/4) | 8.21.0, 8.22.0 |
| zod | [#71](https://github.com/odyliao-lab/pikmin-seed-lens/security/dependabot/71) | 4.6.7 |

## 實際驗證與交付

- 合併 #31：`3239c12`；合併 #33：`fdb768e1c1898f8f0534fde825544e8bd4685870`。#32 的 brace-expansion 修復已由 #33 涵蓋並註記關閉。
- PR #33 與合併後 main 的 Offline policy、Sites website CI 均通過。網站乾淨安裝後 **28 項 protocol／policy／toolchain 測試**、TypeScript、Worker／client 建置、D1 migration 與 **5 項 HTTP 測試**通過。main 網站 run：[43374240356](https://github.com/odyliao-lab/pikmin-seed-lens/actions/runs/43374240356)；Offline policy run：[43374240514](https://github.com/odyliao-lab/pikmin-seed-lens/actions/runs/43374240514)。
- 2026-11-13 重新查詢 GitHub 開啟警示 **0 項**，npm audit（含 dev）**0 項**。新增發現的 fflate 相依鏈也已修復。
- Sites 專用來源 `e685901a8468a64462081896611a79ff712ca3cc`：重新乾淨安裝、audit 0、28 項測試、TypeScript 與完整建置通過；原始碼已由 Sites workflow 推送，對應 archive SHA256 `097c255101ec773a54b50192b0b7e196b13c0643d745cfa0f8dbd105f0b07683`。
- 正式站 **版本 4** 部署成功：`appgdep_6af6a544db808191bce7d77a1929766d`，環境 revision 1 不變，保留 D1 與公開回報／站長登入設定。
- 正式站回歸：以目前產品 `DiagnosticUploader`／`DiagnosticSubmission`／`DiagnosticReport` 在 JVM 傳送合成 0.0.0/code0 報告，無 Origin 或登入 cookie，核對相同成功收據；另用瀏覽器選檔、預覽、合成選填 Email、勾選同意並上傳。兩份報告均可在站長後台篩選、查看，下載 JSON 編號／封閉 schema 相符，下載不含 Email。兩份合成測試均標為已處理，未更改其他回報。
- 2026-11-13（Asia/Taipei）：正式端未登入及偽造身分管理請求均 401；跨來源上傳 403；無 Origin 且無 native 標記 403；native 標記但缺同意 400。未傳 DoS／讀檔攻擊載荷。

正式站：[問題回報](https://seed-lens-reports.ody-liao.chatgpt.site)。原始 JSON、完整回報編號、警示快照、建置 archive 與畫面證據留在 ignored `.tools/security-review-20261113/`，不提交 Git。

## 驗收界線

這輪為網站依賴安全修復與正式服務回歸，未建立新 APK、未啟動 MuMu，JVM 上傳不記為 Android 裝置操作驗收。先前 API32／35 安裝版 App 上傳紀錄仍見 [Sites 驗收](SITES_PRODUCTION_20261112.md)。已登入非站長隔離、真實裝置離線／TLS、Email 寄送、固定清除排程及耐久仍未新增驗證。零已知依賴警示不代表完整應用程式安全稽核或未來零漏洞。
