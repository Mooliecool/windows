ASPX Hosting 範例
=======================
本範例示範如何結合 HttpListener 的功能來建立 Http 伺服器，將呼叫傳送到裝載的 Aspx 應用程式。.NET Framework 2.0 版本採用建置於 Http.Sys 上的 HttpListener 類別，讓使用者能夠建立獨立的 Http 伺服器。

本範例使用下列 HttpListener 功能:
1. 驗證
2. 啟用 SSL
3. 讀取安全連接的用戶端憑證


範例語言實作
===============================
本範例提供下列語言實作:
C#


若要使用命令提示字元建置範例:
=============================================

1. 開啟 SDK 命令提示字元視窗，然後巡覽至 AspxHost 目錄下的 CS 子目錄。

2. 輸入 msbuild AspxHostCS.sln。


若要使用 Visual Studio 建置範例:
=======================================

1. 開啟 Windows 檔案總管，然後巡覽至 AspxHost 目錄下的 CS 子目錄。

2. 按兩下 .sln (方案) 檔的圖示，在 Visual Studio 中開啟檔案。

3. 在 [建置] 功能表中，選取 [建置方案]。
應用程式會建置在預設的 \bin 或 \bin\Debug 目錄中。


若要執行範例:
=================
1. 使用命令提示字元或 Windows 檔案總管，巡覽至包含新增之可執行檔的目錄。
2. 在命令列輸入 AspxHostCS.exe，或是在 Windows 檔案總管中按兩下 AspxHostingCS.exe 的圖示，啟動可執行檔。 


備註
======================
1. 類別資訊

AspxHostCS.cs 檔案包含的主要類別可建立及設定接聽程式和 Aspx 應用程式。

AspxVirtualRoot.cs 檔案包含的類別可設定 HttpListener 接聽前置詞和支援的驗證配置。

AspxNetEngine.cs 檔案包含的類別可透過指派對應至實體目錄的虛擬別名來設定 Aspx 應用程式。

AspxPage.cs 檔案包含的類別可實作 SimpleWorkerRequest 類別並代表用戶端所要求的頁面。

AspxRequestInfo.cs file 檔案包含的資料保存類別可用來將相關資料從 HttpListenerContext 傳遞到裝載應用程式。

AspxException.cs 檔案包含自訂例外狀況類別。

Demopages 目錄包含範例 Aspx 頁面。


2. 範例用法
 
AspxHostCS.cs 檔案是一種類別，其包含的主要方法會啟動 HttpListener，並將實體目錄設定成裝載的 ASPX 應用程式。這個類別預設會嘗試將 DemoPages 目錄 (位於同一個範例目錄) 設定為虛擬別名 / 下的裝載應用程式。由於本範例中的 HttpListener 會接聽通訊埠 80，因此您可能必須停止 IIS 才能執行本範例。

 
修改程式碼以供個別使用: 

//如有必要，建立具有 http 通訊埠和 https 通訊埠的 AspxVirtualRoot 物件
                
AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


//將實體目錄設定為虛擬別名。

//TODO: 將實體目錄取代成要設定的目錄。

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


//TODO: 如果要加入驗證，請在這裡加入

//virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. 設定驗證配置
 
設定 AspxVirtualRoot 物件之後，請設定 AspxVirtualRoot 物件上的 AuthenticationScheme 欄位，以設定必要的驗證配置。

 
4. 啟用 SSL
 

若要啟用 SSL，您必須在需要 SSL 的通訊埠上，設定安裝在電腦存放區的伺服器憑證。如需如何使用 Httpcfg.exe 公用程式在通訊埠上設定伺服器憑證的詳細資訊，請參閱 Httpcfg 連結。

 
注意: 您也可以使用 Winhttpcertcfg 在通訊埠上設定伺服器憑證。


已知的問題
====================== 

問題: 
啟動應用程式時出現下列錯誤訊息:

"System.IO.FileNotFoundException: 找不到檔案或組件 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' 或其相依性的其中之一。系統找不到指定的檔案。檔案名稱: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'"

解決方法:
AspxHostCs.exe 檔案不在目前正在設定之實體目錄的 bin 目錄中。請將 AspxHostcs.exe 檔案複製到 bin 目錄中。


請參閱
============
請參閱 MSDN 上 .NET Framework SDK 文件中的 HttpListener 和 Aspx Hosting API 文件。
