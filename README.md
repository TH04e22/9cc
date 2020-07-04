### 9cc編譯器
此編譯器是依照[網路教學](https://koshizuow.gitbook.io/compilerbook/)製作而成，十分簡易，但基本上有一個編譯器應該有的基礎流程:詞法分析(Lexer)、語法剖析(Parser)、代碼產生(Code generation)。

最難可編譯以下語句:
'_temp1 = 24 - 5 * 4; tem_4 = 12/3; num_tem = _temp1 + tem_4; return _temp1 + tem_4 + num_tem;'

使用方式:
1. 必須先將要編譯的語句用9cc編譯器編譯成組合語言
2. 用gcc將組合語言檔案製作成執行檔

原始檔說明
|  檔案名稱   | 說明  |
|  ----  | ----  |
| token.c | 將原始碼經由詞法分析轉換成token list |
| parser.c  | 將token list轉換成parser tree |
| codegen.c  | 經由parser tree產生組合語言 |
| util.c | 顯示編譯錯誤的函式 |
| 9cc.c  | 主程式 |
| 9cc.h  | 函式定義與符號 |