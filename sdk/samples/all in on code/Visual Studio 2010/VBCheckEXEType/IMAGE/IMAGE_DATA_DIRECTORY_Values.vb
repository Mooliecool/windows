'*************************** Module Header ******************************'
' Module Name:  IMAGE_DATA_DIRECTORY_Values.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' The DataDirectory is an array of 16 structures. Each array entry has a 
' predefined meaning for what it refers to. The IMAGE_DIRECTORY_ENTRY_ xxx 
' #defines are array indexes into the DataDirectory (from 0 to 15).
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

    Public Class IMAGE_DATA_DIRECTORY_Values
        ' Export Directory
        Public Const IMAGE_DIRECTORY_ENTRY_EXPORT As Integer = 0

        ' Import Directory
        Public Const IMAGE_DIRECTORY_ENTRY_IMPORT As Integer = 1

        ' Resource Directory
        Public Const IMAGE_DIRECTORY_ENTRY_RESOURCE As Integer = 2

        ' Exception Directory
        Public Const IMAGE_DIRECTORY_ENTRY_EXCEPTION As Integer = 3

        ' Security Directory
        Public Const IMAGE_DIRECTORY_ENTRY_SECURITY As Integer = 4

        ' Base Relocation Table
        Public Const IMAGE_DIRECTORY_ENTRY_BASERELOC As Integer = 5

        ' Debug Directory
        Public Const IMAGE_DIRECTORY_ENTRY_DEBUG As Integer = 6

        ' Architecture Specific Data
        Public Const IMAGE_DIRECTORY_ENTRY_ARCHITECTURE As Integer = 7

        ' RVA of GP
        Public Const IMAGE_DIRECTORY_ENTRY_GLOBALPTR As Integer = 8

        ' TLS Directory
        Public Const IMAGE_DIRECTORY_ENTRY_TLS As Integer = 9

        ' Load Configuration Directory
        Public Const IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG As Integer = 10

        ' Bound Import Directory in headers
        Public Const IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT As Integer = 11

        ' Import Address Table
        Public Const IMAGE_DIRECTORY_ENTRY_IAT As Integer = 12

        ' Delay Load Import Descriptors
        Public Const IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT As Integer = 13

        ' COM Runtime descriptor 
        Public Const IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR As Integer = 14

        Public Property Values() As IMAGE_DATA_DIRECTORY()

        Public Sub New()
            Values = New IMAGE_DATA_DIRECTORY(15) {}
        End Sub
    End Class

End Namespace

