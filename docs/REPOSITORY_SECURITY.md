# Repo 安全設定

2026-11-11 以 GitHub API 設定並回讀；repo 維持私人，未合併 main。

## 已生效

- Actions 只允許 GitHub 官方 Actions，要求完整 commit SHA；不允許其他已驗證作者或額外白名單。
- Workflow token 預設唯讀，禁止 Actions 核准 PR。
- 私人 fork PR workflow 停用，且不傳遞寫入 token 或 secrets。
- Dependency graph 可匯出 SBOM；Dependabot 漏洞提醒與安全更新已啟用。

本分支新增每週子模組更新 PR 設定與安全政策；須合併至預設分支後才作為預設分支政策使用。不自動合併依賴更新，也不以 Dependabot 取代原生元件人工審查。

## 公開或方案支援後再啟用

| 項目 | 本次 API 結果／後續設定 |
|---|---|
| main Ruleset | 403：目前私人方案不支援。要求 PR、禁止 force push／刪除、解決 review 討論；單人維護不強制另一人核准。 |
| Secret scanning／Push protection | 422：目前 repo 不提供。可用後啟用並回讀確認。 |
| 私人漏洞回報 | 404：未啟用成功。公開後啟用 Report a vulnerability；SECURITY.md 本身不會啟用此功能。 |
| 公開 fork PR workflow 核准 | 422：不接受私人 repo。公開後選擇所有外部貢獻者均須核准。 |
| 必要 CI 檢查 | 功能分支已新增並跑通 Offline display policies，執行 native policy、實際設定 writer、JVM protocol 與契約交叉檢查；尚未合併 main 或設為必要 check，不代表 APK／遊戲驗收。 |

公開前須另外檢查完整 Git 歷史、子模組、附件的私人資料及授權。`.gitignore` 不會清除已追蹤或歷史資料；本次設定不等於完成歷史安全稽核。專案授權尚未選定。

[Rulesets](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-rulesets/available-rules-for-rulesets) · [GitHub 安全設定](https://docs.github.com/en/code-security/getting-started/quickstart-for-securing-your-repository) · [Actions 安全](https://docs.github.com/en/actions/reference/security/secure-use)
