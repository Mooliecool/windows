'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBLoadLibrary
' Copyright (c) Microsoft Corporation.
' 
' VBLoadLibrary in VB.NET mimics the behavior of CppLoadLibrary to 
' dynamically load a native DLL (LoadLibrary) get the address of a function 
' in the export table (GetProcAddress, Marshal.GetDelegateForFunctionPointer), 
' and call it. The technology is called Dynamic P/Invoke. It serves as a 
' supplement for the P/Invoke technique and is useful especially when the 
' target DLL is not in the search path of P/Invoke. If you use P/Invoke, CLR 
' will search the dll in your assembly's directory first, then search the dll 
' in directories listed in PATH environment variable. If the dll is not in 
' any of those directories, you have to use the so called Dynamic PInvoke 
' technique that is demonstrated in this code sample. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Runtime.InteropServices


Module MainModule

#Region "Function Delegates"

    ' Function delegate of GetStringLength1 exported from the DLL module.
    ' The string parameter must be marshaled as LPWStr, otherwise, the
    ' string will be passed into the native as an ANSI string that the
    ' Unicode API cannot resolve appropriately.
    Delegate Function GetStringLength1Delegate( _
        <MarshalAs(UnmanagedType.LPWStr)> ByVal str As String) As Integer

    ' Function delegate of GetStringLength2 exported from the DLL module.
    Delegate Function GetStringLength2Delegate( _
        <MarshalAs(UnmanagedType.LPWStr)> ByVal str As String) As Integer

    ' Function delegate of the 'PFN_COMPARE' callback function, and the Max 
    ' function that requires the callback as one of the arguments.
    <UnmanagedFunctionPointer(CallingConvention.StdCall)> _
    Delegate Function CompareCallback( _
        ByVal a As Integer, _
        ByVal b As Integer) _
        As Integer

    Delegate Function MaxDelegate( _
        ByVal a As Integer, _
        ByVal b As Integer, _
        ByVal cmpFunc As CompareCallback) _
        As Integer

#End Region

    Sub Main()

        Dim isLoaded As Boolean = False
        Const moduleName As String = "CppDynamicLinkLibrary"

        ' Check whether or not the module is loaded.
        isLoaded = IsModuleLoaded(moduleName)
        Console.WriteLine("Module ""{0}"" is {1}loaded", moduleName, _
            If(isLoaded, "", "not "))

        ' Load the DLL module.
        Console.WriteLine("Load the library")
        Using library As New UnmanagedLibrary(moduleName)

            ' Check whether or not the module is loaded.
            isLoaded = IsModuleLoaded(moduleName)
            Console.WriteLine("Module ""{0}"" is {1}loaded", moduleName, _
                If(isLoaded, "", "not "))

            '
            ' Access the global data exported from the module.
            '

            ' The solution does not allow you to access the global data 
            ' exported from a DLL module.

            '
            ' Call the functions exported from the module.
            '

            Dim str As String = "HelloWorld"
            Dim length As Integer

            ' Call int /*__cdecl*/ GetStringLength1(PWSTR pszString);
            Dim GetStringLength1 As GetStringLength1Delegate = _
                library.GetUnmanagedFunction(Of GetStringLength1Delegate) _
                ("GetStringLength1")
            If (GetStringLength1 Is Nothing) Then
                Throw New EntryPointNotFoundException( _
                    "Unable to find an entry point named 'GetStringLength1'")
            End If
            length = GetStringLength1(str)
            Console.WriteLine("GetStringLength1(""{0}"") => {1}", str, length)

            ' Call int __stdcall GetStringLength2(PWSTR pszString);
            Dim GetStringLength2 As GetStringLength2Delegate = _
                library.GetUnmanagedFunction(Of GetStringLength2Delegate) _
                ("_GetStringLength2@4")
            If (GetStringLength2 Is Nothing) Then
                Throw New EntryPointNotFoundException( _
                    "Unable to find an entry point named 'GetStringLength2'")
            End If
            length = GetStringLength2(str)
            Console.WriteLine("GetStringLength2(""{0}"") => {1}", str, length)

            '
            ' Call the callback functions exported from the module.
            '

            Dim cmpFunc As New CompareCallback(AddressOf CompareInts)
            Dim Max As MaxDelegate = library.GetUnmanagedFunction(Of MaxDelegate)("Max")
            If (Max Is Nothing) Then
                Throw New EntryPointNotFoundException( _
                    "Unable to find an entry point named 'Max'")
            End If
            Dim maxVal As Integer = Max(2, 3, cmpFunc)
            Console.WriteLine("Function: Max(2, 3) => {0}", maxVal)

            '
            ' Use the class exported from a module.
            '

            ' The solution does not allow you to use the class exported from  
            ' the DLL.

            ' Attempt to free the library on exit.
            Console.WriteLine("Unload the dynamically-loaded DLL")

        End Using ' The native module should be unloaded here.

        ' Check whether or not the module is loaded.
        isLoaded = IsModuleLoaded(moduleName)
        Console.WriteLine("Module ""{0}"" is {1}loaded", moduleName, _
            If(isLoaded, "", "not "))

    End Sub


    ''' <summary>
    ''' This is the callback function for the method Max exported from the 
    ''' DLL CppDynamicLinkLibrary.dll
    ''' </summary>
    ''' <param name="a">the first integer</param>
    ''' <param name="b">the second integer</param>
    ''' <returns>
    ''' The function returns a positive number if a > b, returns 0 if a 
    ''' equals b, and returns a negative number if a &lt; b.
    ''' </returns>
    Function CompareInts(ByVal a As Integer, ByVal b As Integer) As Integer
        Return (a - b)
    End Function


#Region "IsModuleLoaded"

    ''' <summary>
    ''' Check whether or not the specified module is loaded in the current 
    ''' process.
    ''' </summary>
    ''' <param name="moduleName">the module name</param>
    ''' <returns>
    ''' The function returns true if the specified module is loaded in the 
    ''' current process. If the module is not loaded, the function returns
    ''' false.
    ''' </returns>
    Function IsModuleLoaded(ByVal moduleName As String) As Boolean
        ' Get the module in the process according to the module name.
        Dim hMod As IntPtr = GetModuleHandle(moduleName)
        Return (hMod <> IntPtr.Zero)
    End Function

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto)> _
    Function GetModuleHandle(ByVal moduleName As String) As IntPtr
    End Function

#End Region

End Module
