//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
// GCHandle.cs

using System;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;

public delegate bool CallBack( int handle, IntPtr param );

public class LibWrap
{
	// passing managed object as LPARAM
	// BOOL EnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam);
	
	[ DllImport( "user32.dll" )]
	public static extern bool EnumWindows( CallBack cb, IntPtr param );
}

public class App
{
	public static void Main()
	{
		TextWriter tw = System.Console.Out;
		GCHandle gch = GCHandle.Alloc( tw );
		
		CallBack cewp = new CallBack( CaptureEnumWindowsProc );
		
		// platform invoke will prevent delegate to be garbage collected
		// before call ends
		
		LibWrap.EnumWindows( cewp, (IntPtr)gch );
		
		gch.Free();
	}
	
	private static bool CaptureEnumWindowsProc( int handle, IntPtr param )
	{
		GCHandle gch = (GCHandle)param;
		TextWriter tw = (TextWriter)gch.Target;
		tw.WriteLine( handle );
		return true;
	}	
}


