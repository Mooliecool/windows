================================================================================
       VSX application : CSVSPackageInvokeCoreEditor Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:
The Visual Studio core editor is the default editor of Visual Studio. 
The editor supports text-editing functions such as insert, delete, 
copy, and paste. Its functionality combines with that provided by the 
language that it is currently editing, such as text colorization, 
indentation, and IntelliSense statement completion.

This sample demostrates the basic operations on Core Editor, which
includes:
1. Initiate core editor, include IVsTextBuffer and IVsCodeWindow
2. Associating core editor with file extension: .aio
3. Providing an options page in Tools / Options to let user to choose
languages (VB, CS and XML) in the core editor.

To use this sample, here is the suggested demo steps:
1. Open Tools / Options / CSVSPackageInvokeCoreEditor Options / 
Language Service Settings / Select one language:
None: represents no language service
VB: Visual Basic
CS: C#
XML: XML language
After selecting language settings, please save the changes and close 
options page

2. File / Open / Go to path: 
[All-In-One Root Folder]/CSVSPackageInvokeCoreEditor/DemoFiles
and select one file with name: demo.[Language].aio

3. After the file is opened by Visual Studio, you will see:
a. The file name is suffixed with " [CSVSPackageInvokeCoreEditor]"
b. The editor has functionality of the selected language service like 
coloring.

//////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the project
Setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

NOTE: The Package Load Failure Dialog occurs because there is no PLK(Package Load Key)
      Specified in this package. To obtain a PLK, please to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info
      http://msdn.microsoft.com/en-us/library/bb165395.aspx

/////////////////////////////////////////////////////////////////////////////

Steps:
In order to implement this sample, following are the core steps:
(For detailed informaiton, please view sample code)

1. Create class EditorFactory which inherits from IVsEditorFactory.

2. Implement IVsEditorFactory.CreateEditorInstance method.

3. Obtain a IVsTextBuffer to hold the document data object being edited.

4. Get LanguageServiceOptionsPage's user setting and SetLanguageServiceID 
with selected language guid.
Language service has following guid:
public const string guidVBLangSvcString = 
	"{E34ACDC0-BAAE-11D0-88BF-00A0C9110049}";
public const string guidCSharpLangSvcString = 
	"{694DD9B6-B865-4C5B-AD85-86356E9C88DC}";
public const string guidXmlLangSvcString = 
	"{f6819a78-a205-47b5-be1c-675b3c7f0b8e}";
More languages' guid can be found in 
"HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\9.0\
Languages\Language Services"

5. Gets a GUID value in Microsoft.VisualStudio.TextManager.Interop.IVsUserData
that, when set to false, will stop the core editor for searching for a different
language service.
This operation prevents situation when one file extension is associated with
multiple languages.

6. Create a line oriented representation of the document data object by 
creating an IVsTextLines interface from the IVsTextBuffer interface. 

7. Set IVsTextLines as the document data object for an instance of the 
default implementation of the IVsCodeWindow interface, using the SetBuffer 
method.

/////////////////////////////////////////////////////////////////////////////
References:

Instantiating the Core Editor
http://msdn.microsoft.com/en-us/library/bb166530.aspx

Language Service and the Core Editor
http://msdn.microsoft.com/en-us/library/bb166541.aspx



