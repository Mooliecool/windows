/************************************* Module Header **************************************\
* Module Name:  LanguageServiceOptionsPage.cs
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

using System.ComponentModel;
using Microsoft.VisualStudio.Shell;
using System.Runtime.InteropServices;

namespace AllInOne.CSVSPackageInvokeCoreEditor
{
    /// <summary>
    /// Language service choices
    /// </summary>
    public enum LanguageService
    { 
        None,
        VB,
        CS,
        XML
    }

    /// <summary>
    /// The options page which contains language service setting
    /// </summary>
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class LanguageServiceOptionsPage : DialogPage
    {
        private LanguageService languageServiceSetting = LanguageService.None;

        [Category("CSVSPackageInvokeCoreEditor Options")]
        [DisplayName("Language Service Setting")]
        [Description("Language service setting to determine which language" +
            " service is used in editor.")]
        public LanguageService LanguageServiceSetting
        {
            get { return languageServiceSetting; }
            set { languageServiceSetting = value; }
        }

    }
}
