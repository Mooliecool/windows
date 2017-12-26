'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
Imports system
Imports System.Runtime.InteropServices

Class MemoryHogger

    Private pressure As Integer
    Private addedPressure As Boolean
    Private allocated As Boolean
    Private memptr As IntPtr

    Const MEM_RESERVE As Integer = 8192
    Const MEM_RELEASE As Integer = 32768
    Const PAGE_READWRITE As Integer = 4

    ' This constructor allocates a block of unmanaged memory, plus
    ' optionally inform the GC of the memory
    Public Sub New(ByVal size As Integer, ByVal addPressure As Boolean)

        If addPressure Then
            pressure = size
            addPressure = True
            GC.AddMemoryPressure(pressure)
        End If

        memptr = NativeMethods.VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE)
        allocated = (memptr <> IntPtr.Zero)

        If addPressure And Not allocated Then
            GC.RemoveMemoryPressure(pressure)
            addPressure = False
        End If
    End Sub

    ' Finalization code for the MemoryHogger - make sure to reduce
    ' the GC pressure by EXACTLY the amount that was added
    Protected Overrides Sub Finalize()
        If allocated Then
            NativeMethods.VirtualFree(memptr, 0, MEM_RELEASE)
        End If

        If addedPressure Then
            GC.RemoveMemoryPressure(pressure)
        End If
    End Sub

    Public ReadOnly Property Success() As Boolean
        Get
            Return allocated
        End Get
    End Property

End Class