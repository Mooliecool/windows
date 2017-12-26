SchemaImporterExtension 정수 가져오기 샘플
=============================================
     SchemaImporterExtension을 작성하여 XML 스키마 정수 형식을 string 대신 long 및 ulong으로 가져오는 방법을 보여 줍니다.


샘플 언어 구현
===============================
     이 샘플은 다음과 같은 언어 구현에서 사용할 수 있습니다.
     C#


명령 프롬프트를 사용하여 샘플을 빌드하려면
=============================================
     1. 명령 프롬프트 창을 열고 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 디렉터리로 이동합니다.
     2. msbuild [Solution Filename]을 입력합니다.


Visual Studio를 사용하여 샘플을 빌드하려면
=======================================
     1. Windows 탐색기를 열고 Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger 디렉터리로 이동합니다.
     2. .sln(솔루션) 파일의 아이콘을 두 번 클릭하여 Visual Studio에서 해당 파일을 엽니다.
     3. [빌드] 메뉴에서 [솔루션 빌드]를 선택합니다.
     응용 프로그램은 기본적으로 \bin 또는 \bin\Debug 디렉터리에 빌드됩니다.

     이 샘플을 빌드할 때 추가로 고려할 사항은 다음과 같습니다.
     1.  어셈블리에 대한 강력한 이름을 생성합니다.
     2.  어셈블리를 GAC에 추가합니다.
     3.  schemas.xml.serialization/schemaImporterExtension 섹션의 machine.config에 어셈블리를 추가합니다.


샘플을 실행하려면
=================
     1. 명령 프롬프트나 Windows 탐색기를 사용하여 새 실행 파일이 포함된 디렉터리로 이동합니다.
     2. 명령줄에 [ExecutableFile]을 입력하거나 Windows 탐색기에서 [SampleExecutable] 아이콘을 두 번 클릭하여 샘플을 실행합니다.

     제한된 권한을 사용하여 샘플을 빌드할 수는 있지만 어셈블리를 GAC에 추가하고 machine.config를 편집해야 하므로 샘플을 설치하려면 관리자 권한이 필요합니다.

     machine.config의 예제 엔트리:

     system.xml.serialization
     	schemaImporterExtensions
     		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
     	schemaImporterExtensions
     system.xml.serialization


설명
=================
     1.  xsd.exe, wsdl.exe를 실행하거나 xs:integer, xs:negativeInteger, xs:nonNegativeInteger, xs:positiveInteger 또는 xs:nonPositiveInteger를 사용하는 WSDL에 웹 참조를 추가합니다.
2.  생성된 클래스는 XML 스키마 정수 형식에 대해 string 대신 long 또는 ulong을 사용합니다.
