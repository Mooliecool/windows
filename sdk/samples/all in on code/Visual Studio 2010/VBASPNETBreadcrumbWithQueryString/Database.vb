'****************************** Module Header ******************************\
' Module Name:    Database.vb
' Project:        VBASPNETBreadcrumbWithQueryString
' Copyright (c) Microsoft Corporation
'
' This is a very simple in-code database for demo purpose. It is not the point 
' of this code sample project.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

''' <summary>
''' This is a very simple in-code database for demo purpose.
''' </summary>
Public NotInheritable Class Database
    Private Sub New()
    End Sub

    Public Shared Property Categories As List(Of String)
    Public Shared Property Items As List(Of KeyValuePair(Of String, String))

    Shared Sub New()
        Categories = New List(Of String)() From { _
         "Category1", _
         "Category2", _
         "Category3" _
        }
        Items = New List(Of KeyValuePair(Of String, String))()
        Items.Add(New KeyValuePair(Of String, String)("Category1", "Item1"))
        Items.Add(New KeyValuePair(Of String, String)("Category1", "Item2"))
        Items.Add(New KeyValuePair(Of String, String)("Category1", "Item3"))
        Items.Add(New KeyValuePair(Of String, String)("Category2", "Item4"))
        Items.Add(New KeyValuePair(Of String, String)("Category2", "Item5"))
        Items.Add(New KeyValuePair(Of String, String)("Category2", "Item6"))
        Items.Add(New KeyValuePair(Of String, String)("Category3", "Item7"))
        Items.Add(New KeyValuePair(Of String, String)("Category3", "Item8"))
        Items.Add(New KeyValuePair(Of String, String)("Category3", "Item9"))
    End Sub

    Public Shared Function GetCategoryByItem(ByVal item As String) As String
        For i As Integer = 0 To Items.Count - 1
            If Items(i).Value = item Then
                Return Items(i).Key
            End If
        Next
        Return String.Empty
    End Function

    Public Shared Function GetItemsByCategory(ByVal category As String) As List(Of String)
        Dim list As New List(Of String)()
        For i As Integer = 0 To Items.Count - 1
            If Items(i).Key = category Then
                list.Add(Items(i).Value)
            End If
        Next
        Return list
    End Function

End Class