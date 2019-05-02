# rv32i-sim

![ss](https://pbs.twimg.com/media/D5h4QmuWsAMHOHj.jpg:large)

アーキテクチャ理解を深めるために、RISC-V 基本整数ISAであるrv32iとELFローダーをC++で実装しています。
gccでコンパイルした生成物を直接読み込んで実行することができます。

## 制約(+Known Issue)

* ELFファイルの種類に動的リンクが含まれるものは使用できない

ElfLoaderはProgram Headerのp_typeがLOADのものだけをMemにロードします。

* 使用できる命令セットはiのみ

マシンモードを実装していないため、例外であったり例えば`_exit()`の`ecall`でもプロセッサは停止します。CSRも実装されていないので割り込みや例外の取得などは行うことはできません。


## テスト方法

### rv32i向けテストコードのビルド

ビルドツールを実行できるDockerfileを用意しています。

```
$ cd /rv32i-sample-src
$ docker-compose up
```

`hello.c`から`hello.s`と`hello.o`を生成し、objdumpした結果を表示します。

### エミュレータ本体のビルド

必要なソースコードはすべて`/src`に配置しています。bazelを使用していますが、vscode用のtaskを用意しているのでこちらでもビルド可能です。

また実行時に上書き可能ですが、バイナリの所在をハードコードしてしまっているので気になるようでしたら修正してください。
またデバッグ情報の表示レベルも変更可能です。

[/src/main.cpp](https://github.com/kamiyaowl/rv32i-sim/blob/master/src/main.cpp#L9)

```
$ make build
```

### 実行

先にバイナリのパスを変更していない場合、実行時の第1引数にhello.oの場所を指定して実行してください。

## License

[MIT License](https://github.com/kamiyaowl/rv32i-sim/blob/master/LICENSE)