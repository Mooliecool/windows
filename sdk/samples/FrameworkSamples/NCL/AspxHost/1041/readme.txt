ASPX Hosting サンプル
=======================
このサンプルでは、HttpListener の機能を組み合わせて、ホストされる Aspx アプリケーションの呼び出しを転送する HTTP サーバーの作成方法を示します。.NET Framework のバージョン 2.0 では、Http.Sys 上に構築された HttpListener クラスを導入しています。このクラスを使用すれば、ユーザーがスタンドアロン HTTP サーバーを作成できます。

このサンプルは、HttpListener の次の機能を使用します。
1. 認証
2. SSL の有効化
3. セキュリティ保護された接続でのクライアント証明書の読み取り


サンプル言語実装
===============================
このサンプルは、次の言語実装で使用できます。
C#


コマンド プロンプトを使用してサンプルをビルドするには、次の操作を実行します。
=============================================

1. SDK コマンド プロンプト ウィンドウを開いて、AspxHost ディレクトリの下の CS サブディレクトリに移動します。

2. 「msbuild AspxHostCS.sln」と入力します。


Visual Studio を使用してサンプルをビルドするには、次の操作を実行します。
=======================================

1. Windows エクスプローラを開いて、AspxHost ディレクトリの下の CS サブディレクトリに移動します。

2. .sln (ソリューション) ファイルのアイコンをダブルクリックして、Visual Studio でファイルを開きます。

3. [ビルド] メニューの [ソリューションのビルド] をクリックします。
アプリケーションが、既定の \bin または \bin\Debug ディレクトリでビルドされます。


サンプルを実行するには、次の操作を実行します。
=================
1. コマンド プロンプトまたは Windows エクスプローラを使用して、新しい実行可能ファイルを格納しているディレクトリに移動します。
2. コマンド ラインで「AspxHostCS.exe」と入力するか、または AspxHostCS.exe のアイコンをダブルクリックして Windows エクスプローラからサンプルを起動します。 


備考
======================
1. クラス情報

AspxHostCS.cs ファイルには、リスナや Aspx アプリケーションを作成および設定するメイン クラスが含まれています。

AspxVirtualRoot.cs ファイルには、プリフィックスおよびサポートされている認証スキームを待機するように HttpListener を設定するクラスが含まれています。

AspxNetEngine.cs ファイルには、物理ディレクトリにマップする仮想エイリアスを割り当てることによって Aspx アプリケーションを設定するクラスが含まれています。

AspxPage.cs ファイルには、SimpleWorkerRequest クラスを実装してクライアントから要求されたページを表すクラスが含まれています。

AspxRequestInfo.cs ファイルには、関連データを HttpListenerContext からホストされたアプリケーションに渡すために使用されるデータ ホルダ クラスが含まれています。

AspxException.cs ファイルには、カスタム例外クラスが含まれています。

Demopages ディレクトリには、サンプル Aspx ページが格納されています。


2. サンプルの使用法
 
AspxHostCS.cs ファイルは、HttpListener を起動して物理ディレクトリをホストされた ASPX アプリケーションとして設定するメイン メソッドが含まれるクラスです。既定では、このクラスは、DemoPages ディレクトリ (同じサンプル ディレクトリにあります) を仮想エイリアス / の下のホストされたアプリケーションとして設定しようとします。このサンプルの HttpListener はポート 80 で待機するので、このサンプルを実行するために IIS を停止することが必要になる場合があります。

 
個々の使用に備えてコードを変更します。

//必要に応じて http ポートおよび https ポートで AspxVirtualRoot オブジェクトを作成します
                
AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


//物理ディレクトリを仮想エイリアスとして設定します。

//TODO: 設定されるディレクトリで物理ディレクトリを置き換えます。

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


//TODO: 認証を追加することになっている場合は、ここで追加します

//virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. 認証スキームの設定
 
AspxVirtualRoot オブジェクトを設定したら、AspxVirtualRoot オブジェクトの AuthenticationScheme フィールドを設定して、必要な認証スキームを設定します。

 
4. SSL の有効化
 

SSL を有効にするには、マシン ストアにインストールしたサーバー証明書を、SSL を必要とするポートで設定する必要があります。Httpcfg.exe ユーティリティを使用して、あるポートでサーバー証明書を設定する方法の詳細については、Httpcfg リンクを参照してください。

 
メモ : Winhttpcertcfg を、あるポートでサーバー証明書を設定するために使用することもできます。


既知の問題
====================== 

問題 :
アプリケーションを起動すると、次のエラー メッセージが表示されます。

"System.IO.FileNotFoundException: ファイルまたはアセンブリ 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'、またはその依存関係の 1 つが読み込めませんでした。システムでは、指定されたファイルが見つかりません。ファイル名: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'”

解決策:
AspxHostCs.exe ファイルが、設定対象の物理ディレクトリの bin ディレクトリにありません。AspxHostcs.exe ファイルを bin ディレクトリにコピーしてください。


参照
============
.NET Framework SDK ドキュメントと MSDN で、HttpListener および Aspx をホストする API に関するドキュメントを参照してください。