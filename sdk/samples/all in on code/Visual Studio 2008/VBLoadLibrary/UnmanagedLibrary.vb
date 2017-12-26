'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBLoadLibrary
' Copyright (c) Microsoft Corporation.
' 
' The source code of UnmanagedLibrary is quoted from Mike Stall's article:
' 
' Type-safe Managed wrappers for kernel32!GetProcAddress
' http://blogs.msdn.com/jmstall/archive/2007/01/06/Typesafe-GetProcAddress.aspx
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Security.Permissions
Imports Microsoft.Win32.SafeHandles
Imports System.Runtime.ConstrainedExecution
Imports System.Runtime.InteropServices

#End Region


Public NotInheritable Class UnmanagedLibrary
    Implements IDisposable

#Region "Safe handles and Native imports"

    ''' <summary>
    ''' See http://msdn.microsoft.com/msdnmag/issues/05/10/Reliability/ 
    ''' for more about safe handles.
    ''' </summary>
    <SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode:=True)> _
    Private NotInheritable Class SafeLibraryHandle
        Inherits SafeHandleZeroOrMinusOneIsInvalid

        Private Sub New()
            MyBase.New(True)
        End Sub

        Protected Overrides Function ReleaseHandle() As Boolean
            Return NativeMethod.FreeLibrary(MyBase.handle)
        End Function

    End Class


    ''' <summary>
    ''' Native methods
    ''' </summary>
    Private Class NativeMethod

        <DllImport("kernel32", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function LoadLibrary(ByVal fileName As String) As SafeLibraryHandle
        End Function

        <ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), DllImport("kernel32", SetLastError:=True)> _
        Public Shared Function FreeLibrary(ByVal hModule As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function

        <DllImport("kernel32")> _
        Public Shared Function GetProcAddress(ByVal hModule As SafeLibraryHandle, ByVal procname As String) As IntPtr
        End Function

    End Class

#End Region


    ''' <summary>
    ''' Constructor to load a dll and be responible for freeing it.
    ''' </summary>
    ''' <param name="fileName">full path name of dll to load</param>
    ''' <exception cref="System.IO.FileNotFoundException">
    ''' If fileName can't be found
    ''' </exception>
    ''' <remarks>
    ''' Throws exceptions on failure. Most common failure would be 
    ''' file-not-found, or that the file is not a loadable image.
    ''' </remarks>
    Public Sub New(ByVal fileName As String)
        Me.m_hLibrary = NativeMethod.LoadLibrary(fileName)
        If Me.m_hLibrary.IsInvalid Then
            Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error)
        End If
    End Sub


    ''' <summary>
    ''' Dynamically lookup a function in the dll via kernel32!GetProcAddress.
    ''' </summary>
    ''' <param name="functionName">
    ''' raw name of the function in the export table.
    ''' </param>
    ''' <returns>
    ''' null if function is not found. Else a delegate to the unmanaged 
    ''' </returns>
    ''' <remarks>
    ''' GetProcAddress results are valid as long as the dll is not yet 
    ''' unloaded. This is very very dangerous to use since you need to 
    ''' ensure that the dll is not unloaded until after you're done with any 
    ''' objects implemented by the dll. For example, if you get a delegate 
    ''' that then gets an IUnknown implemented by this dll, you can not 
    ''' dispose this library until that IUnknown is collected. Else, you may 
    ''' free the library and then the CLR may call release on that IUnknown 
    ''' and it will crash.
    ''' </remarks>
    Public Function GetUnmanagedFunction(Of TDelegate As Class)(ByVal functionName As String) As TDelegate
        Dim p As IntPtr = NativeMethod.GetProcAddress(Me.m_hLibrary, functionName)

        ' Failure is a common case, especially for adaptive code.
        If (p = IntPtr.Zero) Then
            Return CType(Nothing, TDelegate)
        End If

        Dim func As [Delegate] = Marshal.GetDelegateForFunctionPointer(p, GetType(TDelegate))

        ' Ideally, we'd just make the constraint on TDelegate be System.Delegate, 
        ' but compiler error CS0702 (constrained can't be System.Delegate)
        ' prevents that. So we make the constraint system.object and do the
        ' cast from object-->TDelegate.
        Dim o As Object = func

        Return DirectCast(o, TDelegate)
    End Function


#Region "IDisposable Members"

    ''' <summary>
    ''' Call FreeLibrary on the unmanaged dll. All function pointers handed 
    ''' out from this class become invalid after this.
    ''' </summary>
    ''' <remarks>
    ''' This is very dangerous because it suddenly invalidate everything
    ''' retrieved from this dll. This includes any functions handed out via 
    ''' GetProcAddress, and potentially any objects returned from those 
    ''' functions (which may have an implemention in the dll).
    ''' </remarks>
    Public Sub Dispose() Implements IDisposable.Dispose
        If Not Me.m_hLibrary.IsClosed Then
            Me.m_hLibrary.Close()
        End If
    End Sub

    ' Unmanaged resource. CLR will ensure SafeHandles get freed, without 
    ' requiring a finalizer on this class.
    Private m_hLibrary As SafeLibraryHandle

#End Region

End Class