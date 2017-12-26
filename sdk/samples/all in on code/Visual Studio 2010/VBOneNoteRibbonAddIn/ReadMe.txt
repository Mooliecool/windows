=============================================================================
          APPLICATION : VBOneNoteRibbonAddIn Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

The code sample demonstrates a OneNote 2010 COM add-in that implements 
IDTExtensibility2. 
The add-in also supports customizing the Ribbon by implementing the 
IRibbonExtensibility interface.
In addition, the sample also demonstrates the usage of the 
OneNote 2010 Object Model.

VBOneNoteRibbonAddIn: The project which generates VBOneNoteRibbonAddIn.dll 
for project VBOneNoteRibbonAddInSetup.

VBOneNoteRibbonAddInSetup: The setup project which generates setup.exe and 
VBOneNoteRibbonAddInSetup.msi for OneNote 2010.

/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this code sample on a computer that has Microsoft OneNote 2010 
installed.

/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the VBOneNoteRibbonAddIn
sample.

Step1. Open the solution file CSOneNoteRibbonAddIn.sln as Administrator;

Step2. Build VBOneNoteRibbonAddIn first, and then build setup project 
VBOneNoteRibbonAddInSetup in Visual Studio 2010, then you will get a 
bootstrapper setup.exe and the application VBOneNoteRibbonAddInSetup.msi;

Step3. Install setup.exe;

Step4. Open OneNote 2010 and you will see three MesseageBoxs:
MessageBox.Show("VBOneNoteRibbonAddIn OnConnection")
MessageBox.Show("VBOneNoteRibbonAddIn OnAddInsUpdate")
MessageBox.Show("VBOneNoteRibbonAddIn OnStartupComplete");

Step5. Click Review Tab and you will see Statistics group which contains a
button ShowForm which the add-in added to the Ribbon. Click the ShowForm 
button, a windows form will pop up and you can click the button on the form 
to get the title of the current page;

Step6. When closing OneNote 2010, you will see two MessageBoxs:
MessageBox.Show("VBOneNoteRibbonAddIn OnBeginShutdown")
MessageBox.Show("VBOneNoteRibbonAddIn OnDisconnection")

/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Shared Add-in Extensibility,and the shared Add-in Wizard is 
as follows:
	Open the Visual Studio 2010 as Administrator;
	Create an Shared Add-in (Other Project Types->Extensibility) 
	using Visual Basic; 
	choose Microsoft Access	(since there doesn't exist Microsoft OneNote
	option to choose, you can choose Microsoft Access first, but remeber 
	to modify Setup project registry HKCU to be OneNote);
	fill name and description of the Add-in;
	select the two checkboxes in Choose Add-in Options.

Step2. Modify the VBOneNoteRibbonAddInSetup Registry 
(right click Project->View->Registry) 
[HKEY_CLASSES_ROOT\AppID\{Your GUID}]
"DllSurrogate"=""
[HKEY_CLASSES_ROOT\CLSID\{Your GUID}]
"AppID"="{Your GUID}"

[HKEY_CURRENT_USER\Software\Microsoft\Office\OneNote\AddIns\
VBOneNoteRibbonAddIn.Connect]
"LoadBehavior"=dword:00000003
"FriendlyName"="OneNoteRibbonAddInSample"
"Description"="OneNote2010 Ribbon AddIn Sample"

[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\AppID\{Your GUID}]
"DllSurrogate"=""
[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{Your GUID}]
"AppID"="{Your GUID}"

Step3. Add customUI.xml and showform.png resource files into 
VBOneNoteRibbonAddIn project.

Step4. Make Connect class inherent IRibbonExtensibility and implement the method 
GetCustomUI.

    ''' <summary>
    '''     Loads the XML markup from an XML customization file 
    '''     that customizes the Ribbon user interface.
    ''' </summary>
    ''' <param name="RibbonID">The ID for the RibbonX UI</param>
    ''' <returns>string</returns>
    Public Function GetCustomUI(ByVal RibbonID As String) As String _
        Implements IRibbonExtensibility.GetCustomUI

        Return Resources.customUI
    End Function
Step5. Implement the methods OnGetImage and ShowForm according to the customUI.xml 
content.


    ''' <summary>
    '''     Implements the OnGetImage method in customUI.xml
    ''' </summary>
    ''' <param name="imageName">the image name in customUI.xml</param>
    ''' <returns>memory stream contains image</returns>
    Public Function OnGetImage(ByVal imageName As String) As IStream
        Dim stream As New MemoryStream()

        If imageName = "showform.png" Then
            Resources.showform.Save(stream, ImageFormat.Png)
        End If

        Return New ReadOnlyIStreamWrapper(stream)
    End Function
    ''' <summary>
    '''     show Windows Form method
    ''' </summary>
    ''' <param name="control">Represents the object passed into every
    ''' Ribbon user interface (UI) control's callback procedure.</param>
    Public Sub ShowForm(ByVal control As IRibbonControl)
        Dim context As OneNote.Window = TryCast(control.Context, OneNote.Window)
        Dim owner As New CWin32WindowWrapper(CType(context.WindowHandle, IntPtr))
        Dim form As New TestForm(TryCast(applicationObject, OneNote.Application))
        form.ShowDialog(owner)

        form.Dispose()
        form = Nothing
        context = Nothing
        owner = Nothing
        GC.Collect()
        GC.WaitForPendingFinalizers()
        GC.Collect()
    End Sub

Step6. Add ReadOnlyIStreamWrapper class and CWin32WindowWrapper class into 
VBOneNoteRibbonAddIn project and add Windows Form for testing to open.

Step7. Add the follwing methods in the TestForm which using OneNote 2010 Object Model
to show the title of the current page:

    ''' <summary>
    ''' Get the title of the page
    ''' </summary>
    ''' <returns>string</returns>
    Private Function GetPageTitle() As String
        Dim pageXmlOut As String = GetActivePageContent()
        Dim doc = XDocument.Parse(pageXmlOut)
        Dim pageTitle As String = ""
        pageTitle = doc.Descendants().FirstOrDefault().Attribute("ID").NextAttribute.Value

        Return pageTitle
    End Function

    ''' <summary>
    ''' Get active page content and output the xml string
    ''' </summary>
    ''' <returns>string</returns>
    Private Function GetActivePageContent() As String
        Dim activeObjectID As String = Me.GetActiveObjectID(_ObjectType.Page)
        Dim pageXmlOut As String = ""
        oneNoteApp.GetPageContent(activeObjectID, pageXmlOut)

        Return pageXmlOut
    End Function

    ''' <summary>
    ''' Get ID of current page 
    ''' </summary>
    ''' <param name="obj">_Object Type</param>
    ''' <returns>current page Id</returns>
    Private Function GetActiveObjectID(ByVal obj As _ObjectType) As String
        Dim currentPageId As String = ""
        Dim count As UInteger = oneNoteApp.Windows.Count
        For Each window As OneNote.Window In oneNoteApp.Windows
            If window.Active Then
                Select Case obj
                    Case _ObjectType.Notebook
                        currentPageId = window.CurrentNotebookId
                        Exit Select
                    Case _ObjectType.Section
                        currentPageId = window.CurrentSectionId
                        Exit Select
                    Case _ObjectType.SectionGroup
                        currentPageId = window.CurrentSectionGroupId
                        Exit Select
                End Select

                currentPageId = window.CurrentPageId
            End If
        Next

        Return currentPageId

    End Function

    ''' <summary>
    ''' Nested types
    ''' </summary>
    Private Enum _ObjectType
        Notebook
        Section
        SectionGroup
        Page
        SelectedPages
        PageObject
    End Enum

Step8. Register the output assembly as COM component.

To do this, click Project->Project Properties... button. And in the project
properties page, navigate to Build tab and check the box "Register for COM
interop".

Step8. Build your VBOneNoteRibbonAddIn project, 
and then build VBOneNoteRibbonAddInSetup project to generate setup.exe and 
VBOneNoteRibbonAddInSetup.msi.



/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating OneNote 2010 Extensions with the OneNote Object Model

http://msdn.microsoft.com/en-us/magazine/ff796230.aspx

Jeff Cardon's Blog
http://blogs.msdn.com/b/onenotetips/


/////////////////////////////////////////////////////////////////////////////

