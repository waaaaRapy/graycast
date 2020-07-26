# Graycast
Graycast は学習目的の自作Cコンパイラです。

https://www.sigbus.info/compilerbook に従って実装していき、セルフホストすることを目標にしています。

# 進捗

- 電卓レベルの言語
  - [x] ステップ1: 整数1個をコンパイルする言語の作成
  - [x] ステップ2: 加減算のできるコンパイラの作成
  - [x] ステップ3: トークナイザを導入
  - [x] ステップ4: エラーメッセージを改良
  - [x] ステップ5: 四則演算のできる言語の作成
  - [x] ステップ6: 単項プラスと単項マイナス
  - [x] ステップ7: 比較演算子
- 関数とローカル変数
  - [x] ステップ9: 1文字のローカル変数
  - [x] ステップ10: 複数文字のローカル変数
  - [x] ステップ11: return文
  - [x] ステップ12: 制御構文を足す
  - [x] ステップ13: ブロック
  - [x] ステップ14: 関数の呼び出しに対応する
  - [x] ステップ15: 関数の定義に対応する
- ポインタと文字列リテラル
  - [ ] ステップ16: 単項&と単項*
  - [ ] ステップ17: 暗黙の変数定義を廃止して、intというキーワードを導入する
  - [ ] ステップ18: ポインタ型を導入する
  - [ ] ステップ19: ポインタの加算と減算を実装する
  - [ ] ステップ20: sizeof演算子
  - [ ] ステップ21: 配列を実装する
  - [ ] ステップ22: 配列の添字を実装する
  - [ ] ステップ23: グローバル変数を実装する
  - [ ] ステップ24: 文字型を実装する
  - [ ] ステップ25: 文字列リテラルを実装する
  - [ ] ステップ26: 入力をファイルから読む
  - [ ] ステップ27: 行コメントとブロックコメント
  - [ ] ステップ28: テストをCで書き直す
- ステップ29以降(WIP)