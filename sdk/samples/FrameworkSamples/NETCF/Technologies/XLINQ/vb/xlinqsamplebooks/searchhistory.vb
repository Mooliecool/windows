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
Imports System.Collections.Specialized
Imports System.Globalization

''' <summary>
''' This class holds the history of list of Unified Searches done by the user
''' </summary>
Class SearchHistory


    ''' <summary>
    ''' private List of string that holds the search history
    ''' </summary>
    Private values As List(Of String)


    ''' <summary>
    ''' public property that returns the last 5 search queries
    ''' </summary>
    Public ReadOnly Property SearchHistoryValues() As NameValueCollection
        Get
            'create a new NameValueCollection
            Dim searchHistory As NameValueCollection = New NameValueCollection()

            Dim i As Integer = 0
            'take top 5 in the list and add it to the NameValueCollection
            For Each s In values.Take(5).ToList()
                Dim key As String = String.Format(CultureInfo.CurrentCulture, "search{0}", i)
                i = i + 1
                searchHistory.Add(key, s)
            Next
            Return searchHistory
        End Get

    End Property


    ''' <summary>
    ''' Constructor for the class
    ''' </summary>
    ''' <param name="history"></param>
    Public Sub New(ByVal history As NameValueCollection)

        'add the search queries to the list (only distinct ones)
        values = New List(Of String)()
        For Each key In history.Keys.OfType(Of String).Distinct()
            values.Add(history(key))
        Next

    End Sub

    ''' <summary>
    ''' Adds a new entry to the top of the list
    ''' </summary>
    ''' <param name="keyword"></param>
    Public Sub AddEntry(ByVal keyword As String)
        'if the list does not contain the keyword, then add it
        If (Not values.Contains(keyword)) Then
            values.Insert(0, keyword)
        End If

    End Sub


    ''' <summary>
    ''' Returns the match string if it exists in the list
    ''' </summary>
    ''' <param name="s"></param>
    ''' <returns></returns>
    Public Function Match(ByVal s As String) As String
        'call the Find method and pass it the Lambda Expression
        'lambda expression is v => v.ToLower().StartsWith(s.ToLower())
        Dim m As String = values.Find(Function(v As String) v.ToLower().StartsWith(s.ToLower()))

        'return empty if nothing found.
        If (m Is Nothing) Then
            Return String.Empty
        Else
            Return m
        End If

    End Function

End Class
