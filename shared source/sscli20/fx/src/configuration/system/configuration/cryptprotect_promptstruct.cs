//------------------------------------------------------------------------------
// <copyright file="CRYPTPROTECT_PROMPTSTRUCT.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Configuration
{
    using System.Collections.Specialized;
    using System.Runtime.Serialization;
    using System.Configuration.Provider;
    using System.Xml;
    using System.Text;
    using  System.Runtime.InteropServices;
    using Microsoft.Win32;

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    internal struct CRYPTPROTECT_PROMPTSTRUCT : IDisposable
    {
        public int cbSize;
        public int dwPromptFlags;
        public IntPtr hwndApp;
        public string szPrompt;
        void IDisposable.Dispose()
        {
            hwndApp = IntPtr.Zero;
        }
    }
}
