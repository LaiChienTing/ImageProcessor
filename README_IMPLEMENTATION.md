# 區域選取與放大功能 - 實作說明

## 📋 功能概述

此 Pull Request 實作了主視窗的區域選取功能，允許使用者選擇圖片的一個區域並在新視窗中放大顯示。放大視窗提供完整的繪圖和存檔功能。

## ✨ 主要功能

### 1. 區域選取
- **操作方式**：按住 `Ctrl` + 滑鼠左鍵拖曳
- **視覺回饋**：即時顯示藍色虛線選取框
- **最小區域**：10x10 像素
- **座標系統**：正確處理 QLabel 縮放與圖片座標的轉換

### 2. 放大視窗
- **自訂倍率**：支援 1.0 到 10.0 倍的放大
- **捲軸支援**：使用 QScrollArea 處理大圖
- **記憶體管理**：視窗關閉時自動釋放資源

### 3. 畫筆功能
- **顏色選擇**：支援 QColorDialog 選色
- **寬度調整**：1 到 20 像素的可調寬度
- **流暢繪圖**：使用 QPainter 的圓角筆刷
- **清除功能**：一鍵恢復原始放大圖

### 4. 存檔功能
- **格式支援**：PNG、JPEG、BMP
- **完整儲存**：包含所有繪製內容
- **位置選擇**：彈出檔案對話框

## 🔧 技術實作

### 新增檔案
```
zoomwindow.h              - 放大視窗類別宣告 (58 行)
zoomwindow.cpp            - 放大視窗類別實作 (192 行)
FEATURE_GUIDE.md          - 使用者功能指南 (84 行)
IMPLEMENTATION_SUMMARY.md - 實作總結文件 (165 行)
README_IMPLEMENTATION.md  - 本檔案
```

### 修改檔案
```
imageprocessor.h          - 加入區域選取相關宣告 (+14 行)
imageprocessor.cpp        - 實作區域選取邏輯 (+137 行)
ImageProcessor.pro        - 加入新原始檔 (+4 行)
.gitignore                - 排除建置目錄 (+2 行)
```

### 總計變更
- **8 個檔案**
- **+653 行程式碼**
- **-5 行刪除**

## 🎯 設計決策

### 1. 座標系統處理
問題：QLabel 使用 `setScaledContents(true)` 會縮放圖片
解決：實作 `labelToImageCoords()` 方法進行座標轉換

```cpp
// 將 label 座標轉換為實際圖片座標
QPoint ImageProcessor::labelToImageCoords(const QPoint &labelPos)
{
    double scaleX = static_cast<double>(imgSize.width()) / labelSize.width();
    double scaleY = static_cast<double>(imgSize.height()) / labelSize.height();
    // ... 轉換邏輯
}
```

### 2. 事件處理機制
問題：如何在 QLabel 上捕捉繪圖滑鼠事件
解決：使用事件過濾器 (eventFilter) 模式

```cpp
imageLabel->installEventFilter(this);

bool ZoomWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == imageLabel && event)
    {
        // 處理滑鼠事件
    }
}
```

### 3. 避免循環依賴
問題：ImageProcessor 和 ZoomWindow 互相引用
解決：使用前置宣告 (forward declaration)

```cpp
// imageprocessor.h
class ZoomWindow;  // 前置宣告

// imageprocessor.cpp
#include "zoomwindow.h"  // 實作檔中才包含
```

### 4. 安全性考量
- 所有指標操作前都加入空指標檢查
- 事件轉型後驗證有效性
- 座標範圍使用 `qBound()` 限制

## 📝 程式碼品質

### 註解規範
✅ 所有新增和修改的程式碼都有繁體中文註解
✅ 關鍵邏輯有詳細說明
✅ 符合原專案的註解風格

### C++ 最佳實踐
✅ 使用 `override` 關鍵字
✅ 正確的 const 使用
✅ RAII 資源管理
✅ Qt 信號槽機制

### 相容性
✅ 不影響現有的開啟檔案功能
✅ 不影響幾何轉換功能
✅ 不影響原有的放大/縮小功能
✅ 保留所有原始的滑鼠事件處理

## 🧪 測試指南

### 編譯
```bash
qmake ImageProcessor.pro
make
```

### 測試案例

#### 1. 基本功能測試
- [ ] 載入圖片
- [ ] 按住 Ctrl + 拖曳選取區域
- [ ] 確認選取框顯示正確
- [ ] 輸入放大倍率
- [ ] 驗證放大視窗開啟

#### 2. 座標轉換測試
- [ ] 測試不同大小的圖片
- [ ] 測試視窗縮放後的選取
- [ ] 驗證選取區域與實際圖片對應

#### 3. 畫筆功能測試
- [ ] 選擇不同顏色繪圖
- [ ] 調整畫筆寬度
- [ ] 連續繪製多條線
- [ ] 測試清除功能

#### 4. 存檔功能測試
- [ ] 測試 PNG 格式
- [ ] 測試 JPEG 格式
- [ ] 測試 BMP 格式
- [ ] 驗證繪圖內容包含在存檔中

#### 5. 邊界條件測試
- [ ] 選取框超出圖片範圍
- [ ] 選取極小區域 (<10x10)
- [ ] 最大放大倍率 (10.0)
- [ ] 最小放大倍率 (1.0)

#### 6. 相容性測試
- [ ] 確認原有功能正常運作
- [ ] 多個放大視窗同時開啟
- [ ] 視窗關閉後記憶體釋放

## 🐛 已知限制

1. **座標顯示**：原有的滑鼠座標顯示仍使用視窗座標，未轉換為圖片座標
2. **選取視覺**：選取框在主視窗重繪時才更新，在快速移動時可能有延遲
3. **繪圖復原**：目前不支援復原/重做功能
4. **觸控支援**：未測試觸控螢幕的操作

## 🚀 未來改進建議

### 短期改進
- [ ] 加入更多繪圖工具（矩形、圓形、文字）
- [ ] 支援復原/重做功能
- [ ] 加入圖層系統
- [ ] 鍵盤快速鍵支援

### 長期改進
- [ ] 儲存選取歷史記錄
- [ ] 批次處理多個區域
- [ ] 預覽模式
- [ ] 自動對比度調整

## 📚 相關文件

- [FEATURE_GUIDE.md](FEATURE_GUIDE.md) - 使用者功能指南
- [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - 詳細實作總結

## 👥 貢獻者

- 實作：GitHub Copilot
- 審查：程式碼自動審查工具
- 測試：待專案維護者進行

## 📄 授權

遵循原專案授權條款。

---

**實作狀態**：✅ 完成
**程式碼審查**：✅ 通過
**測試狀態**：⏳ 待測試（需要 Qt 環境）
