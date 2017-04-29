fsfltview
====
### 概要
このソースコードは、Windowsのフィルタマネージャを使ってボリュームとフィルタインスタンスを列挙する例を示したものです。  
ビルドした実行ファイルは管理者モードで実行してください。

### ビルド
Visual Studio 2010のプロジェクトを添付しています。より新しいVisual Studioでビルドする場合は、ソリューションファイルを読み込んで最新バージョンへの移行を行ってください。  

Windows 7 WDK(Windows Driver Kit) 7.1.0の開発環境でビルドするためのsourcesファイルも含まれています。  

Windows 7 WDKは次の場所からダウンロードできます。  
[https://www.microsoft.com/en-us/download/details.aspx?id=11800](https://www.microsoft.com/en-us/download/details.aspx?id=11800)


例えばWDKで32ビット版実行ファイルを作成する場合、スタートメニューから、
[Windows Driver Kits]->[WDK 7600.16385.1]->[Build Environments]->[Windows 7]のフォルダの”x86 Free Build Environment”を開き、ソースコードのディレクトリに移動してから、以下の様にbuildコマンドを実行します。  
・ビルド後に完成した実行ファイルを動作させる場合は、管理者モードで開いてください。  
・Windows 10ではスタートメニューに正しく表示されない場合があります。

#### ビルド
```sh
build -c
```
ビルドが成功したら、ソースコードディレクトリ下の"objfre_win7_x86\i386"に移動するか、実行ファイルのパスに付加して以下のファイルを実行します。

#### 実行
```sh
fsfltview
```

### 利用条件
このソースコードは自由に利用することができます。  
このコードは現状のまま提供するもので、作者は利用した結果や誤謬などに関する責任を負いません。

