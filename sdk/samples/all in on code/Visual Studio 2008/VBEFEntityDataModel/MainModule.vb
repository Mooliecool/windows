'****************************** Module Header ******************************'
' Module Name:    MainModule.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' The VBEFEntityDataModel example illustrates how to work with EDM in various 
' ways. It includes many to many association, one to many association, one to
' one association, table merging, table splitting, table per hierarchy 
' inheritance, and table per type inheritance. In the example, you will see 
' the insert, update and query operations to entities.
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
Imports VBEFEntityDataModel.VBEFEntityDataModel.TableSplitting
Imports VBEFEntityDataModel.VBEFEntityDataModel.TablePerType
Imports VBEFEntityDataModel.VBEFEntityDataModel.TablePerHierarchy
Imports VBEFEntityDataModel.VBEFEntityDataModel.TableMerging
Imports VBEFEntityDataModel.VBEFEntityDataModel.One2One
Imports VBEFEntityDataModel.VBEFEntityDataModel.One2Many
Imports VBEFEntityDataModel.VBEFEntityDataModel.Many2Many
#End Region


Module MainModule
    Sub Main()

        Console.WriteLine("Many-to-Many test:")

        ' Perform the insert, update and query operations to entities  
        ' with many to many association.
        Many2ManyClass.Many2ManyTest()

        Console.WriteLine()

        Console.WriteLine("One-to-Many test:")

        ' Perform the insert, update and query operations to entities  
        ' with one to many association.
        One2ManyClass.One2ManyTest()

        Console.WriteLine()

        Console.WriteLine("One-to-One test:")

        ' Perform the insert, update and query operations to entities  
        ' with one to one association.
        One2OneClass.One2OneTest()

        Console.WriteLine()

        Console.WriteLine("Table merging test:")

        ' Perform the query operation to merged entity
        TableMergingClass.TableMergingTest()

        Console.WriteLine()

        Console.WriteLine("Table splitting test:")

        ' Perform the insert and query operations in separate table
        TableSplittingClass.TableSplittingTest()

        Console.WriteLine()

        Console.WriteLine("Table-Per-Hierarchy inheritance test:")

        ' Perform the query operation to entities in TablePerType
        TPTClass.TPTTest()

        Console.WriteLine()

        Console.WriteLine("Table-Per-Type inheritance test:")

        ' Perform the query operation to entities in TablePerHierarchy
        TPHClass.TPHTest()

        Console.ReadLine()
    End Sub

End Module
