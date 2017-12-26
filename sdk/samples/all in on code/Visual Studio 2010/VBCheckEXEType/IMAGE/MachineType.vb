'*************************** Module Header ******************************'
' Module Name:  MachineType.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Represents the machine types. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Namespace IMAGE


    Public Enum MachineType
        Native = 0
        I386 = &H14C
        Itanium = &H200
        x64 = &H8664
    End Enum
End Namespace