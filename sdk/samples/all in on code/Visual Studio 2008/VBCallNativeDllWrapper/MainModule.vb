'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBCallNativeDllWrapper
' Copyright (c) Microsoft Corporation.
' 
' The code sample demonstrates calling the functions and classes exported by 
' a native C++ DLL from VB.NET code through C++/CLI wrapper classes.
' 
'   VBCallNativeDllWrapper (this .NET application)
'           -->
'       CppCLINativeDllWrapper (the C++/CLI wrapper)
'               -->
'           CppDynamicLinkLibrary (a native C++ DLL module)
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

Imports System.Runtime.InteropServices
Imports CppCLINativeDllWrapper

#End Region


Module MainModule

    Sub Main()

        Dim isLoaded As Boolean = False
        Const moduleName As String = "CppDynamicLinkLibrary"

        ' Check whether or not the module is loaded.
        isLoaded = IsModuleLoaded(moduleName)
        Console.WriteLine("Module ""{0}"" is {1}loaded", moduleName, _
            If(isLoaded, "", "not "))

        '
        ' Call the functions exported from the module.
        '
        Dim str As String = "HelloWorld"
        Dim length As Integer

        length = NativeMethods.GetStringLength1(str)
        Console.WriteLine("GetStringLength1(""{0}"") => {1}", str, length)

        length = NativeMethods.GetStringLength2(str)
        Console.WriteLine("GetStringLength2(""{0}"") => {1}", str, length)

        '
        ' Call the callback functions exported from the module.
        '

        ' P/Invoke a method that requires callback as one of the args.
        Dim cmpFunc As CompareCallback = New CompareCallback(AddressOf CompareInts)
        Dim max As Integer = NativeMethods.Max(2, 3, cmpFunc)

        ' Make sure the lifetime of the delegate instance covers the lifetime 
        ' of the unmanaged code; otherwise, the delegate will not be 
        ' available after it is garbage-collected, and you may get the Access
        ' Violation or Illegal Instruction error.
        GC.KeepAlive(cmpFunc)
        Console.WriteLine("Max(2, 3) => {0}", max)

        '
        ' Use the class exported from the module.
        '

        Dim obj As New CSimpleObjectWrapper
        obj.FloatProperty = 1.2F
        Dim fProp As Single = obj.FloatProperty
        Console.WriteLine("Class: CSimpleObject::FloatProperty = {0:F2}", fProp)

        ' You cannot unload the C++ DLL CppDynamicLinkLibrary by calling 
        ' GetModuleHandle and FreeLibrary.

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


#Region "Module Related Helper Functions"

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

    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Function GetModuleHandle(ByVal moduleName As String) As IntPtr
    End Function

#End Region

End Module
