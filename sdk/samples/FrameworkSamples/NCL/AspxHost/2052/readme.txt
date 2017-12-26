ASPX Hosting 示例
=======================
本示例演示如何对 HttpListener 的功能进行组合以创建 Http 服务器，该服务器将传送对承载的 Aspx 应用程序的调用。.NET Framework 2.0 版引入了在 Http.Sys 上生成的 HttpListener 类，用户使用该类可以创建独立的 Http 服务器。

本示例使用 HttpListener 的下列功能:
1. 身份验证
2. 启用 SSL
3. 通过安全连接读取客户端证书


示例语言实现
===============================
     本示例可用于下列语言实现中:
     C#


若要使用命令提示符生成示例，请执行下列操作:
=============================================

     1. 打开 SDK 命令提示符窗口，定位到 AspxHost 目录下的 CS 子目录。

     2. 键入 msbuild AspxHostCS.sln。


若要使用 Visual Studio 生成示例，请执行下列操作:
=======================================

     1. 打开 Windows 资源管理器，定位到 AspxHost 目录下的 CS 子目录。

     2. 双击 .sln (解决方案)文件的图标，在 Visual Studio 中打开该文件。

     3. 在“生成”菜单中选择“生成解决方案”。
     应用程序将在默认的 \bin 或 \bin\Debug 目录中生成。


若要运行该示例，请执行下列操作:
=================
     1. 使用命令提示符或 Windows 资源管理器，定位到包含新的可执行文件的目录。
     2. 在命令行上键入 AspxHostCS.exe，或者双击 AspxHostCS.exe 的图标从 Windows 资源管理器中启动该文件。 


备注
======================
1. 类信息

AspxHostCS.cs 文件包含的主类用于创建和配置侦听器和 Aspx 应用程序。

AspxVirtualRoot.cs 文件包含的类用于配置 HttpListener，以侦听前缀和所支持的身份验证方案。

AspxNetEngine.cs 文件包含的类通过分配映射到物理目录的虚拟别名来配置 Aspx 应用程序。

AspxPage.cs 文件包含的类用于实现 SimpleWorkerRequest 类并表示客户端请求的页。

AspxRequestInfo.cs 文件包含的数据容器类用于将相关数据从 HttpListenerContext 传递到承载的应用程序。

AspxException.cs 文件包含自定义异常类。

Demopages 目录包含示例 Aspx 页。


2. 示例用法
 
AspxHostCS.cs 文件是包含主方法的类，该主方法将启动 HttpListener 并将物理目录配置为承载的 ASPX 应用程序。默认情况下，该类会将 DemoPages 目录（在同一个示例目录中）配置为虚拟别名 / 下某个承载的应用程序。由于本示例中的 HttpListener 侦听端口 80，因此，可能需要停止 IIS 才能运行本示例。

 
更改代码以适合各种用途: 

                //如果需要，创建具有 http 端口和 https 端口的 AspxVirtualRoot 对象
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //将物理目录配置为虚拟别名。

                //TODO:使用要配置的目录来替换物理目录。

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO:如果要添加身份验证，请在此处添加

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. 设置身份验证方案
 
配置 AspxVirtualRoot 对象之后，在 AspxVirtualRoot 对象中设置 AuthenticationScheme 字段，以设置所需的身份验证方案。

 
4. 启用 Ssl
 

要启用 SSL，必须在要求 SSL 的端口上配置服务器证书，该证书安装在计算机存储区中。有关如何使用 Httpcfg.exe 实用工具在端口上配置服务器证书的更多信息，请参阅 Httpcfg 链接。

 
注意：也可以使用 Winhttpcertcfg 在端口上配置服务器证书。


已知问题
====================== 

问题:
启动应用程序时，出现以下错误信息：

“System.IO.FileNotFoundException:未能加载文件或程序集‘AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null’或它的某一个依赖项。系统找不到指定文件。文件名:‘AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null’”
解决方法:
AspxHostCs.exe 文件不在所配置的物理目录的 bin 目录中。将 AspxHostcs.exe 文件复制到 bin 目录中。


请参阅
============
请参阅 .NET Framework SDK 文档中和 MSDN 上的 HttpListener 和 Aspx Hosting API 文档。