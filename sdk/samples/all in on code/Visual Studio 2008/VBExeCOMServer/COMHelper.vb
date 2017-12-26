'****************************** Module Header ******************************'
' Module Name:  COMHelper.vb
' Project:      VBExeCOMServer
' Copyright (c) Microsoft Corporation.
' 
' COMHelper provides the helper functions to register/unregister COM server
' and encapsulates the native COM APIs to be used in .NET.
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

Imports Microsoft.Win32
Imports System.Runtime.InteropServices
Imports System.Reflection

#End Region


Friend Class COMHelper

    ''' <summary>
    ''' Register the component as a local server.
    ''' </summary>
    ''' <param name="t"></param>
    Public Shared Sub RegasmRegisterLocalServer(ByVal t As Type)
        COMHelper.GuardNullType(t, "t")  ' Check the argument

        ' Open the CLSID key of the component.
        Using keyCLSID As RegistryKey = Registry.ClassesRoot.OpenSubKey( _
        "CLSID\" & t.GUID.ToString("B"), True)

            ' Remove the auto-generated InprocServer32 key after registration
            ' (REGASM puts it there but we are going out-of-proc).
            keyCLSID.DeleteSubKeyTree("InprocServer32")

            ' Create "LocalServer32" under the CLSID key
            Using subkey As RegistryKey = keyCLSID.CreateSubKey("LocalServer32")
                subkey.SetValue("", Assembly.GetExecutingAssembly.Location, _
                                RegistryValueKind.String)
            End Using
        End Using
    End Sub


    ''' <summary>
    ''' Unregister the component.
    ''' </summary>
    ''' <param name="t"></param>
    Public Shared Sub RegasmUnregisterLocalServer(ByVal t As Type)
        COMHelper.GuardNullType(t, "t")  ' Check the argument

        ' Delete the CLSID key of the component
        Registry.ClassesRoot.DeleteSubKeyTree(("CLSID\" & t.GUID.ToString("B")))
    End Sub


    Private Shared Sub GuardNullType(ByVal t As Type, ByVal param As String)
        If (t Is Nothing) Then
            Throw New ArgumentException("The CLR type must be specified.", param)
        End If
    End Sub

End Class


Friend Class COMNative

    ''' <summary>
    ''' CoInitializeEx() can be used to set the apartment model of individual 
    ''' threads.
    ''' </summary>
    ''' <param name="pvReserved">Must be NULL</param>
    ''' <param name="dwCoInit">
    ''' The concurrency model and initialization options for the thread
    ''' </param>
    ''' <returns></returns>
    <DllImport("ole32.dll")> _
    Public Shared Function CoInitializeEx(ByVal pvReserved As IntPtr, ByVal dwCoInit As UInt32) As Integer
    End Function


    ''' <summary>
    ''' CoUninitialize() is used to uninitialize a COM thread.
    ''' </summary>
    ''' <remarks></remarks>
    <DllImport("ole32.dll")> _
    Public Shared Sub CoUninitialize()
    End Sub


    ''' <summary>
    ''' Registers an EXE class object with OLE so other applications can 
    ''' connect to it. EXE object applications should call 
    ''' CoRegisterClassObject on startup. It can also be used to register 
    ''' internal objects for use by the same EXE or other code (such as DLLs)
    ''' that the EXE uses.
    ''' </summary>
    ''' <param name="rclsid">CLSID to be registered</param>
    ''' <param name="pUnk">
    ''' Pointer to the IUnknown interface on the class object whose 
    ''' availability is being published.
    ''' </param>
    ''' <param name="dwClsContext">
    ''' Context in which the executable code is to be run.
    ''' </param>
    ''' <param name="flags">
    ''' How connections are made to the class object.
    ''' </param>
    ''' <param name="lpdwRegister">
    ''' Pointer to a value that identifies the class object registered; 
    ''' </param>
    ''' <returns></returns>
    ''' <remarks>
    ''' PInvoking CoRegisterClassObject to register COM objects is not 
    ''' supported.
    ''' </remarks>
    <DllImport("ole32.dll")> _
    Public Shared Function CoRegisterClassObject( _
    ByRef rclsid As Guid, <MarshalAs(UnmanagedType.Interface)> ByVal pUnk As IClassFactory, _
    ByVal dwClsContext As CLSCTX, ByVal flags As REGCLS, <Out()> ByRef lpdwRegister As UInt32) _
    As Integer
    End Function


    ''' <summary>
    ''' Informs OLE that a class object, previously registered with the 
    ''' CoRegisterClassObject function, is no longer available for use.
    ''' </summary>
    ''' <param name="dwRegister">
    ''' Token previously returned from the CoRegisterClassObject function
    ''' </param>
    ''' <returns></returns>
    <DllImport("ole32.dll")> _
    Public Shared Function CoRevokeClassObject(ByVal dwRegister As UInt32) As UInt32
    End Function


    ''' <summary>
    ''' Called by a server that can register multiple class objects to inform 
    ''' the SCM about all registered classes, and permits activation requests 
    ''' for those class objects.
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks>
    ''' Servers that can register multiple class objects call 
    ''' CoResumeClassObjects once, after having first called 
    ''' CoRegisterClassObject, specifying REGCLS_LOCAL_SERVER | 
    ''' REGCLS_SUSPENDED for each CLSID the server supports. This function 
    ''' causes OLE to inform the SCM about all the registered classes, and 
    ''' begins letting activation requests into the server process.
    ''' 
    ''' This reduces the overall registration time, and thus the server 
    ''' application startup time, by making a single call to the SCM, no 
    ''' matter how many CLSIDs are registered for the server. Another 
    ''' advantage is that if the server has multiple apartments with 
    ''' different CLSIDs registered in different apartments, or is a free-
    ''' threaded server, no activation requests will come in until the server 
    ''' calls CoResumeClassObjects. This gives the server a chance to 
    ''' register all of its CLSIDs and get properly set up before having to 
    ''' deal with activation requests, and possibly shutdown requests. 
    ''' </remarks>
    <DllImport("ole32.dll")> _
    Public Shared Function CoResumeClassObjects() As Integer
    End Function


    ''' <summary>
    ''' Interface Id of IClassFactory
    ''' </summary>
    ''' <remarks></remarks>
    Public Const IID_IClassFactory As String = "00000001-0000-0000-C000-000000000046"

    ''' <summary>
    ''' Interface Id of IDispatch
    ''' </summary>
    ''' <remarks></remarks>
    Public Const IID_IDispatch As String = "00020400-0000-0000-C000-000000000046"

    ''' <summary>
    ''' Interface Id of IUnknown
    ''' </summary>
    ''' <remarks></remarks>
    Public Const IID_IUnknown As String = "00000000-0000-0000-C000-000000000046"

    ''' <summary>
    ''' Class does not support aggregation (or class object is remote)
    ''' </summary>
    ''' <remarks></remarks>
    Public Const CLASS_E_NOAGGREGATION As Integer = &H80040110

    ''' <summary>
    ''' No such interface supported
    ''' </summary>
    ''' <remarks></remarks>
    Public Const E_NOINTERFACE As Integer = &H80004002

End Class


''' <summary>
''' You must implement this interface for every class that you register in 
''' the system registry and to which you assign a CLSID, so objects of that
''' class can be created.
''' http://msdn.microsoft.com/en-us/library/ms694364.aspx
''' </summary>
<ComImport(), ComVisible(False), _
InterfaceType(ComInterfaceType.InterfaceIsIUnknown), _
Guid("00000001-0000-0000-C000-000000000046")> _
Friend Interface IClassFactory

    ''' <summary>
    ''' Creates an uninitialized object.
    ''' </summary>
    ''' <param name="pUnkOuter"></param>
    ''' <param name="riid">
    ''' Reference to the identifier of the interface to be used to 
    ''' communicate with the newly created object. If pUnkOuter is NULL, this
    ''' parameter is frequently the IID of the initializing interface.
    ''' </param>
    ''' <param name="ppvObject">
    ''' Address of pointer variable that receives the interface pointer 
    ''' requested in riid. 
    ''' </param>
    ''' <returns>S_OK means success.</returns>
    <PreserveSig()> _
    Function CreateInstance(ByVal pUnkOuter As IntPtr, ByRef riid As Guid, _
                            <Out()> ByRef ppvObject As IntPtr) As Integer


    ''' <summary>
    ''' Locks object application open in memory.
    ''' </summary>
    ''' <param name="fLock">
    ''' If TRUE, increments the lock count; 
    ''' if FALSE, decrements the lock count.
    ''' </param>
    ''' <returns>S_OK means success.</returns>
    <PreserveSig()> _
    Function LockServer(ByVal fLock As Boolean) As Integer

End Interface


''' <summary>
''' Values from the CLSCTX enumeration are used in activation calls to 
''' indicate the execution contexts in which an object is to be run. These
''' values are also used in calls to CoRegisterClassObject to indicate the
''' set of execution contexts in which a class object is to be made available
''' for requests to construct instances.
''' </summary>
<Flags()> _
Friend Enum CLSCTX As UInt32
    INPROC_SERVER = &H1
    INPROC_HANDLER = &H2
    LOCAL_SERVER = &H4
    INPROC_SERVER16 = &H8
    REMOTE_SERVER = &H10
    INPROC_HANDLER16 = &H20
    RESERVED1 = &H40
    RESERVED2 = &H80
    RESERVED3 = &H100
    RESERVED4 = &H200
    NO_CODE_DOWNLOAD = &H400
    RESERVED5 = &H800
    NO_CUSTOM_MARSHAL = &H1000
    ENABLE_CODE_DOWNLOAD = &H2000
    NO_FAILURE_LOG = &H4000
    DISABLE_AAA = &H8000
    ENABLE_AAA = &H10000
    FROM_DEFAULT_CONTEXT = &H20000
    ACTIVATE_32_BIT_SERVER = &H40000
    ACTIVATE_64_BIT_SERVER = &H80000
End Enum


''' <summary>
''' The REGCLS enumeration defines values used in CoRegisterClassObject to 
''' control the type of connections to a class object.
''' </summary>
''' <remarks></remarks>
<Flags()> _
Friend Enum REGCLS As UInt32
    SINGLEUSE = 0
    MULTIPLEUSE = 1
    MULTI_SEPARATE = 2
    SUSPENDED = 4
    SURROGATE = 8
End Enum