SchemaImporterExtension Import Integer サンプル
=============================================
string 型ではなく long 型および ulong 型として XML スキーマ整数型をインポートする SchemaImporterExtension の作成方法を示します。


サンプル言語実装
===============================
このサンプルは、次の言語実装で使用できます。
C#


コマンド プロンプトを使用してサンプルをビルドするには、次の操作を実行します。
=============================================
1. コマンド プロンプト ウィンドウを開いて、Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger ディレクトリに移動します。
2. 「msbuild [ソリューション ファイル名]」と入力します。


Visual Studio を使用してサンプルをビルドするには、次の操作を実行します。
=======================================
1. Windows エクスプローラを開いて、Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger ディレクトリに移動します。
2. .sln (ソリューション) ファイルのアイコンをダブルクリックして、Visual Studio でファイルを開きます。
3. [ビルド] メニューの [ソリューションのビルド] をクリックします。
アプリケーションが、既定の \bin または \bin\Debug ディレクトリでビルドされます。

このサンプルをビルドする場合は、さらに次の点を考慮する必要があります。
1.  アセンブリの厳密な名前を生成します。
2.  GAC にアセンブリを追加します。
3.  schemas.xml.serialization/schemaImporterExtension セクションの machine.config にアセンブリを追加します。


サンプルを実行するには、次の操作を実行します。
=================
1. コマンド プロンプトまたは Windows エクスプローラを使用して、新しい実行可能ファイルを格納しているディレクトリに移動します。
2. コマンド ラインで実行可能ファイル名を入力するか、またはサンプル実行可能ファイルのアイコンをダブルクリックして Windows エクスプローラから実行可能ファイルを起動します。

サンプルは、制限されたアクセス許可でビルドできますが、アセンブリを GAC に追加して machine.config を編集する必要があるので、サンプルをインストールする場合は、管理者権限が必要です。

machine.config 内のエントリ例は、次のとおりです。

system.xml.serialization
	schemaImporterExtensions
		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
	schemaImporterExtensions
system.xml.serialization


備考
=================
1.  xsd.exe、wsdl.exe、または WSDL の Web 参照の追加機能を実行します。Web 参照の追加機能では、xs:integer、xs:negativeInteger、xs:nonNegativeInteger、xs:positiveInteger、または xs:nonPositiveInteger を使用します。
2.  生成されたクラスが XML スキーマ整数型に対して string 型ではなく long 型または ulong 型を使用する点に注意してください。
