# Big Num Multiplication Topic

## 拖慢原因

1. allocated memory but not to free them

## 想要改進

1. bit counters
2. 記憶體重複使用
3. 4n(n*n)記憶體使用
4. 工作記憶體不要一直要，初始就處理好
5. 內容不要一直搬遷
6. 遞迴深度管理，不要太深
7. library除了io都不要用

## 算分方式

1. 早交
2. 執行速度
3. log2(3)
4. 盡量200位內超車
5. 禁用malloc
6. 宣告array總長度<5000

## Warning

1. 我用了 ofstream 輸出每次跑的時間
2. 測資存在 ./data 資料夾裡
3. 結果存在 ./ans 資料夾裡
4. 參考檔案放在 ./ref 資料夾裡
