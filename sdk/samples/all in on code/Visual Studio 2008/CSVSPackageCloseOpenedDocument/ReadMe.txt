=============================================================================
  Visual Studio VSPackage : CSVSPackageCloseOpenedDocument Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to close the document which is already open in
another editor.

In Visual Studio, a specific editor cannot open the document which is already
open by another editor.  For example, if a .cs file is already open with
Code Editor, trying to open that .cs file with XML Editor will cause a dialog
comes out saying "The document 'xxx.cs' is already open.  Do you want to
close it?"
Another example is, in VC++ project, the Resource View toolwindow will always
trying to open the .rc file in a invisible editor(IVsInvisibleEditor) and
track its change to update the information in its tree view.  So if we want
to open the .rc file with Code Editor(right-click -> View Code) for seeing
its source code, the same dialog will appear.

In this sample, it shows how to use the IVsRunningDocumentTable interface to
access the RDT(Running Document Table), find out the opened document and
lock it, then trying to close it, so that other editors can open the same
document without the dialog prompt showing.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-
867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

NOTE: The Package Load Failure Dialog occurs because there is no
      PLK(Package Load Key) Specified in this package. To obtain a PLK, please
      to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info:
      http://msdn.microsoft.com/en-us/library/bb165395.aspx


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Visual Studio Integration Package project from the New
Project dialog named CSVSPackageCloseOpenedDocument, choose Visual C# as
the development language.
Check the Menu Command checkbox to create a menu command.

Step2. Open the CSVSPackageCloseOpenedDocumentPackage.cs file, define a
method named CloseAndOpenInXMLEditor which will trying to open a file with
XML Editor, if the document is already open in another editor, close it from
the RDT.

private void CloseAndOpenInXMLEditor(string filePath)
{
    IVsHierarchy ppHier = null;
    uint pitemid = Microsoft.VisualStudio.VSConstants.VSITEMID_NIL;
    IntPtr ppunkDocData = IntPtr.Zero;
    uint pdwCookie = Microsoft.VisualStudio.VSConstants.VSITEMID_NIL;

    try
    {
        // Get the IVsRunningDocumentTable interface and cast it to
        // IVsRunningDocumentTable2 interface.
        IVsRunningDocumentTable rdt =
            GetService(typeof(SVsRunningDocumentTable))
            as IVsRunningDocumentTable;
        IVsRunningDocumentTable2 rdt2 = rdt as IVsRunningDocumentTable2;

        // Find the opened document(.rc file) from the RDT.
        rdt.FindAndLockDocument((uint)_VSRDTFLAGS.RDT_NoLock, filePath,
            out ppHier, out pitemid, out ppunkDocData, out pdwCookie);
        if (ppunkDocData != IntPtr.Zero)
        {
            // Close the opened document.
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
                rdt2.CloseDocuments((uint)__FRAMECLOSE.FRAMECLOSE_SaveIfDirty,
                null, pdwCookie));
            ppunkDocData = IntPtr.Zero;
        }

        IVsInvisibleEditorManager spIEM;
        IVsInvisibleEditor invisibleEditor = null;
        IVsWindowFrame winFrame = null;
        Guid logicalView = Microsoft.VisualStudio.VSConstants.LOGVIEWID_Primary;

        // Get the IVsInvisibleEditorManager interface.
        spIEM = (IVsInvisibleEditorManager)
            GetService(typeof(IVsInvisibleEditorManager));
        // Register a invisible editor, then the specific document will be
        // loaded into the RDT.
        spIEM.RegisterInvisibleEditor(filePath, null,
            (uint)_EDITORREGFLAGS.RIEF_ENABLECACHING,
            null, out invisibleEditor);

        // Get the IVsUIShellOpenDocument interface.
        IVsUIShellOpenDocument uiShellOpenDocument =
            GetService(typeof(SVsUIShellOpenDocument))
            as IVsUIShellOpenDocument;
        // Guid of the Microsoft XML editor
        Guid guidXMLEditor = new Guid("{FA3CD31E-987B-443A-9B81-186104E8DAC1}");
        rdt.FindAndLockDocument((uint)_VSRDTFLAGS.RDT_NoLock, filePath,
                    out ppHier, out pitemid, out ppunkDocData, out pdwCookie);
        // Open the document in XML editor.
        Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
            uiShellOpenDocument.OpenSpecificEditor((uint)0, filePath,
            ref guidXMLEditor, "", ref logicalView, "XML Editor",
            ppHier as IVsUIHierarchy, pitemid,
            ppunkDocData, this, out winFrame));
        // Show the editor window.
        winFrame.Show();
    }
    catch (Exception e)
    {
        System.Windows.Forms.MessageBox.Show(e.Message);
    }
}

Step3. Call the method CloseAndOpenInXMLEditor in the menu command handler,
replace the default message box.

private void MenuItemCallback(object sender, EventArgs e)
{
    CloseAndOpenInXMLEditor(@"E:\Projects\TestProject\TestProject\TestProject.cs");
}

To test the method CloseAndOpenInXMLEditor, you will need to replace the
passed file to the specific one you want to open in XML Editor.

Step4. Save and compile the project.

Step5. Run this VSPackage in a new instance of Visual Studio(simply press F5),
open a existing project in the new instance of Visual Studio, open a project
file with the Code Editor(e.g.: double-click a .cs file in Solution Explorer).
Then run our method from Tools -> CSVSPackageCloseOpenedDocument, you will
see the opened document being closed then opened again in XML Editor without
the "The document 'xxx.cs' is already open.  Do you want to close it?" dialog
being shown.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: IVsRunningDocumentTable Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop
.ivsrunningdocumenttable(VS.80).aspx

MSDN: IVsRunningDocumentTable2 Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop
.ivsrunningdocumenttable2(VS.80).aspx

MSDN: IVsUIShellOpenDocument.OpenSpecificEditor Method
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop
.ivsuishellopendocument.openspecificeditor.aspx


/////////////////////////////////////////////////////////////////////////////
