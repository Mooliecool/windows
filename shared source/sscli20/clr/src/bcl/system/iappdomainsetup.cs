// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Interface:  IAppDomainSetup
**
**
** Purpose: Properties exposed to COM
**
** 
===========================================================*/
namespace System {

    using System.Runtime.InteropServices;

    [GuidAttribute("27FFF232-A7A8-40dd-8D4A-734AD59FCD41")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IAppDomainSetup
    {
        String ApplicationBase {
            get;
            set;
        }

        String ApplicationName
        {
            get;
            set;
        }

        String CachePath
        {
            get;
            set;
        }

        String ConfigurationFile {
            get;
            set;
        }

        String DynamicBase
        {
            get;
            set;
        }

        String LicenseFile
        {
            get;
            set;
        }

        String PrivateBinPath
        {
            get;
            set;
        }

        String PrivateBinPathProbe
        {
            get;
            set;
        }

        String ShadowCopyDirectories
        {
            get;
            set;
        }

        String ShadowCopyFiles
        {
            get;
            set;
        }

    }
}
