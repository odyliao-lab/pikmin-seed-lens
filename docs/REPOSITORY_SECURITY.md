# Repo 安全設定

2026-11-11 依使用者授權公開 repo，程式碼採 [MIT](../LICENSE)。以下設定已透過 GitHub API 寫入並回讀確認。

## 已生效

- main 必須經 PR 與 `policy` CI，合併前須保持最新並解決 review 討論；禁止 force push／刪除，無 bypass。單人維護不強制第二人核准。
- Secret scanning 與 Push protection 已啟用。
- 私人漏洞回報已啟用；請使用 Security → Report a vulnerability，勿在公開 issue 貼敏感資料。
- Actions 只允許 GitHub 官方 Actions，要求完整 commit SHA；workflow token 預設唯讀，禁止 Actions 核准 PR。
- 所有外部貢獻者的 fork PR workflow 均須維護者核准。
- Dependency graph、Dependabot 漏洞提醒與安全更新已啟用。

每週子模組更新由 PR 審查，不自動合併；依賴更新仍須人工檢查。

## 公開前檢查

- Gitleaks 8.30.1 掃描全部 Git refs／歷史及 GitHub PR／issue 文字，未發現秘密；工具下載雜湊已核對官方 release digest。
- 歷史檔案清單未見 keystore、APK、遊戲二進位或原始診斷；兩層 pinned 子模組均指向公開來源。
- 0.9.5 發布原已實測 APK，保留 SHA256 與原簽章；未重建或改寫產物。驗收限制見[驗收矩陣](DISPLAY_PORT_RESULTS.md)。

掃描結果不保證不存在任何敏感資料。請持續避免提交金鑰、帳號資料與遊戲檔案；`.gitignore` 不會移除 Git 歷史內容。第三方元件保留原授權，角色與參考圖片不包含在程式碼 MIT 授權內。

[安全回報政策](../SECURITY.md) · [Rulesets](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-rulesets/available-rules-for-rulesets) · [Actions 安全](https://docs.github.com/en/actions/reference/security/secure-use)
