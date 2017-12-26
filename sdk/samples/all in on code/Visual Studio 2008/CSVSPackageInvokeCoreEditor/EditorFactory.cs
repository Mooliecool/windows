/************************************* Module Header **************************************\
* Module Name:  EditorFactory.cs
* Project:      CSVSPackageInvokeCoreEditorPackage
* Copyright (c) Microsoft Corporation.
* 
* The Visual Studio core editor is the default editor of Visual Studio. 
* The editor supports text-editing functions such as insert, delete, 
* copy, and paste. Its functionality combines with that provided by the 
* language that it is currently editing, such as text colorization, 
* indentation, and IntelliSense statement completion.
*
* This sample demostrates the basic operations on Core Editor, which
* includes:
* 1. Initiate core editor, include IVsTextBuffer and IVsCodeWindow
* 2. Associating core editor with file extension: .aio
* 3. Providing an options page in Tools / Options to let user to choose
* languages (VB, CS and XML) in the core editor.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/19/2009 12:00PM Hongye Sun Created
\******************************************************************************************/

using System.Runtime.InteropServices;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.TextManager.Interop;
using IOleServiceProvider = Microsoft.VisualStudio.OLE.Interop.IServiceProvider;
using System;

namespace AllInOne.CSVSPackageInvokeCoreEditor
{
    /// <summary>
    /// Editor factory which responsible for creating core editor and set language id to
    /// the editor.
    /// </summary>
    [Guid("992F4DF1-E58C-4064-8983-66665F0B1CC6")]
    public class EditorFactory : IVsEditorFactory
    {
        private CSVSPackageInvokeCoreEditorPackage parentPackage;
        private IOleServiceProvider serviceProvider;

        public EditorFactory(Package parentPackage)
        {
            this.parentPackage = parentPackage as CSVSPackageInvokeCoreEditorPackage;
        }

        #region IVsEditorFactory Members

        public int Close()
        {
            return VSConstants.S_OK;
        }

        /// <summary>
        /// Used by the editor factory architecture to create editors that support 
        /// data/view separation.
        /// </summary>
        public int CreateEditorInstance(
            uint grfCreateDoc, 
            string pszMkDocument, 
            string pszPhysicalView, 
            IVsHierarchy pvHier, 
            uint itemid, 
            System.IntPtr punkDocDataExisting, 
            out System.IntPtr ppunkDocView, 
            out System.IntPtr ppunkDocData, 
            out string pbstrEditorCaption, 
            out System.Guid pguidCmdUI, 
            out int pgrfCDW)
        {
            int retval = VSConstants.E_FAIL;

            // Initialize these to empty to start with
            ppunkDocView = IntPtr.Zero;
            ppunkDocData = IntPtr.Zero;
            pbstrEditorCaption = "";
            pguidCmdUI = Guid.Empty;
            pgrfCDW = 0;

            if ((grfCreateDoc & (VSConstants.CEF_OPENFILE |
                  VSConstants.CEF_SILENT)) == 0)
            {
                throw new ArgumentException("Only Open or Silent is valid");
            }
            if (punkDocDataExisting != IntPtr.Zero)
            {
                return VSConstants.VS_E_INCOMPATIBLEDOCDATA;
            }

            // Instantiate a text buffer of type VsTextBuffer.
            // Note: we only need an IUnknown (object) interface for 
            // this invocation.
            Guid clsidTextBuffer = typeof(VsTextBufferClass).GUID;
            Guid iidTextBuffer = VSConstants.IID_IUnknown;
            object pTextBuffer = parentPackage.CreateInstance(
                  ref clsidTextBuffer,
                  ref iidTextBuffer,
                  typeof(object));

            if (pTextBuffer != null)
            {
                // "Site" the text buffer with the service provider we were
                // provided.
                IObjectWithSite textBufferSite = pTextBuffer as IObjectWithSite;
                if (textBufferSite != null)
                {
                    textBufferSite.SetSite(this.serviceProvider);
                }
                VsTextBuffer textBuffer = pTextBuffer as VsTextBuffer;

                // Get the language settings from options page
                Guid guidLangSvc = Guid.Empty;
                LanguageServiceOptionsPage options = parentPackage.GetLanguageServiceOptions();
                switch (options.LanguageServiceSetting)
                {
                    case LanguageService.None:
                        break;
                    case LanguageService.VB:
                        guidLangSvc = GuidList.guidVBLangSvc;
                        break;
                    case LanguageService.CS:
                        guidLangSvc = GuidList.guidCSharpLangSvc;
                        break;
                    case LanguageService.XML:
                        guidLangSvc = GuidList.guidXmlLangSvc;
                        break;
                    default:
                        break;
                }

                // Set language service ID
                int hr = textBuffer.SetLanguageServiceID(ref guidLangSvc);

                IVsUserData usrData = pTextBuffer as IVsUserData;
                //     Gets a GUID value in Microsoft.VisualStudio.TextManager.Interop.IVsUserData
                //     that, when set to false, will stop the core editor for searching for a different
                //     language service.
                Guid guidVSBufferDetectLangSid = Microsoft.VisualStudio.Package.EditorFactory.GuidVSBufferDetectLangSid;
                usrData.SetData(ref guidVSBufferDetectLangSid, false);

                                
                // Instantiate a code window of type IVsCodeWindow.
                Guid clsidCodeWindow = typeof(VsCodeWindowClass).GUID;
                Guid iidCodeWindow = typeof(IVsCodeWindow).GUID;
                IVsCodeWindow pCodeWindow =
                (IVsCodeWindow)this.parentPackage.CreateInstance(
                      ref clsidCodeWindow,
                      ref iidCodeWindow,
                      typeof(IVsCodeWindow));
                if (pCodeWindow != null)
                {
                    // Give the text buffer to the code window.
                    // We are giving up ownership of the text buffer!
                    pCodeWindow.SetBuffer((IVsTextLines)pTextBuffer);

                    // Now tell the caller about all this new stuff 
                    // that has been created.
                    ppunkDocView = Marshal.GetIUnknownForObject(pCodeWindow);
                    ppunkDocData = Marshal.GetIUnknownForObject(pTextBuffer);

                    // Specify the command UI to use so keypresses are 
                    // automatically dealt with.
                    pguidCmdUI = VSConstants.GUID_TextEditorFactory;

                    // This caption is appended to the filename and
                    // lets us know our invocation of the core editor 
                    // is up and running.
                    pbstrEditorCaption = " [CSVSPackageInvokeCoreEditor]";

                    retval = VSConstants.S_OK;
                }
            }
            return retval; 
        }

        public int MapLogicalView(ref System.Guid rguidLogicalView, out string pbstrPhysicalView)
        {
            int retval = VSConstants.E_NOTIMPL;
            pbstrPhysicalView = null;   // We support only one view.
            if (rguidLogicalView.Equals(VSConstants.LOGVIEWID_Designer) ||
            rguidLogicalView.Equals(VSConstants.LOGVIEWID_Primary))
            {
                retval = VSConstants.S_OK;
            }
            return retval;
        }

        public int SetSite(Microsoft.VisualStudio.OLE.Interop.IServiceProvider psp)
        {
            this.serviceProvider = psp;
            return VSConstants.S_OK;
        }

        #endregion
    }
}
