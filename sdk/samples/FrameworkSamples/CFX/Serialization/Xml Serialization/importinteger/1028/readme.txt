SchemaImporterExtension Import Integer 範例
=============================================
示範如何編寫 SchemaImporterExtension，以便將 XML 結構描述整數型別匯入成 long 和 ulong，而非 string。


範例語言實作
===============================
本範例提供下列語言實作:
C#


若要使用命令提示字元建置範例:
=============================================
1. 開啟命令提示字元視窗，然後巡覽至 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 目錄。
2. 輸入 msbuild [Solution Filename]。


若要使用 Visual Studio 建置範例:
=======================================
1. 開啟 Windows 檔案總管，然後巡覽至 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 目錄。
2. 按兩下 .sln (方案) 檔的圖示，在 Visual Studio 中開啟檔案。
3. 在 [建置] 功能表中，選取 [建置方案]。
應用程式會建置在預設的 \bin 或 \bin\Debug 目錄中。

在建置本範例時必須注意的其他事項:
1.  產生組件的強式名稱
2.  將組件加入 GAC
3.  將組件加入 machine.config 的 schemas.xml.serialization/schemaImporterExtension 區段中


若要執行範例:
=================
1. 使用命令提示字元或 Windows 檔案總管，巡覽至包含新增之可執行檔的目錄。
2. 在命令列輸入 [ExecutableFile]，或是在 Windows 檔案總管中按兩下 [SampleExecutable] 的圖示，啟動可執行檔。

本範例可以用有限的權限建置，但是安裝時必須要有系統管理權限，因為必須將組件加入 GAC 並編輯 machine.config。

machine.config 中的範例項目:

system.xml.serialization
	schemaImporterExtensions
		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
	schemaImporterExtensions
system.xml.serialization


備註
=================
1.  在使用 xs:integer、xs:negativeInteger、xs:nonNegativeInteger、xs:positiveInteger 或 xs:nonPositiveInteger 的 WSDL 上執行 xsd.exe、wsdl.exe 或 [加入 Web 參考]
2.  請注意，產生的類別會使用 long 或 ulong 取代 string 做為 XML 結構描述整數型別

