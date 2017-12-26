'*************************** Module Header ******************************'
' Module Name:  WinINet.vb
' Project:	    VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' This class is a simple .NET wrapper of wininet.dll. It contains 2 extern
' methods (InternetSetOption and InternetQueryOption) of wininet.dll. This 
' class can be used to set proxy, disable proxy, backup internet options 
' and restore internet options.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices

Public NotInheritable Class WinINet

    Private Shared _agent As String = Process.GetCurrentProcess().ProcessName

    ''' <summary>
    ''' Set the LAN connection proxy server for current process.
    ''' </summary>
    ''' <param name="proxyServer">
    ''' The Proxy Server.
    ''' </param>
    ''' <returns></returns>
    Public Shared Function SetConnectionProxy(ByVal isMachineSetting As Boolean, ByVal proxyServer As String) As Boolean
        If isMachineSetting Then
            Return SetConnectionProxy(String.Empty, proxyServer)
        Else
            Return SetConnectionProxy(_agent, proxyServer)
        End If
    End Function

    ''' <summary>
    ''' Set the LAN connection proxy server.
    ''' </summary>
    ''' <param name="agentName">
    ''' If agentName is null or empty, this function will set the Lan proxy for
    ''' the machine, else for the current process.
    ''' </param>
    ''' <param name="proxyServer">The Proxy Server.</param>
    ''' <returns></returns>
    Public Shared Function SetConnectionProxy(ByVal agentName As String, ByVal proxyServer As String) As Boolean
        Dim hInternet As IntPtr = IntPtr.Zero
        Try
            If Not String.IsNullOrEmpty(agentName) Then
                hInternet = NativeMethods.InternetOpen(agentName, CInt(Fix(INTERNET_OPEN_TYPE.INTERNET_OPEN_TYPE_DIRECT)), Nothing, Nothing, 0)
            End If

            Return SetConnectionProxyInternal(hInternet, proxyServer)
        Finally
            If hInternet <> IntPtr.Zero Then
                NativeMethods.InternetCloseHandle(hInternet)
            End If
        End Try
    End Function

    ''' <summary>
    ''' Set the proxy server for LAN connection.
    ''' </summary>
    Shared Function SetConnectionProxyInternal(ByVal hInternet As IntPtr, ByVal proxyServer As String) As Boolean

        ' Create 3 options.
        Dim Options(2) As INTERNET_PER_CONN_OPTION

        ' Set PROXY flags.
        Options(0) = New INTERNET_PER_CONN_OPTION()
        Options(0).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_FLAGS))
        Options(0).Value.dwValue = CInt(Fix(INTERNET_OPTION_PER_CONN_FLAGS.PROXY_TYPE_PROXY))

        ' Set proxy name.
        Options(1) = New INTERNET_PER_CONN_OPTION()
        Options(1).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_SERVER))
        Options(1).Value.pszValue = Marshal.StringToHGlobalAnsi(proxyServer)

        ' Set proxy bypass.
        Options(2) = New INTERNET_PER_CONN_OPTION()
        Options(2).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_BYPASS))
        Options(2).Value.pszValue = Marshal.StringToHGlobalAnsi("local")

        ' Allocate a block of memory of the options.
        Dim buffer As System.IntPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(Options(0)) + Marshal.SizeOf(Options(1)) + Marshal.SizeOf(Options(2)))

        Dim current As System.IntPtr = buffer

        ' Marshal data from a managed object to an unmanaged block of memory.
        For i As Integer = 0 To Options.Length - 1
            Marshal.StructureToPtr(Options(i), current, False)
            current = CType(CInt(current) + Marshal.SizeOf(Options(i)), System.IntPtr)
        Next i

        ' Initialize a INTERNET_PER_CONN_OPTION_LIST instance.
        Dim option_list As New INTERNET_PER_CONN_OPTION_LIST()

        ' Point to the allocated memory.
        option_list.pOptions = buffer

        ' Return the unmanaged size of an object in bytes.
        option_list.Size = Marshal.SizeOf(option_list)

        ' IntPtr.Zero means LAN connection.
        option_list.Connection = IntPtr.Zero

        option_list.OptionCount = Options.Length
        option_list.OptionError = 0
        Dim size As Integer = Marshal.SizeOf(option_list)

        ' Allocate memory for the INTERNET_PER_CONN_OPTION_LIST instance.
        Dim intptrStruct As IntPtr = Marshal.AllocCoTaskMem(size)

        ' Marshal data from a managed object to an unmanaged block of memory.
        Marshal.StructureToPtr(option_list, intptrStruct, True)

        ' Set internet settings.
        Dim bReturn As Boolean = NativeMethods.InternetSetOption( _
            hInternet, INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION, intptrStruct, size)

        ' Free the allocated memory.
        Marshal.FreeCoTaskMem(buffer)
        Marshal.FreeCoTaskMem(intptrStruct)

        ' Throw an exception if this operation failed.
        If Not bReturn Then
            Throw New ApplicationException(" Set Internet Option Failed!")
        End If

        ' Notify the system that the registry settings have been changed and cause
        ' the proxy data to be reread from the registry for a handle.
        NativeMethods.InternetSetOption(hInternet, INTERNET_OPTION.INTERNET_OPTION_SETTINGS_CHANGED, IntPtr.Zero, 0)

        NativeMethods.InternetSetOption(hInternet, INTERNET_OPTION.INTERNET_OPTION_REFRESH, IntPtr.Zero, 0)

        Return bReturn
    End Function

    ''' <summary>
    ''' Get the current system options for LAN connection.
    ''' Make sure free the memory after restoration. 
    ''' </summary>
    Public Shared Function GetSystemProxy() As INTERNET_PER_CONN_OPTION_LIST

        ' Query following options. 
        Dim Options(2) As INTERNET_PER_CONN_OPTION

        Options(0) = New INTERNET_PER_CONN_OPTION()
        Options(0).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_FLAGS))
        Options(1) = New INTERNET_PER_CONN_OPTION()
        Options(1).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_SERVER))
        Options(2) = New INTERNET_PER_CONN_OPTION()
        Options(2).dwOption = CInt(Fix(INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_BYPASS))

        ' Allocate a block of memory of the options.
        Dim buffer As System.IntPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(Options(0)) + Marshal.SizeOf(Options(1)) + Marshal.SizeOf(Options(2)))

        Dim current As System.IntPtr = CType(buffer, System.IntPtr)

        ' Marshal data from a managed object to an unmanaged block of memory.
        For i As Integer = 0 To Options.Length - 1
            Marshal.StructureToPtr(Options(i), current, False)
            current = CType(CInt(current) + Marshal.SizeOf(Options(i)), System.IntPtr)
        Next i

        ' Initialize a INTERNET_PER_CONN_OPTION_LIST instance.
        Dim Request As New INTERNET_PER_CONN_OPTION_LIST()

        ' Point to the allocated memory.
        Request.pOptions = buffer

        Request.Size = Marshal.SizeOf(Request)

        ' IntPtr.Zero means LAN connection.
        Request.Connection = IntPtr.Zero

        Request.OptionCount = Options.Length
        Request.OptionError = 0
        Dim size As Integer = Marshal.SizeOf(Request)

        ' Query system internet options. 
        Dim result As Boolean = NativeMethods.InternetQueryOption(IntPtr.Zero, INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION, Request, size)

        If Not result Then
            Throw New ApplicationException("Get System Internet Option Failed! ")
        End If

        Return Request
    End Function

    ''' <summary>
    ''' Restore to the system proxy settings.
    ''' </summary>
    Public Shared Function RestoreSystemProxy() As Boolean
        Return RestoreSystemProxy(_agent)
    End Function

    ''' <summary>
    ''' Restore to the system proxy settings.
    ''' </summary>
    Public Shared Function RestoreSystemProxy(ByVal agentName As String) As Boolean
        If String.IsNullOrEmpty(agentName) Then
            Throw New ArgumentNullException("Agent name cannot be null or empty!")
        End If

        Dim hInternet As IntPtr = IntPtr.Zero
        Try
            If Not String.IsNullOrEmpty(agentName) Then
                hInternet = NativeMethods.InternetOpen(agentName, CInt(Fix(INTERNET_OPEN_TYPE.INTERNET_OPEN_TYPE_DIRECT)), Nothing, Nothing, 0)
            End If

            Return RestoreSystemProxyInternal(hInternet)
        Finally
            If hInternet <> IntPtr.Zero Then
                NativeMethods.InternetCloseHandle(hInternet)
            End If
        End Try
    End Function

    ''' <summary>
    ''' Restore to the system proxy settings.
    ''' </summary>
    Shared Function RestoreSystemProxyInternal(ByVal hInternet As IntPtr) As Boolean
        Dim request = GetSystemProxy()

        Dim size As Integer = Marshal.SizeOf(request)

        ' Allocate memory. 
        Dim intptrStruct As IntPtr = Marshal.AllocCoTaskMem(size)

        ' Convert structure to IntPtr 
        Marshal.StructureToPtr(request, intptrStruct, True)

        ' Set internet options.
        Dim bReturn As Boolean = NativeMethods.InternetSetOption(hInternet, INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION, intptrStruct, size)

        ' Free the allocated memory.
        Marshal.FreeCoTaskMem(request.pOptions)
        Marshal.FreeCoTaskMem(intptrStruct)

        If Not bReturn Then
            Throw New ApplicationException(" Set Internet Option Failed! ")
        End If

        ' Notify the system that the registry settings have been changed and cause
        ' the proxy data to be reread from the registry for a handle.
        NativeMethods.InternetSetOption(hInternet, INTERNET_OPTION.INTERNET_OPTION_SETTINGS_CHANGED, IntPtr.Zero, 0)

        NativeMethods.InternetSetOption(hInternet, INTERNET_OPTION.INTERNET_OPTION_REFRESH, IntPtr.Zero, 0)
        Return bReturn
    End Function

End Class
