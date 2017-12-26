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

Imports System.IO
Imports System.Globalization
Imports System.Security.AccessControl

Namespace Microsoft.Samples.FileDemo

    Friend Enum FileItemType
        File
        Directory
        Other
    End Enum

    Friend Enum DefaultPropertiesView
        General
        Security
        Other
    End Enum

    Friend Enum FileRightShortCuts
        Read = FileSystemRights.ListDirectory Or FileSystemRights.ReadAttributes Or _
                FileSystemRights.ReadExtendedAttributes Or FileSystemRights.ReadPermissions
        ReadAndExecute = FileRightShortCuts.Read Or FileSystemRights.Traverse
        Write = FileSystemRights.WriteData Or FileSystemRights.AppendData Or _
                    FileSystemRights.WriteExtendedAttributes Or _
                    FileSystemRights.WriteAttributes
        Modify = FileRightShortCuts.ReadAndExecute Or FileRightShortCuts.Write Or _
                FileSystemRights.Delete
        FullControl = FileSystemRights.FullControl
    End Enum

    Module General

        Public Enum DemoCategories
            Created = 1
            Moved = 2
            Deleted = 3
        End Enum

        Public EnvironmentChanged As Boolean
        Public curDir As String
        Public NewName As String
        Public NewValue As String

        Public Function GetDriveType(ByVal d As DriveInfo, ByVal network As Boolean) As String

            Dim typeName As String

            Select Case d.DriveType
                Case DriveType.Removable
                    typeName = "Floppy"
                Case DriveType.CDRom
                    typeName = "CD Drive"
                Case DriveType.Fixed
                    typeName = "Local Disk"
                Case DriveType.Network
                    If Not network Then
                        typeName = "Network Drive"
                    Else
                        typeName = "Unknown"
                    End If
                Case Else
                    typeName = "Unknown"
            End Select

            Return typeName

        End Function

        Public Function GetDriveIndex(ByVal d As DriveInfo) As Integer

            Select Case d.DriveType
                Case DriveType.Removable
                    Return 2
                Case DriveType.CDRom
                    Return 0
                Case DriveType.Fixed
                    Return 5
                Case DriveType.Network
                    Return 7
            End Select

            Return 5

        End Function

        Public Function GetNameAndType(ByVal d As DriveInfo) As String
            Return String.Format(CultureInfo.CurrentCulture, "{0} ({1})", GetDriveType(d, False), d.Name.Substring(0, 2))
        End Function
    End Module
End Namespace