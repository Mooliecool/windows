//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Runtime.InteropServices;

namespace Microsoft.Samples
{
	internal sealed class NativeMethods
	{
		private NativeMethods() { }

		// this import is necessary, to allow us to check if someone can login ...
		[DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool LogonUser(String username,
				String domain, IntPtr password,
				int logonType, int logonProvider, ref IntPtr token);
	}
}
