/****************************** Module Header ******************************\
* Module Name:  ProvideReferencePageAttribute.cs
* Project:      CSVSPackageAddReferenceTab
* Copyright (c) Microsoft Corporation.
* 
* Visual Studio supports to extend the Add Reference dialog and add custom
* tab page into the dialog.
*
* This sample demostrate you how to add a custom .NET user control as a tab 
* page into the add reference dialog, and how to enable select button and
*  handle item selection events.
*
* All the sample code is based on MPF.
*
* The sample is initiated by the thread on the forum:
* http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/ddb0f935-b8ac-400d-9e3d-64d74be85031
* 
* History:
* * 1/12/2010 1:00 AM Hongye Sun Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace Microsoft.CSVSPackageAddReferenceTab
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple=true, Inherited=true)]
    public class ProvideReferencePageAttribute : RegistrationAttribute
    {

        private Guid pageGuid;
        private Guid packageGuid;
        private string pageRegKey;

        public ProvideReferencePageAttribute(
            Type pageType,
            Type packageType,
            string name)
        {
            this.pageGuid = pageType.GUID;
            this.packageGuid = packageType.GUID;
            this.pageRegKey = @"ComponentPickerPages\" + name; ;
        }

        public override void Register(RegistrationAttribute.RegistrationContext context)
        {
            using (Key pageKey = context.CreateKey(pageRegKey))
            {
                pageKey.SetValue(string.Empty, string.Empty);
                pageKey.SetValue("AddToMru", 1);
                pageKey.SetValue("ComponentType", ".NET Assembly");
                pageKey.SetValue("Package", packageGuid.ToString("B"));
                pageKey.SetValue("Page", pageGuid.ToString("B"));
                pageKey.SetValue("Sort", 0x35);
            }
        }

        public override void Unregister(RegistrationAttribute.RegistrationContext context)
        {
            context.RemoveKey(pageRegKey);
        }
    }
}
