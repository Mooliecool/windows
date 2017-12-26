'---------------------------------------------------------------------
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
'---------------------------------------------------------------------

Imports System
Imports System.Runtime.InteropServices
Namespace Microsoft.Samples
    Friend NotInheritable Class NativeMethods
        Private Sub New()
        End Sub

        ' this import is necessary, to allow us to check if someone can login ...
        <DllImport("advapi32.dll", SetLastError:=True, CharSet:=CharSet.Unicode)> _
        Shared Function LogonUser(ByVal username As String, _
        ByVal domain As String, ByVal password As IntPtr, _
        ByVal logonType As Integer, ByVal logonProvider As Integer, _
        ByRef token As IntPtr) As Boolean
        End Function
    End Class
End Namespace
