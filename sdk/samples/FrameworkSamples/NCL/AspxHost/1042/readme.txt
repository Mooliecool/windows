ASPX 호스팅 샘플
=======================
이 샘플에서는 HttpListener의 기능을 조합하여 호스팅된 Aspx 응용 프로그램에 대한 호출을 라우팅하는 Http 서버를 만드는 방법을 보여 줍니다. .NET Framework 버전 2.0에는 Http.Sys에 기초한 HttpListener 클래스가 포함되어 있으며, 이 클래스를 사용하면 독립 실행형 Http 서버를 만들 수 있습니다.

이 샘플은 다음 HttpListener 기능을 사용합니다.
1. 인증
2. SSL 사용
3. 보안 연결에서 클라이언트 인증서 읽기


샘플 언어 구현
===============================
     이 샘플은 다음과 같은 언어 구현에서 사용할 수 있습니다.
     C#


명령 프롬프트를 사용하여 샘플을 빌드하려면
=============================================

     1. SDK 명령 프롬프트 창을 열고 AspxHost 디렉터리 아래의 CS 하위 디렉터리로 이동합니다.

     2. msbuild AspxHostCS.sln을 입력합니다.


Visual Studio를 사용하여 샘플을 빌드하려면
=======================================

     1. Windows 탐색기를 열고 AspxHost 디렉터리 아래의 CS 하위 디렉터리로 이동합니다.

     2. .sln(솔루션) 파일의 아이콘을 두 번 클릭하여 Visual Studio에서 해당 파일을 엽니다.

     3. [빌드] 메뉴에서 [솔루션 빌드]를 선택합니다.
     응용 프로그램은 기본적으로 \bin 또는 \bin\Debug 디렉터리에 빌드됩니다.


샘플을 실행하려면
=================
     1. 명령 프롬프트나 Windows 탐색기를 사용하여 새 실행 파일이 포함된 디렉터리로 이동합니다.
     2. 명령줄에 AspxHostCS.exe를 입력하거나 Windows 탐색기에서 AspxHostingCS.exe 아이콘을 두 번 클릭하여 샘플을 실행합니다. 


설명
======================
1. 클래스 정보

AspxHostCS.cs 파일에는 수신기와 Aspx 응용 프로그램을 만들고 구성하는 주 클래스가 들어 있습니다.

AspxVirtualRoot.cs 파일에는 접두사와 지원되는 인증 체계를 수신 대기하기 위해 HttpListener를 구성하는 클래스가 들어 있습니다.

AspxNetEngine.cs 파일에는 실제 디렉터리에 매핑하는 가상 별칭을 지정하여 Aspx 응용 프로그램을 구성하는 클래스가 들어 있습니다.

AspxPage.cs 파일에는 SimpleWorkerRequest 클래스를 구현하고 클라이언트에서 요청하는 페이지를 나타내는 클래스가 들어 있습니다.

AspxRequestInfo.cs 파일에는 관련 데이터를 HttpListenerContext에서 호스팅된 응용 프로그램으로 전달하는 데 사용하는 데이터 소유자 클래스가 들어 있습니다.

AspxException.cs 파일에는 사용자 지정 예외 클래스가 들어 있습니다.

Demopages 디렉터리에는 샘플 Aspx 페이지가 들어 있습니다.


2. 샘플 사용
 
AspxHostCS.cs 파일은 HttpListener를 시작하고 실제 디렉터리를 호스팅된 ASPS 응용 프로그램으로 구성하는 main 메서드가 들어 있는 클래스입니다. 기본적으로 이 클래스는 DemoPages 디렉터리(동일한 샘플 디렉터리 안에 있음)를 가상 별칭 / 아래에 호스팅된 응용 프로그램으로 구성합니다.  이 샘플의 HttpListener는 포트 80에서 수신하므로 이 샘플을 실행하려면 IIS를 중지해야 할 수도 있습니다.

 
각각의 사용에 적합하도록 코드를 수정합니다.

                //필요한 경우 http 포트 및 https 포트를 사용하여 AspxVirtualRoot 개체를 만듭니다.
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //실제 디렉터리를 가상 별칭으로 구성합니다.

                //TODO: 실제 디렉터리를 구성할 디렉터리로 바꿉니다.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO: 인증을 추가하려면 여기에 추가합니다.

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. 인증 체계 설정
 
AspxVirtualRoot 개체를 구성한 후 AspxVirtualRoot 개체에서 AuthenticationScheme 필드를 설정하여 필요한 인증 체계를 설정합니다.

 
4. SSL 사용
 

SSL을 사용하려면 컴퓨터 저장소에 설치되어 있는 서버 인증서가 SSL을 사용할 포트에 구성되어 있어야 합니다. Httpcfg.exe 유틸리티를 사용하여 포트에서 서버 인증서를 구성하는 방법에 대한 자세한 내용은 Httpcfg 링크를 참조하십시오.

 
참고: Winhttpcertcfg도 포트에서 서버 인증서를 구성하는 데 사용할 수 있습니다.


알려진 문제
====================== 

문제:
응용 프로그램을 시작할 때 다음과 같은 오류 메시지가 표시됩니다.

"System.IO.FileNotFoundException: 파일이나 어셈블리 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' 또는 여기에 종속되어 있는 파일이나 어셈블리 중 하나를 로드할 수 없습니다. 지정한 파일을 찾을 수 없습니다. 파일 이름: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'"

해결 방법:
구성할 실제 디렉터리의 bin 디렉터리에 AspxHostCs.exe 파일이 없습니다. AspxHostcs.exe 파일을 bin 디렉터리에 복사하십시오.


참고 항목
============
.NET Framework SDK 설명서와 MSDN에서 HttpListener 및 Aspx 호스팅 API 설명서를 참조하십시오.