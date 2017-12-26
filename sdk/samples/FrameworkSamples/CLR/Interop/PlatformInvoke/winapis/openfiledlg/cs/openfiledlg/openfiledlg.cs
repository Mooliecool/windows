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
// OpenFileDlg.cs

using System;
using System.Text;
using System.Runtime.InteropServices;

/*
typedef struct tagOFN { 
  DWORD         lStructSize; 
  HWND          hwndOwner; 
  HINSTANCE     hInstance; 
  LPCTSTR       lpstrFilter; 
  LPTSTR        lpstrCustomFilter; 
  DWORD         nMaxCustFilter; 
  DWORD         nFilterIndex; 
  LPTSTR        lpstrFile; 
  DWORD         nMaxFile; 
  LPTSTR        lpstrFileTitle; 
  DWORD         nMaxFileTitle; 
  LPCTSTR       lpstrInitialDir; 
  LPCTSTR       lpstrTitle; 
  DWORD         Flags; 
  WORD          nFileOffset; 
  WORD          nFileExtension; 
  LPCTSTR       lpstrDefExt; 
  LPARAM        lCustData; 
  LPOFNHOOKPROC lpfnHook; 
  LPCTSTR       lpTemplateName; 
#if (_WIN32_WINNT >= 0x0500)
  void *        pvReserved;
  DWORD         dwReserved;
  DWORD         FlagsEx;
#endif // (_WIN32_WINNT >= 0x0500)
} OPENFILENAME, *LPOPENFILENAME; 
*/

[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Auto )]  
public class OpenFileName 
{
    public int		structSize = 0;
	private IntPtr dlgOwner = IntPtr.Zero;
	private IntPtr instance = IntPtr.Zero;
    
    public String	filter = null;
    public String	customFilter = null;
    public int		maxCustFilter = 0;
    public int		filterIndex = 0;
    
    public String	file = null;
    public int		maxFile = 0;
    
    public String	fileTitle = null;
	public int		maxFileTitle = 0;
	
    public String	initialDir = null;
    
    public String	title = null;   
    
    public int		flags = 0; 
    public short	fileOffset = 0;
    public short	fileExtension = 0;
    
    public String	defExt = null; 
    
    private IntPtr	custData = IntPtr.Zero;
	private IntPtr hook = IntPtr.Zero;  
    
    public String	templateName = null;

	private IntPtr reservedPtr = IntPtr.Zero; 
    public int		reservedInt = 0;
    public int		flagsEx = 0;
}

public class LibWrap
{
	//BOOL GetOpenFileName(LPOPENFILENAME lpofn);
	
	[ DllImport( "Comdlg32.dll", CharSet=CharSet.Auto )]				
	public static extern bool GetOpenFileName([ In, Out ] OpenFileName ofn );	
}

public class App
{
	public static void Main()
	{
		OpenFileName ofn = new OpenFileName();
		
		ofn.structSize = Marshal.SizeOf( ofn );
		
		ofn.filter = "Log files\0*.log\0Batch files\0*.bat\0";
		
		ofn.file = new String( new char[ 256 ]);
		ofn.maxFile = ofn.file.Length;
		
		ofn.fileTitle = new String( new char[ 64 ]);
		ofn.maxFileTitle = ofn.fileTitle.Length;	
			
		ofn.initialDir = "C:\\";
		ofn.title = "Open file called using platform invoke...";
		ofn.defExt = "txt";
		
		if( LibWrap.GetOpenFileName( ofn ))
		{
			Console.WriteLine( "Selected file with full path: {0}", ofn.file );
			Console.WriteLine( "Selected file name: {0}", ofn.fileTitle );
			Console.WriteLine( "Offset from file name: {0}", ofn.fileOffset );
			Console.WriteLine( "Offset from file extension: {0}", ofn.fileExtension );
		}
	}
}


