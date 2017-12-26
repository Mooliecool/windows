'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
' SysTime.vb

Imports System
Imports System.Runtime.InteropServices

'typedef struct _SYSTEMTIME { 
'    WORD wYear; 
'    WORD wMonth; 
'    WORD wDayOfWeek; 
'    WORD wDay; 
'    WORD wHour; 
'    WORD wMinute; 
'    WORD wSecond; 
'    WORD wMilliseconds; 
'} SYSTEMTIME, *PSYSTEMTIME; 

< StructLayout(LayoutKind.Sequential )> _
Public Class SystemTime

   Public year As Short
   Public month As Short 
   Public dayOfWeek As Short 
   Public day As Short 
   Public hour As Short 
   Public minute As Short 
   Public second As Short 
   Public milliseconds As Short 
   
End Class 'SystemTime


Public Class LibWrap
   
	'VOID GetSystemTime(LPSYSTEMTIME lpSystemTime)
	
	Declare Sub GetSystemTime Lib "Kernel32.dll" (<[In], Out> ByVal st As SystemTime )
	
End Class 'LibWrap

Public Class App
	Public Shared Sub Main()
	
		Dim st As New SystemTime()
		
		LibWrap.GetSystemTime( st )
		
		Console.Write( "The Date and Time is: " )
		Console.Write( "{0:00}/{1:00}/{2} at ", st.month, st.day, st.year )
		Console.WriteLine( "{0:00}:{1:00}:{2:00}", st.hour, st.minute, st.second )
	
	End Sub 'Main
End Class 'App
