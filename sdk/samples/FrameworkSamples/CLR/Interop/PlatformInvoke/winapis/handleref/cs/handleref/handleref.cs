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
// HandleRef.cs

using System;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

/*
typedef struct _OVERLAPPED { 
    ULONG_PTR  Internal; 
    ULONG_PTR  InternalHigh; 
    DWORD  Offset; 
    DWORD  OffsetHigh; 
    HANDLE hEvent; 
} OVERLAPPED; 
*/

// declared as structure
[ StructLayout( LayoutKind.Sequential )]
public struct Overlapped
{
	private IntPtr intrnal;
	private IntPtr internalHigh;
	public int offset;
	public int offsetHigh;
	private IntPtr hEvent;
}

// declared as class
[ StructLayout( LayoutKind.Sequential )]
public class Overlapped2
{
	//private IntPtr intrnal;
	//private IntPtr internalHigh;
	public int offset;
	public int offsetHigh;
	//private IntPtr hEvent;
}

public class LibWrap
{
    // to prevent FileStream to be GC-ed before call ends, 
    // its handle should be wrapped in HandleRef
    
	// since Overlapped is struct, null can't be passed instead, 
	// we must declare overload method if we will use this     
    
    //BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
    //				LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);    
    
	[ DllImport( "Kernel32.dll" )]
	public static extern bool ReadFile(
		SafeHandle hndRef,
		StringBuilder buffer, 
		int numberOfBytesToRead, 
		out int numberOfBytesRead, 
		ref Overlapped flag );
	    
	[ DllImport( "Kernel32.dll" )]
	public static extern bool ReadFile(
		SafeHandle hndRef, 
		StringBuilder buffer, 
		int numberOfBytesToRead, 
		out int numberOfBytesRead, 
		int flag );	// int instead of structure reference
			
	// since Overlapped2 is class, we can pass null as parameter,
	// no overload is needed	
										
	[ DllImport( "Kernel32.dll", EntryPoint="ReadFile" )]
	public static extern bool ReadFile2(
		SafeHandle hndRef,
		StringBuilder buffer, 
		int numberOfBytesToRead, 
		out int numberOfBytesRead,
		Overlapped2 flag );
}

public class App
{
	public static void Main()
	{
        FileStream fs = new FileStream( "HandleRef.txt", FileMode.Open );
		SafeHandle sh = fs.SafeFileHandle;
		StringBuilder buffer = new StringBuilder( 5 );
        int read = 0;
        
        // platform invoke will hold reference to HandleRef until call ends
        
        LibWrap.ReadFile( sh, buffer, 5, out read, 0 );
        Console.WriteLine( "Read with struct parameter: {0}", buffer );

		LibWrap.ReadFile2(sh, buffer, 5, out read, null);
		Console.WriteLine( "Read with class parameter: {0}", buffer );
	}
}


