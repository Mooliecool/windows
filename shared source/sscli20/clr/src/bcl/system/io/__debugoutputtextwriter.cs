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

#if _DEBUG
// This class writes to wherever OutputDebugString writes to.  If you don't have
// a Windows app (ie, something hosted in IE), you can use this to redirect Console
// output for some good old-fashioned console spew in MSDEV's debug output window.


using System;
using System.IO;
using System.Text;
using System.Security;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Globalization;

namespace System.IO {
    internal class __DebugOutputTextWriter : TextWriter {
        private readonly String _consoleType;

        internal __DebugOutputTextWriter(String consoleType): base(CultureInfo.InvariantCulture)
        {
            _consoleType = consoleType;
        }

        public override Encoding Encoding {
            get {
                if (Marshal.SystemDefaultCharSize == 1)
                    return Encoding.Default;
                else
                    return new UnicodeEncoding(false, false);
            }
        }

        public override void Write(char c)
        {
            OutputDebugString(c.ToString());
        }

        public override void Write(String str)
        {
            OutputDebugString(str);
        }

        public override void Write(char[] array)
        {
            if (array != null) 
                OutputDebugString(new String(array));
        }
        
        public override void WriteLine(String str)
        {
            if (str != null)
                OutputDebugString(_consoleType + str);
            else
                OutputDebugString("<null>");
            OutputDebugString(new String(CoreNewLine));
        }

        [DllImport(Win32Native.KERNEL32, CharSet=CharSet.Auto), SuppressUnmanagedCodeSecurityAttribute()]
        private static extern void OutputDebugString(String output);
    }
}
       
#endif // _DEBUG
