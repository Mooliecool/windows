''---------------------------------------------------------------------
''  This file is part of the Microsoft .NET Framework SDK Code Samples.
'' 
''  Copyright (C) Microsoft Corporation.  All rights reserved.
'' 
''This source code is intended only as a supplement to Microsoft
''Development Tools and/or on-line documentation.  See these other
''materials for detailed information regarding Microsoft code samples.
'' 
''THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
''KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
''IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
''PARTICULAR PURPOSE.
''---------------------------------------------------------------------

Imports System.IO
Imports System.Linq
Imports System.Collections.Generic
Imports System.Runtime.CompilerServices



''' <summary>
''' Module that holds custom Extension Methods
''' </summary>
Module ExtensionMethods


    ''' <summary>
    ''' Extension Method for IEnumerable(Of FileInfo ) which enumerates the items and creates new TreeNodes and adds it to the root TreeNode
    ''' </summary>
    ''' <param name="source"></param>
    ''' <param name="root"></param>
    ''' <remarks></remarks>
    <Extension()> _
    Public Sub AddNodes(ByVal source As IEnumerable(Of FileInfo), ByVal root As TreeNode)

        'for every FileInfo in the source
        For Each fi As FileInfo In source

            'Create and add new TreeNode to root
            Dim n As TreeNode = root.Nodes.Add(fi.Name)

            'set appropriate image (directory or just a file)
            If ((fi.Attributes And FileAttributes.Directory) = FileAttributes.Directory) Then
                n.ImageIndex = 0
            Else
                n.ImageIndex = 1
            End If

        Next


        'expand the root
        root.Expand()

    End Sub

End Module


