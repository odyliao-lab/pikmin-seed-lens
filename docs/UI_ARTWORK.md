# 森林手帳首頁插畫

2026-11-11，使用內建 image_gen 工具，依使用者提供的皮克敏／放大鏡參考圖製作新的
水粉風格透明素材。原始參考圖不進 Git；本檔記錄生成提示詞，不含帳號或診斷資料。

- 素材：`app/assets/art/forest-lens-mascot-v1.png`
- SHA256：`63bf13f79dcbda0c3cb4c3293764034ae70ee9f5d161bb5c0941d38e8ea095a4`
- 1536×1024、RGBA，2,561,297 bytes；四角alpha皆0，761,994個全透明pixel，alpha範圍0–254。
- 完整複製生成原檔並保留alpha，未覆寫使用者參考圖，沒有以Python改圖。
- 僅為首頁品牌插畫；操作文字、同意、狀態及開關由原生介面獨立呈現。
- 0.6.4起APK內採flat `assets/forest-lens-mascot-v1.png`；來源仍保留上述資料夾。
  Windows巢狀entry反斜線問題與實機顯示結果見[本輪UI驗收](FOREST_UI_QUALIFICATION.md)。

## 生成提示詞（built-in 模式）

Use case: style-transfer. Asset type: an offline Android app homepage mascot illustration for a forest field-notebook UI. Input image 1 is the edit target and character/composition reference. Transform this dense 3D garden scene into a compact, polished, hand-painted gouache companion cutout: the same recognizable red leaf-topped Pikmin with pointed nose holding a warm brass magnifying glass, looking at the terracotta seedling pot; keep the little blue Pikmin in its cream coffee-cup costume peeking happily inside the pot/magnifier. Preserve the friendly identities, leaf, magnifying-glass discovery idea, red/blue character colors and warm golden highlights. Simplify fine detail so it remains clear at 160dp. Forest journal style: soft brush texture, gentle earthy greens, cream and brass, crisp readable silhouettes, charming expressive eyes. Landscape composition around 3:2, all characters, leaves, glass rim, pot and hands fully inside the canvas with modest transparent padding, red character on left, pot and lens on right, balanced compact group. Actual transparent alpha background throughout outside the characters/objects; no forest scenery, no opaque rectangular background, no checkerboard, no text, no UI, no labels or watermark. Keep a tiny natural moss footing only, no cast shadow rectangle. This will sit on a warm ivory UI card, with heading and buttons outside the illustration.
