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

本機候選 lockfile audit：0 項。工具鏈測試、protocol／policy 測試與 CI／部署結果後續記錄於本文件；audit 為執行當時 advisory 資料庫的結果，不保證未來無新增警示。正式站須另外同步部署，repo 合併本身不會更新服務。
