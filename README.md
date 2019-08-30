# SMF2MML

## 概要
このアプリケーションは、Standard MIDI Fileを Sakura仕様のMML(Music Macro Language)に逆コンパイルします。

## 別途インストールが必要なソフト
- Microsoft Visual C++ 2008 SP1 再頒布可能パッケージ (x86) 


## 使用方法
コマンドラインに、次の通りに打ち込んでください。
尚、（）内は省略可能です。

SMF2MML （/option） ファイル名（.mid）

ファイル名(.mid)は、逆コンパイルする元ファイルを指定します。
拡張子を省略した場合は、自動的に拡張子を".mid"とします。
拡張子がないファイルを指定する場合は、"ファイル名１."とピリオドまで記述して下さい。
元ファイルは、SMFのFormat 0若しくはFormat 1である必要があります。
指定されたファイル名で、拡張子をmmlとしてmmlファイルを出力します。 
MMLの文法は、別途、Sakuraのドキュメントを参照して下さい。

---------

## Overview
This application converts Sandard MIDI files into MML (Music Macro Language), for use with [Sakura MML](https://sakuramml.com/).

## Dependencies
- [Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=5582)

## Usage
Type the following into the command line:

```SMF2MML /option filename.mid```

`filename.mid` specifies the original file to be converted. It must be either SMF Format 0 or Format 1.

If the extension is omitted here, it will automatically be changed to `.mid`.

When specifying a file with no extension, describe up to "file name 1." and a period.

If no output filename is specified, an `.mml` file with the original filename will be generated.

For more information about the MML syntax, see [Sakura MML's documentation](https://sakuramml.com/).
