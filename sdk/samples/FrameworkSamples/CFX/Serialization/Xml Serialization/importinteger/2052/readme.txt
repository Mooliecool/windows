SchemaImporterExtension Import Integer 示例
=============================================
     演示如何编写 SchemaImporterExtension，以便将 XML 架构整数类型作为 long 和 ulong 而不是 string 进行导入。


示例语言实现
===============================
     本示例可用于下列语言实现中:
     C#


若要使用命令提示符生成示例，请执行下列操作:
=============================================
     1. 打开命令提示符窗口，定位到 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 目录。
     2. 键入 msbuild [Solution Filename]。


若要使用 Visual Studio 生成示例，请执行下列操作:
=======================================
     1. 打开 Windows 资源管理器，定位到 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 目录。
     2. 双击 .sln (解决方案)文件的图标，在 Visual Studio 中打开该文件。
     3. 在“生成”菜单中选择“生成解决方案”。
     应用程序将在默认的 \bin 或 \bin\Debug 目录中生成。

     生成本示例时还需要考虑以下事项:
     1. 生成程序集的强名称
     2. 向 GAC 添加程序集
     3. 向 schemas.xml.serialization/schemaImporterExtension 一节中的 machine.config 添加程序集


若要运行该示例，请执行下列操作:
=================
     1. 使用命令提示符或 Windows 资源管理器，定位到包含新的可执行文件的目录。
     2. 在命令行键入 [ExecutableFile]，或者双击 [SampleExecutable] 的图标从 Windows 资源管理器中启动该文件。

     使用有限权限即可生成该示例，但安装该示例需要管理员特权，因为必须向 GAC 添加程序集且必须编辑 machine.config。

     machine.config 中的示例项:

     system.xml.serialization
	schemaImporterExtensions
		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
	schemaImporterExtensions
     system.xml.serialization


备注
=================
     1.  运行 xsd.exe、wsdl.exe，或使用 xs:integer、xs:negativeInteger、xs:nonNegativeInteger、xs:positiveInteger 或 xs:nonPositiveInteger 在 WSDL 中添加 Web 引用
2.  请注意，生成的类使用 long 或 ulong 而不是 string 作为 XML 架构整数类型
