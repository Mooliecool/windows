'****************************** Module Header ******************************\
' Module Name:	MainModule.vb
' Project:		VBCheckOSVersion
' Copyright (c) Microsoft Corporation.
' 
' The VBCheckOSVersion sample demonstrates how to detect the version of the 
' current operating system, and how to make application that checks for the 
' minimum operating system version work with later operating system versions.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Module MainModule

    Sub Main()

        ' Detect the current OS version.
        Console.WriteLine("Current OS: {0}", Environment.OSVersion.VersionString)

        ' Make application that checks for the minimum operating system 
        ' version work with later operating system versions. (For example, 
        ' Check if the current OS is at least Windows XP.)
        If (Environment.OSVersion.Version < New Version(5, 1)) Then
            Console.WriteLine("Windows XP or later required.")
            ' Quit the application due to incompatible OS
            Return
        End If

        Console.WriteLine("Application Running...")
        Console.Read()

    End Sub

End Module
